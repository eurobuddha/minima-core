#pragma once
#include "org/minima/objects/IBD.hpp"
#include "support/GzipInput.hpp"
namespace minima {
class RawArchiveInput {
    std::filesystem::path mFile;
    std::unique_ptr<GzipInput> mInput;
    std::vector<uint8_t> mBuffered;
    int32_t mTotalFound=0,mTotalAdded=0;
    std::shared_ptr<Cascade> mCascade;
    bool mClosed=false;
    template<class T> T readObject() {
        if(!mInput)throw std::invalid_argument("Raw archive is not connected");
        if(mClosed)throw wire::IOException("Stream closed");
        for(;;){size_t offset=0;
            try{
                auto result=T::deserialise(mBuffered,offset);
                mBuffered.erase(mBuffered.begin(),mBuffered.begin()+offset);return result;
            }catch(const wire::EOFException&){
                std::array<uint8_t,65536> more;
                try{auto count=mInput->read(more);if(!count)throw wire::EOFException("Truncated archive object");wire::append(mBuffered,std::span(more).first(count));}
                catch(...){mBuffered.clear();throw;}
            }catch(...){mBuffered.erase(mBuffered.begin(),mBuffered.begin()+offset);throw;}
        }
    }
public:
    explicit RawArchiveInput(std::filesystem::path file):mFile(std::move(file)){}
    void connect();
    void stop();
    std::shared_ptr<Cascade> getCascade() const {return mCascade;}
    IBD getNextIBD();
};
}
