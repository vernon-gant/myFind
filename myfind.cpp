#include "ArgumentsParser.h"
#include "SearchConfigurator.h"

int main(int argc, char *argv[]) {
    ArgumentsParser parser(argc, argv);

    if (!parser.valid_arguments()) {
        std::cout << parser.get_error() << std::endl;
        return EXIT_FAILURE;
    }

    std::shared_ptr<ProcessHandler> process_handler = std::make_shared<ProcessHandler>();
    std::shared_ptr<MessageQueue> mq = std::make_shared<MessageQueue>();
    search_config_t config = parser.get_search_config();
    std::shared_ptr<Searcher> file_searcher = SearchConfigurator::get_file_searcher(config, process_handler, mq);

    pid_t search_pid = fork();

    if (search_pid == 1) return EXIT_FAILURE;
    else if (search_pid == 0) {
        file_searcher->search();
        mq->send_message(mq->END_OF_MESSAGES_MARKER);
        return EXIT_SUCCESS;
    }

    std::vector<std::string> messages;
    mq->receive_messages(messages);
    std::cout << std::endl;
    for (const std::string &message: messages) {
        std::cout << message << std::endl;
    }
}