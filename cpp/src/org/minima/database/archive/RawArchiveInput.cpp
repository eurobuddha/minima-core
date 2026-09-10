#include "RawArchiveInput.hpp"
#include <iostream>
namespace minima {
void RawArchiveInput::connect() {
    mInput=std::make_unique<GzipInput>(mFile);mBuffered.clear();mClosed=false;
    if(readObject<MiniByte>().isTrue()){
        std::clog<<"Cascade found in RAW archive..\n";
        mCascade=std::make_shared<Cascade>(readObject<Cascade>());
    }
    mTotalFound=readObject<MiniNumber>().getAsInt();mTotalAdded=0;
    std::clog<<"Blocks found in RAW Archive : "<<mTotalFound<<'\n';
}
void RawArchiveInput::stop() {
    if(!mInput)throw std::invalid_argument("Raw archive is not connected");mClosed=true;mBuffered.clear();mInput->close();
}
IBD RawArchiveInput::getNextIBD() {
    IBD result;
    if(mTotalAdded<mTotalFound){
        if(mTotalAdded==0&&mCascade)result.setCascade(mCascade);
        for(int i=0;i<256;++i){
            result.getTxBlocks()->push_back(std::make_shared<TxBlock>(readObject<TxBlock>()));
            ++mTotalAdded;if(mTotalAdded>=mTotalFound)break;
        }
    }
    return result;
}
}
