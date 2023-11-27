#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

struct message_buffer {
    long message_type;
    char message_text[256];
};

// Simple wrapper for message queue functionality so that we can send messages
// and then use receive method from the main thread to wait for incoming messages
// from other processes
class MessageQueue {
private:
    int mq_id;
    const int MAX_MESSAGE_SIZE = 256;
    const long MESSAGE_TYPE = 1;

public:
    const std::string END_OF_MESSAGES_MARKER = "END_OF_MESSAGES";

    explicit MessageQueue() {
        key_t key = ftok("myfind", 65);
        mq_id = msgget(key, 0666 | IPC_CREAT);
        if (mq_id == -1) throw std::runtime_error("Error creating a message queue...");
    }

    void send_message(const std::string &message) const {
        message_buffer buffer{};
        buffer.message_type = MESSAGE_TYPE;
        strncpy(buffer.message_text, message.c_str(), MAX_MESSAGE_SIZE);
        msgsnd(mq_id, &buffer, sizeof(buffer.message_text), 0);
    }

    void receive_messages(std::vector<std::string> &messages) {
        message_buffer buffer{};

        while (true) {
            msgrcv(mq_id, &buffer, sizeof(buffer.message_text), MESSAGE_TYPE, 0);
            std::string message(buffer.message_text);
            if (message == END_OF_MESSAGES_MARKER) break;
            messages.emplace_back(std::move(message));
        }

        msgctl(mq_id, IPC_RMID, nullptr);
    }
};
