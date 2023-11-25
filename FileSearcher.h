#pragma once

#include "MessageQueue.h"
#include "ProcessHandler.h"
#include "Strategies.h"
#include <memory>
#include <utility>

class FileSearcher : public Searcher, public std::enable_shared_from_this<Searcher> {
private:
    std::shared_ptr<MessageQueue> _mq;
    std::string _path;
    std::unique_ptr<DirectorySearchStrategy> _dir_search_strategy;
    std::unique_ptr<CaseStrategy> _case_search_strategy;
    std::shared_ptr<ProcessHandler> _process_handler;

public:
    FileSearcher(const std::shared_ptr<MessageQueue> &mq, const std::string &path,
                 std::shared_ptr<ProcessHandler> process_handler,
                 std::unique_ptr<DirectorySearchStrategy> dir_search_strategy,
                 std::unique_ptr<CaseStrategy> case_search_strategy) {
        _mq = mq;
        _path = path;
        _process_handler = std::move(process_handler);
        _dir_search_strategy = std::move(dir_search_strategy);
        _case_search_strategy = std::move(case_search_strategy);
    }

    void update_path(const std::string &path) override { _path = path; }

    void search() override {
        try {
            for (const auto &entry: fs::directory_iterator(_path)) {
                if (entry.is_directory()) _dir_search_strategy->search(entry.path(), shared_from_this());
                if (_case_search_strategy->file_found(entry)) {
                    std::string message =
                            std::to_string((int) getpid()) + ": " + entry.path().filename().string() + ": " +
                            entry.path().string() + "\n";
                    _mq->send_message(message);
                }
            }
        } catch (std::filesystem::filesystem_error &e) {
            std::cout << "You do not have permissions for that directory, so the search is not performed there - "
                      << e.what() << std::endl;
        }
        _process_handler->wait_for_all();
    }
};