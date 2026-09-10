#include "org/minima/system/network/p2p2/P2P2DB.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static std::shared_ptr<JSONObject> object(const JSONValue& value){return std::get<std::shared_ptr<JSONObject>>(value.value);}
static std::shared_ptr<JSONArray> array(const JSONValue& value){return std::get<std::shared_ptr<JSONArray>>(value.value);}
static std::optional<std::string> string(const JSONValue& value){return value.isNull()?std::nullopt:std::optional(value.asString());}
static JSONValue run(const JSONObject& request){
    p2p2::P2P2DB db;JsonDB& base=db;auto alias=db.getAllKnownPeers();
    auto directory=std::filesystem::temp_directory_path()/("minima-p2p2-fixture-"+MiniData::getRandomData(12).toHexString(false));
    if(!std::filesystem::create_directory(directory))throw std::runtime_error("temporary directory");
    struct Cleanup{std::filesystem::path path;~Cleanup(){std::error_code error;std::filesystem::remove_all(path,error);}}cleanup{directory};
    auto file=directory/"peers.db";MiniFile::writeDataToFile(directory/"blocked",{});JSONArray states;
    for(const auto& item:array(request.get("ops"))->elements()){
        auto op=object(item);auto name=op->get("op").asString();auto value=op->get("value");JSONValue result="OK";
        try{
            if(name=="add")result=db.addPeerToAllKnown(string(value));
            else if(name=="append")alias->push_back(string(value));
            else if(name=="clear")alias->clear();
            else if(name=="first")db.setFirstStartUp(value.asBool());
            else if(name=="put"){auto data=db.getAllData();if(!data)throw std::runtime_error("Null data");data->put(op->get("key").asString(),value);}
            else if(name=="read"){auto bytes=MiniData::fromHex(value.asString());size_t offset=0;db.readDataStream(bytes.bytes(),offset);}
            else if(name=="file")MiniFile::writeDataToFile(file,MiniData::fromHex(value.asString()).bytes());
            else if(name=="save")base.saveDB(file);
            else if(name=="saveFail")base.saveDB(directory/"blocked"/"file");
            else if(name=="load")base.loadDB(file);
            else if(name=="loadMissing")base.loadDB(directory/"missing");
            else throw std::invalid_argument(name);
        }catch(const std::exception&){result="ERROR";}
        JSONObject state;state.put("result",result);state.put("peers",*MiniUtil::convertArrayList(*db.getAllKnownPeers()));state.put("alias",alias==db.getAllKnownPeers());state.put("data",db.getAllData()?JSONValue(*db.getAllData()):JSONValue{});
        try{state.put("first",db.isFirstStartUp());}catch(const std::exception&){state.put("first","ERROR");}
        try{state.put("wire",MiniData(db.serialise()).to0xString());}catch(const std::exception&){state.put("wire","ERROR");}
        if(std::filesystem::exists(file))state.put("file",MiniData(MiniFile::readCompleteFile(file)).to0xString());else state.put("file",nullptr);
        states.add(JSONValue::parseWithException(state.toJSONString()));
    }
    return states;
}
int main(){std::string line;while(std::getline(std::cin,line))try{std::cout<<run(*object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\"\n";}}
