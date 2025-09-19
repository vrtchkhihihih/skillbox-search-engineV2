#include "searchlib/ConverterJSON.hpp"
#include "searchlib/InvertedIndex.hpp"
#include "searchlib/SearchServer.hpp"
#include <iostream>

using namespace searchlib;

int main() {
    try {
        ConverterJSON conv("config.json", "requests.json", "answers.json");
        const auto cfg = conv.GetConfig();
        const auto docs = conv.GetTextDocuments();
        const auto requests = conv.GetRequests();

        InvertedIndex index;
        index.UpdateDocumentBase(docs);

        SearchServer srv(index);
        auto results = srv.Search(requests, conv.GetResponsesLimit());
        conv.PutAnswers(results, requests);

        std::cout << cfg.name << " " << cfg.version << " — OK\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
