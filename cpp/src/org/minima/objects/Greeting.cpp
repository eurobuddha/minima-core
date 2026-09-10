#include "Greeting.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "support/ByteIO.hpp"
#include <iostream>
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include "org/minima/system/network/p2p/messages/InetSocketAddressIO.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima {
GreetingServices Greeting::services;
Greeting::Greeting():mVersion(GlobalParams::MINIMA_VERSION){}
Greeting& Greeting::createGreeting(){
    services.readLock(true);
    try{
        auto welcome=services.getWelcome();
        getExtraData().put("welcome",welcome?json::JSONValue(*welcome):json::JSONValue{});
        if(GeneralParams::IS_HOST_SET)getExtraData().put("host",GeneralParams::MINIMA_HOST);
        getExtraData().put("port",std::to_string(GeneralParams::MINIMA_PORT));
        auto tip=services.getTip();
        setTopBlock(tip?tip->getTxPoW().getBlockNumber():MiniNumber(-1));
        while(tip){mChain.push_back(tip->getTxPoW().getTxPoWIDData());tip=tip->getParent();}
        if(GeneralParams::P2P_ENABLED){auto peers=p2p::InetSocketAddressIO::addressesListToJSONArray(services.getPeersCopy());getExtraData().put("peers",peers);}
    }catch(const std::exception& error){std::clog<<error.what()<<'\n';}
    services.readLock(false);
    return *this;
}
std::optional<std::string> Greeting::getExtraDataValue(const std::string& key) const {const auto& value=getExtraData().get(key);return value.isNull()?std::nullopt:std::optional(value.asString());}
MiniNumber Greeting::getRootBlock() const {
    if(mTopBlock.isEqual(MiniNumber(-1))) return MiniNumber(-1);
    if(mTopBlock.isMore(MiniNumber::TRILLION)) {std::clog<<"[!] Greeting TopBlock error topblock:"<<mTopBlock.toString()<<" ChainSize:"<<mChain.size()<<'\n';return MiniNumber(-1);}
    return mTopBlock.sub(MiniNumber(int64_t(mChain.size())-1));
}
std::vector<uint8_t> Greeting::serialise() const {
    auto out=wire::join(mVersion,MiniString(getExtraData().toString()),mTopBlock,MiniNumber(int64_t(mChain.size())));for(const auto& id:mChain) wire::append(out,id.serialise());return out;
}
Greeting Greeting::deserialise(std::span<const uint8_t> data,size_t& offset) {
    Greeting greeting;greeting.mVersion=MiniString::deserialise(data,offset);auto text=MiniString::deserialise(data,offset);
    try {
        auto value=json::parser::JSONParser().parse(text.toString());
        if(value.isNull()) greeting.mExtraData.reset();else greeting.mExtraData=*std::get<std::shared_ptr<json::JSONObject>>(value.value);
    } catch(const json::parser::ParseException& error) {std::clog<<error.what()<<'\n';greeting.mExtraData.emplace();}
    greeting.mTopBlock=MiniNumber::deserialise(data,offset);int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i) greeting.mChain.push_back(MiniData::deserialise(data,offset));return greeting;
}
}
