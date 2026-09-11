#pragma once
#include "org/minima/utils/encrypt/GenerateKey.hpp"
#include "ByteIO.hpp"
#include <algorithm>
namespace minima {
// CipherInputStream buffering around the existing SunJCE-compatible Cipher.
class CipherInput {
    Cipher mCipher;
    std::span<const uint8_t> mInput;
    wire::Bytes mOutput;
    size_t mPosition=0;
    bool mDone=false,mClosed=false;
public:
    CipherInput(Cipher cipher,std::span<const uint8_t> input):mCipher(std::move(cipher)),mInput(input){}
    size_t read(std::span<uint8_t> output){
        if(mClosed)throw wire::IOException("Stream closed");
        while(mPosition==mOutput.size()){
            if(mDone)return 0;mPosition=0;mOutput.clear();
            if(!mInput.empty()){
                auto count=std::min(size_t(8192),mInput.size());mOutput=mCipher.update(mInput.first(count));mInput=mInput.subspan(count);
            }else{
                mDone=true;
                try{mOutput=mCipher.doFinal({});}
                catch(const std::bad_alloc&){throw;}
                catch(const std::exception& error){throw wire::IOException(error.what());}
            }
        }
        auto count=std::min(output.size(),mOutput.size()-mPosition);
        std::copy_n(mOutput.data()+mPosition,count,output.data());mPosition+=count;return count;
    }
    void close(){
        if(mClosed)return;mClosed=true;
        if(!mDone)try{mCipher.doFinal({});}catch(const std::bad_alloc&){throw;}catch(const std::exception&){}
        mOutput.clear();
    }
};
}
