#include "CoinFixture.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/objects/CoinProof.hpp"
#include "org/minima/objects/Witness.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
namespace {
std::string errorText(const std::exception& e){if(dynamic_cast<const JavaNullPointerException*>(&e))return "java.lang.NullPointerException";return nativeExceptionText(e);}
template<class T> JSONValue snap(const T& value){if constexpr(requires{value.isNull();})if(value.isNull())return nullptr;try{auto json=value.toJSON().toJSONString();return BaseConverter::encode16(value.serialise())+"|"+json;}catch(const std::exception& e){return "snapshot:"+errorText(e);}}
MMRProof proof(int count){MMRProof p(MiniNumber(123));for(int i=0;i<count;++i)p.addProofChunk(i%2==0,MMRData::CreateMMRDataLeafNode(MiniString("branch"+std::to_string(i)),MiniNumber(i+1)));return p;}
// Capability probes keep the baseline executable without inventing production readers.
template<class T> std::string readInto(T& value,std::span<const uint8_t> bytes,size_t& offset){if constexpr(requires{value.readDataStream(bytes,offset);}){value.readDataStream(bytes,offset);return "ok";}else return "missing in-place reader";}
template<class T> auto holdState(T& coin){if constexpr(requires{coin.getSharedState();})return coin.getSharedState();else return std::make_shared<std::vector<StateVariable>>(coin.getState());}
template<class T> void shareState(T& coin,std::shared_ptr<std::vector<StateVariable>> state){if constexpr(requires{coin.setSharedState(state);})coin.setSharedState(std::move(state));else if(state)coin.setState(*state);else throw JavaNullPointerException("missing nullable state API");}
JSONArray states(const std::vector<StateVariable>& list){JSONArray a;for(const auto& state:list)a.add(state.toJSON());return a;}
JSONArray snapshot(const Coin& c,const Coin& other,const Coin& held,const CoinProof& cp,const CoinProof& alias,const Witness& witness,const MMRProof& p,const MMRProof& heldProof,const std::vector<StateVariable>& oldState){JSONArray out;out.add(snap(c));out.add(snap(other));out.add(snap(held));out.add(snap(cp));out.add(snap(alias));out.add(snap(witness.getAllCoinProofs()->at(0)));out.add(snap(p));out.add(snap(heldProof));out.add(states(oldState));return out;}
JSONObject run(const JSONObject& request){
    auto flag=[&](const char* key){return request.containsKey(key)&&request.get(key).asBool();};
    auto c=support::coinFixture(7);auto oldState=holdState(c);auto other=support::coinFixture(1);shareState(other,oldState);
    auto p=proof(2);CoinProof cp(c,p);auto alias=cp;Witness witness;witness.addCoinProof(cp);auto held=cp.getCoin();auto heldProof=cp.getMMRProof();
    if(flag("badHash"))c.resetCoinID(MiniData(std::vector<uint8_t>(65)));
    // Before the nullable-state API exists the baseline records the missing capability.
    bool missingNullState=false;if(flag("nullState")){try{shareState(c,nullptr);}catch(const JavaNullPointerException&){missingNullState=true;}}
    auto data=MiniData::fromHex(request.getString("wire","0x"));size_t offset=0;JSONArray logs;int prints=0,failAt=int(std::get<int64_t>(request.get("failAt").value));auto kind=request.getString("kind");
    MinimaLogger::Runtime logger;logger.println=[&](const auto& full){auto text=unicode::encodeUTF8(full);auto at=text.find("] : ");if(at==std::string::npos)return;auto body=text.substr(at+4);if(body.starts_with("     "))return;logs.add(body);if(++prints==failAt){if(kind=="error")throw JavaError("java.lang.LinkageError","logger");throw JavaTypedException("java.lang.IllegalStateException","logger");}};MinimaLogger::setRuntime(logger);struct Reset{~Reset(){MinimaLogger::setRuntime({});}}reset;
    JSONValue result="ok";auto action=request.getString("action");
    try{
        if(missingNullState)throw JavaNullPointerException("missing nullable state API");
        if(action=="readCoin")result=readInto(c,data.bytes(),offset);
        else if(action=="readProof")result=readInto(cp,data.bytes(),offset);
        else if(action=="readMMR")result=readInto(p,data.bytes(),offset);
        else if(action=="convertCoin"){auto value=Coin::convertMiniDataVersion(data);result=value?snap(*value):JSONValue(nullptr);}
        else if(action=="convertProof"){auto value=CoinProof::convertMiniDataVersion(data);result=value?snap(*value):JSONValue(nullptr);}
        else if(action=="convertMMR")result=snap(MMRProof::convertMiniDataVersion(data));
        else if(action=="floating"){
            auto node=std::make_shared<TxPoWTreeNode>(TxPoW{});
            auto previous=GeneralParams::USE_SQL_COINDB;GeneralParams::USE_SQL_COINDB=false;
            struct Restore{bool value;~Restore(){GeneralParams::USE_SQL_COINDB=value;}}restore{previous};node->addCoinToNode(c);
            auto found=TxPoWSearcher::getFloatingCoin(node,c.getAmount(),c.getAddress(),c.getTokenID());JSONObject value;value.put("present",found.has_value());value.put("value",found?snap(*found):JSONValue(nullptr));result=value;
        }
        else if(action=="copy")result=snap(c.deepCopy());
        else if(action=="rename")result=snap(c.getSameCoinWithCoinID(MiniData::fromHex("0xEE")));
        else if(action!="mutate")throw std::invalid_argument("action");
    }catch(const std::exception& e){result="escaped:"+errorText(e);}
    JSONObject out;out.put("result",result);out.put("logs",logs);out.put("consumed",int64_t(offset));
    out.put("beforeMutation",snapshot(c,other,held,cp,alias,witness,p,heldProof,*oldState));
    if(flag("after")){
        oldState->emplace_back(255,"[held list]");held.setSpent(!held.getSpent());p.addProofChunk(false,MMRData::CreateMMRDataLeafNode(MiniString("after"),MiniNumber(9)));
        cp.getCoin().setBlockCreated(MiniNumber(999));auto wrapperProof=cp.getMMRProof();wrapperProof.addProofChunk(true,MMRData::CreateMMRDataLeafNode(MiniString("wrapper"),MiniNumber(10)));
    }
    out.put("afterMutation",snapshot(c,other,held,cp,alias,witness,p,heldProof,*oldState));return out;
}
}
int main(){std::string line;while(std::getline(std::cin,line))try{auto parsed=parser::JSONParser().parse(line);std::cout<<run(*std::get<std::shared_ptr<JSONObject>>(parsed.value)).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<errorText(e)<<'\n';return 1;}}
