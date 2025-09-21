#include "searchlib/InvertedIndex.hpp"
#include "searchlib/ConverterJSON.hpp"
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <mutex>
#include <vector>

namespace searchlib {

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& texts) {
    documents_ = texts;
    freq_dictionary_.clear();
    
    std::vector<std::thread> threads;

    std::unordered_map<std::string, std::unordered_map<size_t, size_t>> tmp;
    std::mutex tmp_mutex;
    for (size_t doc_id = 0; doc_id < documents_.size(); ++doc_id) {

        threads.emplace_back([this, doc_id, &tmp, &tmp_mutex]() {
            this->ProcessDocument(doc_id, std::ref(tmp), std::ref(tmp_mutex));
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (auto& [word, mp] : tmp) {
        std::vector<Entry> vec;
        vec.reserve(mp.size());
        for (auto& [doc, cnt] : mp) {
            vec.push_back({doc, cnt});
        }
        std::sort(vec.begin(), vec.end(), [](const Entry& a, const Entry& b){
            if (a.count != b.count) return a.count > b.count;
            return a.doc_id < b.doc_id;
        });
        freq_dictionary_.emplace(word, std::move(vec));
    }
}

void InvertedIndex::ProcessDocument(size_t doc_id, 
                                  std::unordered_map<std::string, std::unordered_map<size_t, size_t>>& tmp,
                                  std::mutex& tmp_mutex) {
    const auto tokens = ConverterJSON::TokenizeAsciiWords(documents_[doc_id]);

    std::lock_guard<std::mutex> lock(tmp_mutex);
    for (const auto& word : tokens) {
        tmp[word][doc_id]++;
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {
    auto it = freq_dictionary_.find(word);
    if (it == freq_dictionary_.end()) return {};
    return it->second;
}

} // namespace searchlib
