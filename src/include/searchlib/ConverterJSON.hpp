#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace searchlib {

struct Config {
    std::string name = "SkillboxSearchEngine";
    std::string version = "1.0.0";
    size_t max_responses = 5;
    std::vector<std::string> files;
};

struct DocRank {
    size_t docid{};
    double rank{};
};

class ConverterJSON {
public:
    explicit ConverterJSON(const std::string& config_path = "config.json",
                           const std::string& requests_path = "requests.json",
                           const std::string& answers_path = "answers.json");

    Config GetConfig() const;
    std::vector<std::string> GetTextDocuments() const;
    std::vector<std::string> GetRequests() const;
    size_t GetResponsesLimit() const;
    void PutAnswers(const std::vector<std::vector<DocRank>>& ranked, const std::vector<std::string>& requests) const;

    static std::vector<std::string> TokenizeAsciiWords(const std::string& text);

private:
    Config config_;
    std::string answers_path_;
    std::vector<std::string> docs_;
    std::vector<std::string> requests_;
};

} // namespace searchlib
