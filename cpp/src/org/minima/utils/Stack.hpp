#pragma once
#include "org/minima/utils/messages/Message.hpp"
namespace minima {
class Stack {
    std::vector<MessageObject> mStack;
public:
    void push(MessageObject object);
    MessageObject pop();
    MessageObject peek() const;
    bool isEmpty() const;
};
}
