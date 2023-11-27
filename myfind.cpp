#include "ArgumentsParser.h"
#include "SearchConfigurator.h"
#include "Utils.h"
#include "ArgumentValidator.h"

int main(int argc, char *argv[]) {
    std::vector<std::string> args = Utils::args_to_strings(argc, argv);
    ArgumentsParser parser(args);
    parsed_args_t parsed_args = parser.parse();
    ArgumentValidator validator(parsed_args);

    if (!validator.validate()) {
        std::cout << validator.get_error() << std::endl;
        return EXIT_FAILURE;
    }

    std::shared_ptr<ProcessHandler> process_handler = std::make_shared<ProcessHandler>();
    std::shared_ptr<MessageQueue> message_queue = std::make_shared<MessageQueue>();
    search_config_t config = parser.get_search_config();
    std::shared_ptr<Searcher> file_searcher = SearchConfigurator::get_file_searcher(config, process_handler,
                                                                                    message_queue);

    pid_t search_pid = fork();

    if (search_pid == 1) return EXIT_FAILURE;
    else if (search_pid == 0) {
        // main child which starts search in the provided directory
        file_searcher->search();
        // Send end message to the queue when has finished what means that all children have finished as well
        // so that main process stops waiting for new messages.
        message_queue->send_message(message_queue->END_OF_MESSAGES_MARKER);
        return EXIT_SUCCESS;
    }

    // While children process search main process waits for new messages
    // from the queue
    std::vector<std::string> messages;
    message_queue->receive_messages(messages);
    std::cout << std::endl;
    // When they all arrived - print them
    for (const std::string &message: messages) {
        std::cout << message << std::endl;
    }
}