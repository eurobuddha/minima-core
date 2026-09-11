#pragma once
#include "ByteIO.hpp"
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
namespace minima {
// FileOutputStream plus ordinary-thread BufferedOutputStream (8192 bytes).
// POSIX opening/writing follows MiniFile and nodecount's existing writer.
class BufferedFileOutput {
    int mDescriptor=-1;
    wire::Bytes mPending;
    void direct(std::span<const uint8_t> data){
        while(!data.empty()){
            auto count=::write(mDescriptor,data.data(),data.size());
            if(count<0&&errno==EINTR)continue;
            if(count<=0)throw wire::IOException(count<0?std::strerror(errno):"Write failed");
            data=data.subspan(size_t(count));
        }
    }
public:
    explicit BufferedFileOutput(const std::filesystem::path& file){
        auto path=file.string();if(path.find('\0')!=path.npos)throw wire::TypedIOException("java.io.FileNotFoundException","Invalid file path");
        do{mDescriptor=::open(path.c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666);}while(mDescriptor<0&&errno==EINTR);
        if(mDescriptor<0)throw wire::TypedIOException("java.io.FileNotFoundException",path+" ("+std::strerror(errno)+")");
    }
    BufferedFileOutput(const BufferedFileOutput&)=delete;
    BufferedFileOutput& operator=(const BufferedFileOutput&)=delete;
    ~BufferedFileOutput(){if(mDescriptor>=0)::close(mDescriptor);} // No implicit flush on an exceptional path.
    void write(std::span<const uint8_t> data){
        if(data.size()>=8192){flush();direct(data);return;}
        if(data.size()>8192-mPending.size())flush();wire::append(mPending,data);
    }
    void flush(){if(!mPending.empty()){direct(mPending);mPending.clear();}}
    void close(){if(mDescriptor<0)return;flush();int descriptor=mDescriptor;mDescriptor=-1;if(::close(descriptor)<0)throw wire::IOException(std::strerror(errno));}
};
}
