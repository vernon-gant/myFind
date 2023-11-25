#pragma once

#include <string>

class Searcher {
public:
    virtual void search() = 0;

    virtual void update_path(const std::string &path) = 0;

    virtual ~Searcher() = default;
};

