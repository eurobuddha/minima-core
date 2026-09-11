#pragma once
#include <memory>
#include <optional>
#include <string>
namespace minima {
class ConsoleInput {
public:
    using String=std::optional<std::u16string>;
    virtual ~ConsoleInput()=default;
    virtual String readLine()=0;
    virtual void closeBufferedReader()=0;
    virtual void closeInputStreamReader()=0;
};
std::shared_ptr<ConsoleInput> openStandardConsoleInput();
}
