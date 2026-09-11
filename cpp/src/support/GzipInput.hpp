#pragma once
#include "ByteIO.hpp"
#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <cerrno>
#include <cstring>
#include <zlib.h>
namespace minima {
// Streaming counterpart to Gzip.hpp. Raw inflate keeps trailer validation on
// the next read, as in GZIPInputStream, rather than rejecting returned payload.
class GzipInput {
    std::ifstream mFile;
    std::function<size_t(std::span<uint8_t>)> mReader;
    std::function<void()> mClose;
    size_t mReadSize=65536;
    std::array<uint8_t,65536> mInput{};
    size_t mBegin=0,mEnd=0;
    z_stream mStream{};
    bool mInitialized=false,mClosed=false,mEndMember=false,mEOF=false;
    uint32_t mCRC=0,mSize=0;
    bool fill(size_t requested=65536) {
        if(mBegin<mEnd)return true;
        if(mReader){
            auto capacity=std::min(requested,mReadSize);mBegin=0;mEnd=mReader(std::span(mInput).first(capacity));
            if(mEnd>capacity)throw wire::IOException("Invalid gzip reader byte count");return mEnd!=0;
        }
        mFile.read(reinterpret_cast<char*>(mInput.data()),mInput.size());
        mBegin=0;mEnd=size_t(mFile.gcount());
        if(mFile.bad())throw wire::IOException("Gzip input read failed");return mEnd!=0;
    }
    uint8_t byte() {if(!fill(mReader?1:65536))throw wire::EOFException("Truncated gzip");return mInput[mBegin++];}
    uint32_t little32() {uint32_t value=0;for(unsigned shift=0;shift<32;shift+=8)value|=uint32_t(byte())<<shift;return value;}
    void header() {
        uint32_t crc=0;
        auto next=[&]{auto b=byte();crc=uint32_t(crc32(crc,&b,1));return b;};
        auto first=next(),second=next();if(first!=31||second!=139)throw wire::IOException("Not in GZIP format");
        if(next()!=8)throw wire::IOException("Unsupported gzip method");auto flags=next();
        for(int i=0;i<6;++i)next();
        if(flags&4){unsigned n=next();n|=unsigned(next())<<8;for(unsigned i=0;i<n;++i)next();}
        if(flags&8)while(next()!=0){}
        if(flags&16)while(next()!=0){}
        if(flags&2){unsigned expected=byte();expected|=unsigned(byte())<<8;if(expected!=(crc&65535))throw wire::IOException("Corrupt gzip header");}
        mCRC=0;mSize=0;
    }
public:
    GzipInput(std::function<size_t(std::span<uint8_t>)> reader,std::function<void()> close):mReader(std::move(reader)),mClose(std::move(close)),mReadSize(512){
        header();if(inflateInit2(&mStream,-MAX_WBITS)!=Z_OK)throw wire::IOException("Gzip inflate initialization failed");mInitialized=true;
    }
    explicit GzipInput(const std::filesystem::path& file) {
        auto text=file.string();if(text.find('\0')!=std::string::npos)throw wire::TypedIOException("java.io.FileNotFoundException","Invalid file path");
        auto fail=[&](int error){throw wire::TypedIOException("java.io.FileNotFoundException",text+" ("+std::strerror(error)+")");};
        std::error_code error;if(std::filesystem::is_directory(file,error))fail(EISDIR);
        errno=0;mFile.open(file,std::ios::binary);if(!mFile)fail(errno?errno:ENOENT);header();
        if(inflateInit2(&mStream,-MAX_WBITS)!=Z_OK)throw wire::IOException("Gzip inflate initialization failed");mInitialized=true;
    }
    GzipInput(const GzipInput&)=delete;
    GzipInput& operator=(const GzipInput&)=delete;
    ~GzipInput(){if(mInitialized)inflateEnd(&mStream);}
    void close(){if(mClosed)return;mClosed=true;if(mReader){if(mClose)mClose();return;}mFile.close();if(mFile.bad())throw wire::IOException("Gzip input close failed");}
    size_t read(std::span<uint8_t> output) {
        if(mClosed)throw wire::IOException("Stream closed");if(output.empty()||mEOF)return 0;
        for(;;){
            if(mEndMember){
                if(little32()!=mCRC||little32()!=mSize)throw wire::IOException("Corrupt gzip trailer");
                try{header();}catch(const wire::IOException&){mEOF=true;return 0;}
                if(inflateReset(&mStream)!=Z_OK)throw wire::IOException("Gzip reset failed");mEndMember=false;
            }
            if(!fill())throw wire::EOFException("Unexpected end of ZLIB input stream",true);
            mStream.next_in=mInput.data()+mBegin;mStream.avail_in=uInt(mEnd-mBegin);
            auto capacity=std::min(output.size(),size_t(UINT_MAX));mStream.next_out=output.data();mStream.avail_out=uInt(capacity);
            int status=inflate(&mStream,Z_NO_FLUSH);mBegin=mEnd-mStream.avail_in;auto produced=capacity-mStream.avail_out;
            if(status!=Z_OK&&status!=Z_STREAM_END&&status!=Z_BUF_ERROR)throw wire::IOException("Invalid deflate data");
            mEndMember=status==Z_STREAM_END;
            if(produced){mCRC=uint32_t(crc32(mCRC,output.data(),uInt(produced)));mSize+=uint32_t(produced);return produced;}
            if(status==Z_BUF_ERROR&&mBegin<mEnd)throw wire::IOException("Gzip decoder made no progress");
        }
    }
};
}
