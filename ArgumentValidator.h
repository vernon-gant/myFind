#pragma once

#include <filesystem>
#include <regex>
#include <utility>
#include "ArgumentsParser.h"

namespace fs = std::filesystem;

class ArgumentValidator {

private:
    parsed_args_t _parsed_args;
    std::string _error_message;

    static bool is_valid_filename(const std::string &name) {
        std::regex valid_filename_pattern(R"(^[a-zA-Z0-9_.-]+$)");
        return std::regex_match(name, valid_filename_pattern);
    }

    bool valid_options() {
        for (const std::string &option: _parsed_args.options) {
            if (ArgumentsParser::VALID_OPTIONS.find(option) == ArgumentsParser::VALID_OPTIONS.end())
                _error_message =
                        "Invalid option : " + option + "\nChoose from -R (recursive) and -i (case insensitive)";
        }
        return _error_message.empty();
    }

    bool valid_path() {
        if (_parsed_args.path.empty()) _error_message = "No search path provided!\nTry again...";
        if (!fs::exists(_parsed_args.path))
            _error_message = "Invalid search path : " + _parsed_args.path + "\nTry again...";

        return _error_message.empty();
    }

    bool valid_filenames() {
        if (_parsed_args.file_names.empty()) _error_message = "No filenames provided for search!\nTry again...";

        for (const std::string &filename: _parsed_args.file_names) {
            if (!is_valid_filename(filename)) {
                _error_message = "Invalid filename format : " + filename + "\nTry again...";
                break;
            }
        }
        return _error_message.empty();
    }

public:

    explicit ArgumentValidator(parsed_args_t parsed_args) { _parsed_args = std::move(parsed_args); }

    bool validate() { return valid_options() && valid_path() && valid_filenames(); }

    const std::string &get_error() { return _error_message; }
};
