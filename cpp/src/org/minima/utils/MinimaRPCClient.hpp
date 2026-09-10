#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace minima {
class MinimaRPCClient {
public:
    using String=std::optional<std::u16string>;
    struct Input {
        virtual ~Input()=default;
        virtual String readLine()=0;
        virtual void closeBufferedReader()=0;
        virtual void closeInputStreamReader()=0;
    };
    struct Runtime {
        std::function<std::shared_ptr<Input>()> openInput;
        std::function<void(const std::string&)> println;
        std::function<void(const std::exception&)> printStackTrace;
    };
    // Exit status represents source System.exit; uncaught source exceptions
    // propagate to the native executable's uncaught-exception boundary.
    static int main(const std::vector<String>* args,Runtime runtime={});
    static int main(const std::vector<String>& args,Runtime runtime={}) {return main(&args,std::move(runtime));}
};
}
