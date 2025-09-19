#pragma once
#include "InvertedIndex.hpp"
#include "ConverterJSON.hpp"
#include <vector>
#include <string>

namespace searchlib {

class SearchServer {
public:
    explicit SearchServer(const InvertedIndex& idx) : index_(idx) {}

    std::vector<std::vector<DocRank>> Search(const std::vector<std::string>& queries, size_t max_responses) const;

private:
    const InvertedIndex& index_;
};

} // namespace searchlib
