#pragma once

#include <string>
#include <vector>
#include "FileSearcher.h"

class SearchConfigurator {
public:
    static std::shared_ptr<FileSearcher>
    get_file_searcher(const search_config_t &config, const std::shared_ptr<ProcessHandler> &process_handler,
                      const std::shared_ptr<MessageQueue> &mq) {
        std::unique_ptr<CaseStrategy> case_strategy;
        if (config.is_case_sensitive) {
            case_strategy = std::make_unique<CaseSensitive>(config.file_names);
        } else {
            case_strategy = std::make_unique<CaseInsensitive>(config.file_names);
        }

        std::unique_ptr<DirectorySearchStrategy> dir_strategy;
        if (config.is_recursive) {
            dir_strategy = std::make_unique<RecursiveSearch>(process_handler);
        } else {
            dir_strategy = std::make_unique<NormalSearch>();
        }

        return std::make_shared<FileSearcher>(mq, config.path, process_handler, std::move(dir_strategy),
                                              std::move(case_strategy));
    }
};

