#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "FileSearcher.h"

class SearchConfigurator {
private:
    // Static maps for strategy creation
    static std::map<bool, std::function<std::unique_ptr<CaseStrategy>(
            const std::vector<std::string> &)>> caseStrategyMap;
    static std::map<bool, std::function<std::unique_ptr<DirectorySearchStrategy>(
            const std::shared_ptr<ProcessHandler> &)>> dirStrategyMap;

public:
    // Configure main FileSearcher according to config object
    static std::shared_ptr<FileSearcher>
    get_file_searcher(const search_config_t &config, const std::shared_ptr<ProcessHandler> &process_handler,
                      const std::shared_ptr<MessageQueue> &mq) {

        std::unique_ptr<CaseStrategy> case_strategy = caseStrategyMap[config.is_case_sensitive](config.file_names);
        std::unique_ptr<DirectorySearchStrategy> dir_strategy = dirStrategyMap[config.is_recursive](process_handler);

        return std::make_shared<FileSearcher>(mq, config.path, process_handler, std::move(dir_strategy),
                                              std::move(case_strategy));
    }
};

// Maps for mapping config options to Search objects
std::map<bool, std::function<std::unique_ptr<CaseStrategy>(
        const std::vector<std::string> &)>> SearchConfigurator::caseStrategyMap = {
        {true,  [](const std::vector<std::string> &file_names) { return std::make_unique<CaseSensitive>(file_names); }},
        {false, [](const std::vector<std::string> &file_names) {
            return std::make_unique<CaseInsensitive>(file_names);
        }}
};

std::map<bool, std::function<std::unique_ptr<DirectorySearchStrategy>(
        const std::shared_ptr<ProcessHandler> &)>> SearchConfigurator::dirStrategyMap = {
        {true,  [](const std::shared_ptr<ProcessHandler> &handler) {
            return std::make_unique<RecursiveSearch>(handler);
        }},
        {false, [](const std::shared_ptr<ProcessHandler> &handler) { return std::make_unique<NormalSearch>(); }}
};
