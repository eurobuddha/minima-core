#pragma once
#include "ByteIO.hpp"
#include <zlib.h>
#include <array>
#include <functional>
#include <limits>
namespace minima {
// Gzip.hpp's zlib configuration, streamed with Java GZIPOutputStream framing.
class GzipOutput {
    z_stream mStream{};
    std::function<void(std::span<const uint8_t>)> mWrite;
    std::array<uint8_t,512> mBuffer{};
    uint32_t mCRC=0;
    bool mFinished=false;
    size_t pump(int flush){
        mStream.next_out=mBuffer.data();mStream.avail_out=mBuffer.size();
        auto status=deflate(&mStream,flush);
        if(status!=Z_OK&&status!=Z_STREAM_END)throw wire::IOException("Gzip deflate");
        mFinished=status==Z_STREAM_END;return mBuffer.size()-mStream.avail_out;
    }
    void trailer(std::span<uint8_t> output){
        uint32_t size=uint32_t(mStream.total_in);
        for(int i=0;i<4;++i){output[i]=uint8_t(mCRC>>(8*i));output[i+4]=uint8_t(size>>(8*i));}
    }
public:
    explicit GzipOutput(std::function<void(std::span<const uint8_t>)> write):mWrite(std::move(write)){
        if(deflateInit2(&mStream,Z_DEFAULT_COMPRESSION,Z_DEFLATED,-15,8,Z_DEFAULT_STRATEGY)!=Z_OK)throw wire::IOException("Gzip init");
        constexpr std::array<uint8_t,10> header{31,139,8,0,0,0,0,0,0,255};
        try{mWrite(header);}catch(...){deflateEnd(&mStream);throw;}
    }
    GzipOutput(const GzipOutput&)=delete;
    GzipOutput& operator=(const GzipOutput&)=delete;
    ~GzipOutput(){deflateEnd(&mStream);}
    void write(std::span<const uint8_t> data){
        if(mFinished)throw wire::IOException("write beyond end of stream");
        while(!data.empty()){
            auto part=data.first(std::min(data.size(),size_t(std::numeric_limits<uInt>::max())));
            mStream.next_in=const_cast<Bytef*>(part.data());mStream.avail_in=uInt(part.size());
            while(mStream.avail_in){auto count=pump(Z_NO_FLUSH);if(count)mWrite(std::span(mBuffer).first(count));}
            mCRC=uint32_t(crc32(mCRC,part.data(),uInt(part.size())));data=data.subspan(part.size());
        }
    }
    void finish(){
        if(mFinished)return;
        while(!mFinished){auto count=pump(Z_FINISH);
            if(mFinished&&count<=mBuffer.size()-8){trailer(std::span(mBuffer).subspan(count,8));mWrite(std::span(mBuffer).first(count+8));return;}
            if(count)mWrite(std::span(mBuffer).first(count));
        }
        std::array<uint8_t,8> tail;trailer(tail);mWrite(tail);
    }
};
}
