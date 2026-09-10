#include "NIOClient.hpp"
#include "support/JavaDateString.hpp"
#include "support/JavaExceptions.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include <iostream>
#include <algorithm>
namespace minima {
std::atomic<bool> NIOClient::mTraceON{false};
std::function<std::shared_ptr<NIORuntime>()> NIOClient::runtimeLookup;
std::shared_ptr<NIORuntime> NIOClient::requireRuntime(){return runtimeLookup();}
NIOClient::NIOClient(std::optional<std::string> host,int32_t port){mUID=MiniFormat::createRandomString(8);mHost=std::move(host);mPort=port;mIncoming=false;mRuntime=requireRuntime();}
NIOClient::NIOClient(bool incoming,std::optional<std::string> host,int32_t port,std::shared_ptr<NIOSocket> socket,std::shared_ptr<NIOSelectionKey> key){mUID=MiniFormat::createRandomString(8);mHost=std::move(host);mPort=port;mSocket=std::move(socket);mKey=std::move(key);mIncoming=incoming;mBufferIn=std::make_unique<Buffer>();mBufferOut=std::make_unique<Buffer>();mReadByteArrayTemp.resize(MAX_NIO_BUFFERS);mMessages=std::make_unique<std::deque<std::shared_ptr<MiniData>>>();mRuntime=requireRuntime();mTimeConnected=mRuntime?mRuntime->currentTimeMillis():TimerMessage::currentTimeMillis();mLastMessageRead=mTimeConnected;}
json::JSONObject NIOClient::toJSON() const {std::lock_guard lock(mMetadataLock);json::JSONObject out;out.put("welcome",nullable(mWelcomeMessage));out.put("uid",nullable(mUID));out.put("incoming",mIncoming);out.put("host",nullable(mHost));out.put("port",mPort);out.put("minimaport",mMinimaPort);out.put("connected",javaDateString(mTimeConnected));out.put("valid",mValidGreeting);out.put("sentgreeting",mSentGreeting);out.put("maxima",nullable(mMaximaIdent));out.put("maximamls",nullable(mMaximaMLS));return out;}
void NIOClient::sendData(std::shared_ptr<MiniData> data){if(!mMessages)throw std::runtime_error("Null message list");std::lock_guard lock(mMessagesLock);if(!mKey)throw std::runtime_error("Null selection key");if(mKey->isValid()){mMessages->push_back(std::move(data));mKey->interestOps(NIOSelectionKey::OP_READ|NIOSelectionKey::OP_WRITE);mKey->wakeup();}}
void NIOClient::handleRead(){
    if(!mSocket||!mBufferIn)throw std::runtime_error("Null input channel/buffer");auto& buffer=*mBufferIn;
    int32_t read=mSocket->read(std::span(buffer.bytes).subspan(size_t(buffer.position),size_t(buffer.remaining())));
    if(read==-1)throw wire::IOException("Socket Closed!");if(read<0||read>buffer.remaining())throw wire::IOException("Invalid channel read count");buffer.position+=read;
    if(mTraceON)std::clog<<"[NIOCLIENT] "<<getUID().value_or("null")<<" read "<<read<<'\n';
    if(!mRuntime||!mRuntime->traffic)throw std::runtime_error("Null NIO traffic listener");mRuntime->traffic->addToTotalRead(read);if(read==0)return;buffer.flip();
    while(buffer.remaining()>0){
        if(!mReadByteArray){if(buffer.remaining()<4)break;uint32_t limit=0;for(int i=0;i<4;++i)limit=(limit<<8)|buffer.bytes[size_t(buffer.position++)];mReadCurrentLimit=std::bit_cast<int32_t>(limit);mReadCurrentPosition=0;if(mReadCurrentLimit>MAX_MESSAGE)throw wire::IOException("Message too big for read! "+std::to_string(mReadCurrentLimit));mReadByteArray=std::make_unique<FastByteArrayStream>(mReadCurrentLimit);}
        int32_t count=std::min(buffer.remaining(),mReadCurrentLimit-mReadCurrentPosition);if(count<0)throw std::out_of_range("Negative buffer read length");
        std::copy_n(buffer.bytes.begin()+buffer.position,count,mReadByteArrayTemp.begin());buffer.position+=count;mReadByteArray->writeData(mReadByteArrayTemp,0,count);mReadCurrentPosition+=count;
        if(mReadCurrentPosition==mReadCurrentLimit){auto data=std::make_shared<MiniData>(mReadByteArray->toByteArray());auto message=std::make_shared<Message>("NIO_NEWMSG");message->addString("uid",getUID()).addString("fullhost",getFullAddress()).addObject("data",MessageObject(data,std::function<std::string(const MiniData&)>([](const MiniData& value){return value.to0xString();})));mRuntime->postMessage(message);mReadByteArray.reset();{std::lock_guard lock(mMetadataLock);mLastMessageRead=mRuntime->currentTimeMillis();}}
    }
    buffer.compact();
}
void NIOClient::handleWrite(){
    if(!mBufferOut)throw std::runtime_error("Null output buffer");auto& buffer=*mBufferOut;
    while(buffer.remaining()>0){
        if(!mWriteData){if(!mMessages)throw std::runtime_error("Null message list");bool next;{std::lock_guard lock(mMessagesLock);next=!mMessages->empty();}if(!next)break;{std::lock_guard lock(mMessagesLock);if(!mMessages->empty()){mWriteData=mMessages->front();mMessages->pop_front();}}if(!mWriteData)throw std::runtime_error("Null outgoing data");if(mWriteData->size()>size_t(MAX_MESSAGE)){std::clog<<"ERROR : Trying to write a message that is too big! "<<mWriteData->size()<<'\n';mWriteData.reset();break;}mWritePosition=0;mWriteLimit=int32_t(mWriteData->size());mWriteStart=false;}
        if(!mWriteStart){if(buffer.remaining()<4)break;auto limit=uint32_t(mWriteLimit);for(int shift:{24,16,8,0})buffer.bytes[size_t(buffer.position++)]=uint8_t(limit>>shift);mWriteStart=true;}
        int32_t count=std::min(buffer.remaining(),mWriteLimit-mWritePosition);std::copy_n(mWriteData->bytes().begin()+mWritePosition,count,buffer.bytes.begin()+buffer.position);buffer.position+=count;mWritePosition+=count;if(mWritePosition==mWriteLimit)mWriteData.reset();
    }
    buffer.flip();if(!mSocket)throw std::runtime_error("Null output socket");auto write=mSocket->write(std::span<const uint8_t>(buffer.bytes).subspan(size_t(buffer.position),size_t(buffer.remaining())));if(write<0||write>buffer.remaining())throw wire::IOException("Invalid channel write count");buffer.position+=write;
    if(mTraceON)std::clog<<"[NIOCLIENT] "<<getUID().value_or("null")<<" wrote : "<<write<<'\n';if(!mRuntime||!mRuntime->traffic)throw std::runtime_error("Null NIO traffic listener");mRuntime->traffic->addToTotalWrite(write);
    if(!mMessages)throw std::runtime_error("Null message list");{std::lock_guard lock(mMessagesLock);if(buffer.remaining()==0&&mMessages->empty()&&!mWriteData){if(!mKey)throw std::runtime_error("Null selection key");if(mKey->isValid())mKey->interestOps(NIOSelectionKey::OP_READ);}}
    buffer.compact();
}
void NIOClient::disconnect(){try{if(!mKey)throw std::runtime_error("Null key");mKey->cancel();if(!mSocket)throw std::runtime_error("Null socket");mSocket->close();}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&) {}}
}
