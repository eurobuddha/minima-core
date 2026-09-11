#include "ConsoleInput.hpp"
#include "SocketCharacterReader.hpp"
#include <cerrno>
#include <cstring>
#include <unistd.h>
namespace minima {
namespace {
class StandardInput final:public ConsoleInput {
    bool mClosed=false;
    SocketCharacterReader mReader;
    int32_t readBytes(std::span<uint8_t> bytes) {
        if(mClosed)throw wire::IOException("Stream closed");
        ssize_t count;
        do {count=::read(STDIN_FILENO,bytes.data(),bytes.size());}while(count<0&&errno==EINTR);
        if(count<0)throw wire::IOException(std::strerror(errno));
        return count==0?-1:static_cast<int32_t>(count);
    }
    void close() {
        if(mClosed)return;
        mClosed=true;
        if(::close(STDIN_FILENO)<0)throw wire::IOException(std::strerror(errno));
    }
public:
    StandardInput():mReader([this](std::span<uint8_t> bytes){return readBytes(bytes);}){}
    ConsoleInput::String readLine() override{return mReader.readLineUTF16(true);}
    void closeBufferedReader() override{close();}
    void closeInputStreamReader() override{close();}
};
}
std::shared_ptr<ConsoleInput> openStandardConsoleInput(){return std::make_shared<StandardInput>();}
}
