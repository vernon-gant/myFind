#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

//
typedef struct search_config {
    std::string path;
    std::vector<std::string> file_names;
    bool is_recursive = false, is_case_sensitive = true;
} search_config_t;

class ArgumentsParser {
private:
    std::vector<std::string> _args;
    std::string _path, _error_message;
    std::vector<std::string> _file_names;
    bool _is_recursive = false, _is_case_sensitive = true;

    static std::vector<std::string> args_to_strings(int argc, char *argv[]) {
        std::vector<std::string> args;
        args.reserve(argc);
        for (int i = 0; i < argc; i++) {
            args.emplace_back(argv[i]);
        }
        args.erase(args.begin());
        return args;
    }

    static bool is_valid_file_path(const std::string &path) { return fs::exists(path); }

    static bool is_valid_filename(const std::string &name) {
        std::regex pattern(R"(^[a-zA-Z0-9_.-]+$)");
        return std::regex_match(name, pattern);
    }

    bool validate_path() {
        if (_args.empty()) {
            _error_message = "No search path provided.";
            return false;
        }
        _path = _args[0];
        if (!is_valid_file_path(_args[0])) {
            _error_message = "Invalid search path: " + _path;
            return false;
        }
        _args.erase(_args.begin());
        return true;
    }

    bool process_filenames() {
        if (_args.empty()) {
            _error_message = "No filenames provided.";
            return false;
        }
        for (const std::string &filename: _args) {
            if (!is_valid_filename(filename)) {
                _error_message = "Invalid filename format: " + filename;
                return false;
            }
            _file_names.push_back(filename);
        }
        return !_file_names.empty();
    }

    void handle_flags() {
        _args.erase(std::remove_if(_args.begin(), _args.end(),
                                   [this](const std::string &arg) {
                                       if (arg == "-R") {
                                           _is_recursive = true;
                                           return true;
                                       } else if (arg == "-i") {
                                           _is_case_sensitive = false;
                                           return true;
                                       } else if (arg[0] == '-') return true;
                                       return false;
                                   }),
                    _args.end());
    }

public:
    ArgumentsParser(int argc, char *argv[]) {
        _args = args_to_strings(argc, argv);
    }

    bool valid_arguments() {
        handle_flags();
        return validate_path() && process_filenames();
    }

    const std::string &get_error() { return _error_message; }

    search_config_t get_search_config() {
        if (!_error_message.empty() || _path.empty()) throw std::runtime_error("Invalid operation!");
        return {_path, _file_names, _is_recursive, _is_case_sensitive};
    }
};