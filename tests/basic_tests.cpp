#include <gtest/gtest.h>
#include "searchlib/InvertedIndex.hpp"
#include "searchlib/SearchServer.hpp"
#include "searchlib/ConverterJSON.hpp"

using namespace searchlib;

TEST(Tokenize, BasicAscii) {
    auto v = ConverterJSON::TokenizeAsciiWords("Milk, sugar!!! salt...");
    ASSERT_EQ((std::vector<std::string>{"milk","sugar","salt"}), v);
}

TEST(Index, WordCount) {
    InvertedIndex idx;
    idx.UpdateDocumentBase({"milk sugar milk", "sugar tea"});
    auto m = idx.GetWordCount("milk");
    ASSERT_EQ(2u, m[0].count);
    ASSERT_EQ(0u, m[0].doc_id);
}

TEST(Search, AndIntersection) {
    InvertedIndex idx;
    idx.UpdateDocumentBase({"milk sugar milk", "london is the capital of great britain"});
    SearchServer s(idx);
    auto res = s.Search({"london capital"}, 5);
    ASSERT_EQ(1u, res.size());
    ASSERT_EQ(1u, res[0].size());
    EXPECT_EQ(1u, res[0][0].docid);
    EXPECT_DOUBLE_EQ(1.0, res[0][0].rank);
}
