// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь:

// Закомитьте изменения и отправьте их в свой репозиторий.
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const auto words = ParseQuery(document);

        for(const auto&word : words.plus){
            double tf = 1.0/words.plus.size();
            word_to_document_freqs_[word][document_id] += tf;
        }
        ++document_count_;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        auto query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    struct Query {
        vector<string> minus;
        vector<string> plus;
    };    
    
    int document_count_ = 0;
    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }
    
    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0]=='-'){
                query_words.minus.push_back(word.substr(1));
            }
            else {
                query_words.plus.push_back(word);
            }
        }
        return query_words;
    }

    double IDF (const string& word) const {
        if(word_to_document_freqs_.count(word)==0){
            return 0.0;
        }
        return static_cast<double> (log(document_count_ * 1.0 /word_to_document_freqs_.at(word).size()));
    }
    
    vector<Document> FindAllDocuments(Query& query_words) const {
        vector<Document> matched_documents; 
        map<int, double> document_to_relevance;
        for (const auto& word : query_words.plus) {
            if(word_to_document_freqs_.count(word)==0){
                continue;
            }
                const double idf = IDF(word);
                for (auto& [id, tf]:word_to_document_freqs_.at(word)){
                    document_to_relevance[id] += tf*idf;
                }
        }
       for (const auto& word:query_words.minus){
           if(word_to_document_freqs_.count(word)==0){
               continue;
           }
                for (auto& [id, tf]:word_to_document_freqs_.at(word)){
                    document_to_relevance.erase(tf);
                }
       }

        for (const auto& [document_id, relevance] : document_to_relevance) {

                matched_documents.push_back({document_id, relevance});
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}
