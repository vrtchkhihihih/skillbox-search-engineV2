#include "searchlib/ConverterJSON.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <stdexcept>

using nlohmann::json;

namespace searchlib {

static std::string ReadFile(const std::string& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) throw std::runtime_error("Cannot open file: " + path);
    std::string s((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return s;
}

static void WriteFile(const std::string& path, const std::string& s) {
    std::ofstream ofs(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs) throw std::runtime_error("Cannot write file: " + path);
    ofs.write(s.data(), static_cast<std::streamsize>(s.size()));
}

ConverterJSON::ConverterJSON(const std::string& config_path,
                             const std::string& requests_path,
                             const std::string& answers_path)
    : answers_path_(answers_path)
{
    // Read config
    auto cfg_text = ReadFile(config_path);
    auto j = json::parse(cfg_text);

    if (j.contains("config")) {
        auto jc = j["config"];
        if (jc.contains("name"))    config_.name = jc["name"].get<std::string>();
        if (jc.contains("version")) config_.version = jc["version"].get<std::string>();
        if (jc.contains("max_responses")) config_.max_responses = jc["max_responses"].get<size_t>();
    }
    if (j.contains("files")) {
        for (auto& it : j["files"]) config_.files.push_back(it.get<std::string>());
    }

    // Load documents
    docs_.reserve(config_.files.size());
    for (const auto& path : config_.files) {
        try {
            docs_.push_back(ReadFile(path));
        } catch (...) {
            // skip missing files
            docs_.push_back(std::string{});
        }
    }

    // Load requests
    try {
        auto rq_text = ReadFile(requests_path);
        auto rq = json::parse(rq_text);
        if (rq.is_array()) {
            for (auto& it : rq) requests_.push_back(it.get<std::string>());
        }
    } catch (...) {
        // no requests file — okay
    }
}

Config ConverterJSON::GetConfig() const { return config_; }
std::vector<std::string> ConverterJSON::GetTextDocuments() const { return docs_; }
std::vector<std::string> ConverterJSON::GetRequests() const { return requests_; }
size_t ConverterJSON::GetResponsesLimit() const { return config_.max_responses; }

std::vector<std::string> ConverterJSON::TokenizeAsciiWords(const std::string& text) {
    std::vector<std::string> out;
    std::string cur;
    cur.reserve(32);
    size_t words = 0;
    for (unsigned char ch : text) {
        if (std::isalpha(ch)) {
            cur.push_back(static_cast<char>(std::tolower(ch)));
            if (cur.size() > 100) {
                // truncate very long token to avoid memory abuse
                cur.resize(100);
            }
        } else {
            if (!cur.empty()) {
                out.push_back(cur);
                cur.clear();
                if (++words >= 1000) break;
            }
        }
    }
    if (!cur.empty() && words < 1000) out.push_back(cur);
    return out;
}

void ConverterJSON::PutAnswers(const std::vector<std::vector<DocRank>>& ranked,
                               const std::vector<std::string>& requests) const
{
    json out;
    json answers = json::object();
    for (size_t i = 0; i < ranked.size(); ++i) {
        std::string key = (i + 1 < 10 ? "request00" : i + 1 < 100 ? "request0" : "request") + std::to_string(i + 1);
        const auto& res = ranked[i];
        if (res.empty()) {
            answers[key] = { {"result", false} };
        } else {
            json rel = json::array();
            for (const auto& r : res) rel.push_back({ {"docid", r.docid}, {"rank", r.rank} });
            answers[key] = { {"result", true}, {"relevance", rel} };
        }
    }
    out["answers"] = answers;
    WriteFile(answers_path_, out.dump(2));
}

} // namespace searchlib
