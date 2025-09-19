#include "searchlib/InvertedIndex.hpp"
#include "searchlib/ConverterJSON.hpp"
#include <unordered_map>
#include <algorithm>

namespace searchlib {

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& texts) {
    documents_ = texts;
    freq_dictionary_.clear();
    std::unordered_map<std::string, std::unordered_map<size_t, size_t>> tmp;

    for (size_t doc_id = 0; doc_id < documents_.size(); ++doc_id) {
        const auto tokens = ConverterJSON::TokenizeAsciiWords(documents_[doc_id]);
        for (const auto& w : tokens) {
            tmp[w][doc_id]++;
        }
    }

    for (auto& [word, mp] : tmp) {
        std::vector<Entry> vec;
        vec.reserve(mp.size());
        for (auto& [doc, cnt] : mp) vec.push_back({doc, cnt});
        std::sort(vec.begin(), vec.end(), [](const Entry& a, const Entry& b){
            if (a.count != b.count) return a.count > b.count;
            return a.doc_id < b.doc_id;
        });
        freq_dictionary_.emplace(word, std::move(vec));
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {
    auto it = freq_dictionary_.find(word);
    if (it == freq_dictionary_.end()) return {};
    return it->second;
}

} // namespace searchlib
