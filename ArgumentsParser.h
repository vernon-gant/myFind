#pragma once

#include <regex>
#include <string>
#include <utility>
#include <vector>

typedef struct parsed_args {
    std::string path;
    std::vector<std::string> file_names, options;
} parsed_args_t;

// Config object derived from arguments
typedef struct search_config {
    std::string path;
    std::vector<std::string> file_names;
    bool is_recursive = false, is_case_sensitive = true;
} search_config_t;

class ArgumentsParser {
private:
    std::vector<std::string> _args;
    std::string _path;
    std::vector<std::string> _file_names, _options;

    void parse_options() {
        _args.erase(std::remove_if(_args.begin(), _args.end(),
                                   [this](const std::string &arg) {
                                       if (arg[0] == '-') {
                                           _options.emplace_back(arg);
                                           return true;
                                       }
                                       return false;
                                   }),
                    _args.end());
    }

    void parse_path() {
        if (_args.empty()) return;

        _path = _args.front();
        _args.erase(_args.begin());
    }

    void parse_filenames() {
        if (_args.empty()) return;

        _file_names.assign(_args.begin(), _args.end());
    }


public:
    static const std::map<std::string, std::function<void(search_config_t &)>> VALID_OPTIONS;

    explicit ArgumentsParser(std::vector<std::string> args) {
        _args = std::move(args);
    }

    // Main method to call, returns whether given argument list is okay or not and fills out the data in the object
    // If returns true then you may call get_search_config which will deliver config
    parsed_args_t parse() {
        parse_options();
        parse_path();
        parse_filenames();
        return {_path, _file_names, _options};
    }

    search_config_t get_search_config() {
        search_config config = {.path =  _path, .file_names = _file_names};
        for (std::string &option: _options) VALID_OPTIONS.at(option)(config);
        return config;
    }
};

const std::map<std::string, std::function<void(search_config_t &)>> ArgumentsParser::VALID_OPTIONS{
        {"-R", [](search_config_t &config) { config.is_recursive = true; }},
        {"-i", [](search_config_t &config) { config.is_case_sensitive = false; }}
};