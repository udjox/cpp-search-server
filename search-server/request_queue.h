#pragma once
#include <deque>
#include "search_server.h"
#include <cstdint>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        const auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
        AddRequest(result.size());
        return result;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status) {
        const auto result = search_server_.FindTopDocuments(raw_query, status);
        AddRequest(result.size());
        return result;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query) {
        const auto result = search_server_.FindTopDocuments(raw_query);
        AddRequest(result.size());
        return result;
    }

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        uint64_t timestamp;
        int results;
    };
    std::deque<QueryResult> requests_;
    const SearchServer& search_server_;
    int no_results_requests_;
    uint64_t current_time_;
    const static int sec_in_day_ = 1440;

    void AddRequest(int results_num);
};