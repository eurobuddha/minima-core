#include "Stack.hpp"
namespace minima {
void Stack::push(MessageObject object) {mStack.push_back(std::move(object));}
MessageObject Stack::pop() {if(isEmpty()) return {};auto object=std::move(mStack.back());mStack.pop_back();return object;}
MessageObject Stack::peek() const {return isEmpty()?MessageObject{}:mStack.back();}
bool Stack::isEmpty() const {return mStack.empty();}
}
