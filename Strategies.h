#pragma once

#include "ProcessHandler.h"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>


namespace fs = std::filesystem;

// When using case insensitive search we rewrite hashmap function to hash only lowered letters
struct CaseInsensitiveHash {
    size_t operator()(const std::string &str) const {
        std::string lower_str = str;
        std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return std::hash<std::string>{}(lower_str);
    }
};

// When checking presence ignore case again my lowering both letters
struct CaseInsensitiveEqual {
    bool operator()(const std::string &a, const std::string &b) const {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                          [](unsigned char a, unsigned char b) {
                              return std::tolower(a) == std::tolower(b);
                          });
    }
};

class CaseStrategy {
public:
    virtual bool file_found(const fs::directory_entry &entry) = 0;

    virtual ~CaseStrategy() = default;
};

// Uses normal map internally so store all file names from args
class CaseSensitive : public CaseStrategy {
private:
    std::unordered_set<std::string> _file_names;

public:
    explicit CaseSensitive(const std::vector<std::string> &file_names) {
        for (const std::string &file_name: file_names) {
            _file_names.insert(file_name);
        }
    }

    bool file_found(const fs::directory_entry &entry) override {
        return entry.is_regular_file() &&
               _file_names.find(entry.path().filename().string()) !=
               _file_names.end();
    }
};

// Uses modified map so that when we perform search the case if ignored
class CaseInsensitive : public CaseStrategy {
private:
    std::unordered_set<std::string, CaseInsensitiveHash, CaseInsensitiveEqual>
            _file_names;

public:
    explicit CaseInsensitive(const std::vector<std::string> &file_names) {
        for (const std::string &file_name: file_names) {
            _file_names.insert(file_name);
        }
    }

    bool file_found(const fs::directory_entry &entry) override {
        return entry.is_regular_file() &&
               _file_names.find(entry.path().filename().string()) !=
               _file_names.end();
    }
};

class DirectorySearchStrategy {
public:
    virtual void search(const fs::path &path, const std::shared_ptr<Searcher> &searcher) = 0;

    virtual ~DirectorySearchStrategy() = default;
};

// Do nothing as we search only in 1 folder
class NormalSearch : public DirectorySearchStrategy {

public:
    NormalSearch() = default;

    void search(const fs::path &path, const std::shared_ptr<Searcher> &searcher) override {}
};

// Uses process handler to start another process for searching for new found directory
class RecursiveSearch : public DirectorySearchStrategy {
private:
    std::shared_ptr<ProcessHandler> _process_handler;

public:
    explicit RecursiveSearch(std::shared_ptr<ProcessHandler> process_handler) {
        _process_handler = std::move(process_handler);
    }

    void search(const fs::path &path, const std::shared_ptr<Searcher> &searcher) override {
        _process_handler->launch(path, searcher);
    }
};