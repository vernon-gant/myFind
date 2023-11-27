#pragma once

#include <string>

// Just an interface to avoid circular dependency
class Searcher {
public:
    virtual void search() = 0;

    virtual void update_path(const std::string &path) = 0;

    virtual ~Searcher() = default;
};

