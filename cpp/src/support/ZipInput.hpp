/*
 * Copyright (c) 1996, 2024, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#pragma once
// Streaming ZIP dependency for ZipExtractor, adapted from OpenJDK 26
// ZipInputStream/InflaterInputStream and the existing native GzipInput pattern.
// GPL version 2 with the Classpath exception; see OPENJDK_TIMSORT_LICENSE.
#include "ByteIO.hpp"
#include "JavaExceptions.hpp"
#include "JavaUnicode.hpp"
#include "UTF8Spans.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <functional>
#include <optional>
#include <sstream>
#include <zlib.h>

namespace minima {
class ZipInput {
public:
    // Same native stream boundary as GzipInput: zero bytes denotes EOF.
    using Reader = std::function<size_t(std::span<uint8_t>)>;
    struct Entry {
        std::string name;
        uint16_t method = 0;
        uint64_t crc = uint64_t(-1), size = uint64_t(-1), compressed = uint64_t(-1);
        bool isDirectory() const { return name.ends_with('/'); }
    };
private:
    Reader mReader;
    std::function<void()> mClose;
    std::vector<uint8_t> mPushback;
    std::array<uint8_t,512> mInput{};
    z_stream mInflater{};
    std::optional<Entry> mEntry;
    bool mInitialized = false, mClosed = false, mFinished = false, mPending = false, mDescriptor64 = false;
    uint16_t mFlags = 0;
    uint64_t mRemaining = 0, mConsumed = 0, mProduced = 0;
    uint32_t mCRC = 0;
    [[noreturn]] static void zipError(const std::string& message) { throw wire::TypedIOException("java.util.zip.ZipException", message); }
    static uint64_t little(std::span<const uint8_t> bytes, size_t offset, size_t count) {
        uint64_t value = 0;
        for (size_t i = 0; i < count; ++i) value |= uint64_t(bytes[offset+i]) << (8*i);
        return value;
    }
    static std::string signedText(uint64_t value) { return std::to_string(std::bit_cast<int64_t>(value)); }
    static std::string hexText(uint64_t value) { std::ostringstream out; out << std::hex << value; return out.str(); }
    void ensureOpen() const { if (mClosed) throw wire::IOException("Stream closed"); }
    size_t input(std::span<uint8_t> out) {
        auto count = std::min(out.size(), mPushback.size());
        std::copy_n(mPushback.begin(), count, out.begin());
        mPushback.erase(mPushback.begin(), mPushback.begin()+count);
        if (count < out.size()) {
            auto read = mReader(out.subspan(count));
            if (read > out.size()-count) throw wire::IOException("Invalid ZIP reader byte count");
            count += read;
        }
        return count;
    }
    void unread(std::span<const uint8_t> bytes) {
        if (mPushback.size()+bytes.size()>512) throw wire::IOException("Push back buffer is full");
        mPushback.insert(mPushback.begin(), bytes.begin(), bytes.end());
    }
    std::vector<uint8_t> readFully(size_t count) {
        std::vector<uint8_t> bytes(count);size_t offset=0;
        while (offset<count) {
            auto read=input(std::span(bytes).subspan(offset));
            if (!read) throw wire::EOFException("");
            offset+=read;
        }
        return bytes;
    }
    void checkCRC() {
        if (mEntry->crc != mCRC) zipError("invalid entry CRC (expected 0x"+hexText(mEntry->crc)+" but got 0x"+hexText(mCRC)+")");
    }
    void readEnd() {
        if (mInflater.avail_in) unread({mInflater.next_in,mInflater.avail_in});
        mInflater.avail_in=0;
        if (mFlags&8) {
            bool wide=mDescriptor64 || mConsumed>UINT32_MAX || mProduced>UINT32_MAX;
            auto bytes=readFully(wide?24:16);size_t offset=4;
            if (little(bytes,0,4)!=0x08074b50) { offset=0;unread(std::span(bytes).last(4)); }
            mEntry->crc=little(bytes,offset,4);
            mEntry->compressed=little(bytes,offset+4,wide?8:4);
            mEntry->size=little(bytes,offset+(wide?12:8),wide?8:4);
        }
        if (mEntry->size!=mProduced) zipError("invalid entry size (expected "+signedText(mEntry->size)+" but got "+std::to_string(mProduced)+" bytes)");
        if (mEntry->compressed!=mConsumed) zipError("invalid entry compressed size (expected "+signedText(mEntry->compressed)+" but got "+std::to_string(mConsumed)+" bytes)");
        checkCRC();
    }
public:
    ZipInput(Reader reader,std::function<void()> close):mReader(std::move(reader)),mClose(std::move(close)) {
        if (!mReader) throw JavaNullPointerException("in is null");
        if (inflateInit2(&mInflater,-MAX_WBITS)!=Z_OK) throw wire::IOException("ZIP inflate initialization failed");
        mInitialized=true;
    }
    ZipInput(const ZipInput&)=delete;
    ZipInput& operator=(const ZipInput&)=delete;
    ~ZipInput() { if (mInitialized) inflateEnd(&mInflater); }
    std::optional<Entry> getNextEntry() {
        ensureOpen();if (mEntry) closeEntry();
        mCRC=0;mConsumed=mProduced=0;mFinished=mPending=false;
        if (inflateReset(&mInflater)!=Z_OK) throw wire::IOException("ZIP inflate reset failed");
        mInflater.avail_in=0;
        std::vector<uint8_t> header;
        try { header=readFully(30); } catch (const wire::EOFException&) { return std::nullopt; }
        if (little(header,0,4)!=0x04034b50) return std::nullopt;
        mFlags=uint16_t(little(header,6,2));
        auto name=readFully(size_t(little(header,26,2)));
        unicode::forEachUTF8Span(name,[](auto,bool valid){if(!valid)zipError("invalid LOC header (bad entry name)");});
        Entry entry;entry.name=std::string(name.begin(),name.end());
        auto units=unicode::decodeUTF8(entry.name).size();
        if (46+units>65535) throw JavaIllegalArgumentException("entry name too long");
        if (mFlags&1) zipError("encrypted ZIP entry not supported");
        entry.method=uint16_t(little(header,8,2));mDescriptor64=false;
        auto compressed=little(header,18,4),size=little(header,22,4);
        if (mFlags&8) {
            if (entry.method!=8) zipError("only DEFLATED entries can have EXT descriptor");
        } else { entry.crc=little(header,14,4);entry.compressed=compressed;entry.size=size; }
        auto extra=readFully(size_t(little(header,28,2)));
        if (!extra.empty() && 46+units+extra.size()>65535) throw JavaIllegalArgumentException("invalid extra field length");
        bool use64=entry.compressed==UINT32_MAX || entry.size==UINT32_MAX;
        // Match ZipEntry's tolerant traversal, including the strict '<' bound.
        for (size_t offset=0;offset+4<extra.size();) {
            auto tag=little(extra,offset,2),length=little(extra,offset+2,2);offset+=4;
            if (length>extra.size()-offset) break;
            if (tag==1) {
                if (use64 && length>=16) { entry.size=little(extra,offset,8);entry.compressed=little(extra,offset+8,8); }
                if ((mFlags&8) && (compressed==UINT32_MAX || size==UINT32_MAX)) mDescriptor64=true;
            }
            offset+=size_t(length);
        }
        mEntry=entry;mRemaining=entry.size;return entry;
    }
    size_t read(std::span<uint8_t> output) {
        ensureOpen();if(output.empty() || !mEntry)return 0;
        if(mEntry->method==0) {
            if(std::bit_cast<int64_t>(mRemaining)<=0){mEntry.reset();return 0;}
            auto count=input(output.first(size_t(std::min(uint64_t(output.size()),mRemaining))));
            if(!count)zipError("unexpected EOF");
            mCRC=uint32_t(crc32(mCRC,output.data(),uInt(count)));mRemaining-=count;
            if(!mRemaining)checkCRC(); // A failing final STORED chunk is not returned.
            return count;
        }
        if(mEntry->method!=8)zipError("invalid compression method");
        for(;;) {
            if(mFinished) { readEnd();mEntry.reset();return 0; }
            if(!mInflater.avail_in && !mPending) {
                auto count=input(mInput);
                if(!count)throw wire::EOFException("Unexpected end of ZLIB input stream");
                mInflater.next_in=mInput.data();mInflater.avail_in=uInt(count);
            }
            auto capacity=std::min(output.size(),size_t(UINT_MAX));
            mInflater.next_out=output.data();mInflater.avail_out=uInt(capacity);
            auto before=mInflater.avail_in;
            int status=inflate(&mInflater,Z_NO_FLUSH);
            mConsumed+=before-mInflater.avail_in;
            auto count=capacity-mInflater.avail_out;mProduced+=count;
            mPending=mInflater.avail_out==0;
            if(status!=Z_OK && status!=Z_STREAM_END && status!=Z_BUF_ERROR)zipError(mInflater.msg?mInflater.msg:"Invalid ZLIB data format");
            mFinished=status==Z_STREAM_END;
            if(count) { mCRC=uint32_t(crc32(mCRC,output.data(),uInt(count)));return count; }
            if(status==Z_BUF_ERROR && mInflater.avail_in)zipError("Invalid ZLIB data format");
        }
    }
    void closeEntry() { ensureOpen();std::array<uint8_t,512> discard;while(read(discard)){} }
    void close() {
        if(mClosed)return;
        if(mInitialized){inflateEnd(&mInflater);mInitialized=false;}
        if(mClose)mClose();mClosed=true;
    }
};
}
