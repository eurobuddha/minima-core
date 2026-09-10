#pragma once
#include "support/ByteIO.hpp"
#include <zlib.h>
#include <array>
#include <limits>
namespace minima {
inline std::vector<uint8_t> compressGzip(std::span<const uint8_t> data) {
    if(data.size()>std::numeric_limits<uInt>::max()) throw std::length_error("Gzip input too large");
    z_stream stream{};if(deflateInit2(&stream,Z_DEFAULT_COMPRESSION,Z_DEFLATED,31,8,Z_DEFAULT_STRATEGY)!=Z_OK) throw std::runtime_error("Gzip init");
    struct End {z_stream* s;~End(){deflateEnd(s);}} end{&stream};gz_header header{};header.os=255;
    if(deflateSetHeader(&stream,&header)!=Z_OK) throw std::runtime_error("Gzip header");stream.next_in=const_cast<Bytef*>(data.data());stream.avail_in=uInt(data.size());std::array<uint8_t,16384> buffer;std::vector<uint8_t> out;
    int status;do {stream.next_out=buffer.data();stream.avail_out=uInt(buffer.size());status=deflate(&stream,Z_FINISH);if(status!=Z_OK && status!=Z_STREAM_END) throw std::runtime_error("Gzip deflate");wire::append(out,std::span(buffer).first(buffer.size()-stream.avail_out));}while(status!=Z_STREAM_END);return out;
}
inline std::vector<uint8_t> decompressGzip(std::span<const uint8_t> data,bool miniDataFrameOnly=false) {
    if(data.size()>std::numeric_limits<uInt>::max()) throw std::length_error("Gzip input too large");
    z_stream stream{};if(inflateInit2(&stream,31)!=Z_OK) throw std::runtime_error("Gzip init");struct End {z_stream* s;~End(){inflateEnd(s);}} end{&stream};
    stream.next_in=const_cast<Bytef*>(data.data());stream.avail_in=uInt(data.size());std::array<uint8_t,16384> buffer;std::vector<uint8_t> out;
    for(;;) {stream.next_out=buffer.data();stream.avail_out=uInt(buffer.size());int status=inflate(&stream,Z_NO_FLUSH);auto added=buffer.size()-stream.avail_out;wire::append(out,std::span(buffer).first(added));
        if(miniDataFrameOnly && out.size()>=4) {uint32_t length=(uint32_t(out[0])<<24)|(uint32_t(out[1])<<16)|(uint32_t(out[2])<<8)|out[3];if(length>uint32_t(INT32_MAX)) throw std::out_of_range("Negative MiniData length");if(out.size()>=size_t(length)+4) return out;}
        if(status==Z_STREAM_END) {if(stream.avail_in>=2 && stream.next_in[0]==31 && stream.next_in[1]==139) {if(inflateReset(&stream)!=Z_OK) throw std::runtime_error("Gzip reset");continue;}return out;}
        if(status!=Z_OK || (!added && !stream.avail_in)) throw std::runtime_error("Invalid or truncated gzip");
    }
}
}
