#pragma once
#include <string>
#include "netcommon.h"
enum class MessageType {
    MessageAll,
    PrivateMessage,
};
struct message {
    MessageType type;
    std::string msg;
};