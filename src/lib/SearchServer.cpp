#include "searchlib/SearchServer.hpp"
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace searchlib {

std::vector<std::vector<DocRank>> SearchServer::Search(const std::vector<std::string>& queries, size_t max_responses) const {
    std::vector<std::vector<DocRank>> results;
    results.reserve(queries.size());

    for (const auto& q : queries) {
        auto tokens = ConverterJSON::TokenizeAsciiWords(q);

        // unique tokens
        std::sort(tokens.begin(), tokens.end());
        tokens.erase(std::unique(tokens.begin(), tokens.end()), tokens.end());

        if (tokens.empty()) {
            results.push_back({});
            continue;
        }

        // AND logic: need documents containing all tokens
        std::unordered_map<size_t, double> abs_rel;
        bool first = true;
        std::unordered_set<size_t> current_docs;

        for (const auto& t : tokens) {
            auto postings = index_.GetWordCount(t);
            std::unordered_set<size_t> docs_with_t;
            for (const auto& e : postings) {
                docs_with_t.insert(e.doc_id);
            }

            if (first) {
                current_docs = std::move(docs_with_t);
                first = false;
            } else {
                // intersection
                std::unordered_set<size_t> inter;
                for (auto id : current_docs) {
                    if (docs_with_t.count(id)) inter.insert(id);
                }
                current_docs = std::move(inter);
            }
        }

        if (current_docs.empty()) {
            results.push_back({});
            continue;
        }

        // accumulate absolute relevance
        for (const auto& t : tokens) {
            auto postings = index_.GetWordCount(t);
            for (const auto& e : postings) {
                if (current_docs.count(e.doc_id)) {
                    abs_rel[e.doc_id] += static_cast<double>(e.count);
                }
            }
        }

        // normalize to relative
        double max_abs = 0.0;
        for (const auto& [id, val] : abs_rel) if (val > max_abs) max_abs = val;
        std::vector<DocRank> ranks;
        ranks.reserve(abs_rel.size());
        for (const auto& [id, val] : abs_rel) {
            ranks.push_back({ id, max_abs > 0.0 ? (val / max_abs) : 0.0 });
        }

        std::sort(ranks.begin(), ranks.end(), [](const DocRank& a, const DocRank& b){
            if (a.rank != b.rank) return a.rank > b.rank;
            return a.docid < b.docid;
        });

        if (ranks.size() > max_responses) ranks.resize(max_responses);
        results.push_back(std::move(ranks));
    }

    return results;
}

} // namespace searchlib
