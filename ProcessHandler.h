#pragma once

#include <memory>
#include <csignal>
#include <iostream>
#include <wait.h>
#include "Searcher.h"
#include <unordered_set>

class ProcessHandler {

private:
    std::unordered_set<pid_t> running_processes;

public:
    void launch(const std::string &path, const std::shared_ptr<Searcher> &searcher) {
        pid_t new_process_id = fork();
        if (new_process_id == 1) return;
        else if (new_process_id == 0) {
            running_processes.clear();
            searcher->update_path(path);
            searcher->search();
            exit(0);
        }
        running_processes.insert(new_process_id);
    }

    void wait_for_all() {
        int status;
        while (!running_processes.empty()) {
            pid_t done_process = wait(&status);
            if (done_process != -1) running_processes.erase(done_process);
        }
    }
};