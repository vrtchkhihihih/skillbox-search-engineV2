#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace searchlib {

struct Entry {
    size_t doc_id;
    size_t count;
};

class InvertedIndex {
public:
    InvertedIndex() = default;

    void UpdateDocumentBase(const std::vector<std::string>& texts);
    std::vector<Entry> GetWordCount(const std::string& word) const;
    size_t GetDocsCount() const { return documents_.size(); }

private:
    std::vector<std::string> documents_;
    std::unordered_map<std::string, std::vector<Entry>> freq_dictionary_;
};

} // namespace searchlib
