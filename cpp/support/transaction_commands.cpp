#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include <iostream>
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaUnicode.hpp"
using namespace minima;using namespace minima::json;
static auto row(std::optional<std::string> id,int n){auto tx=std::make_shared<Transaction>();auto witness=std::make_shared<Witness>();for(int i=0;i<n%4;++i)tx->addInput(Coin(i==0&&n%3==0?MiniData::fromHex("0x03"):MiniData(BigInt(n+i+1)),MiniData::fromHex("0xabcd"),MiniNumber(i+10),Token::TOKENID_MINIMA));for(int i=0;i<n%5;++i)tx->addOutput(Coin(Coin::COINID_OUTPUT,MiniData::fromHex("0x1234"),MiniNumber(i+1),Token::TOKENID_MINIMA));for(int i=0;i<n%3;++i)tx->addStateVariable(StateVariable(i,"[hello "+std::to_string(n)+"]"));if(n%2==0)witness->addScript(ScriptProof("RETURN TRUE"));return std::make_shared<TxnRow>(id,tx,witness);}
namespace minima {struct TxPoWSearcherFixture {static void reset(){std::lock_guard lock(TxPoWSearcher::mImportedMutex);TxPoWSearcher::mImportedTokens.clear();}};}
static JSONValue run(const JSONObject& request){
    TxPoWSearcherFixture::reset();TxPoWSearcher::services.getTip=[](){return std::shared_ptr<TxPoWTreeNode>{};};
    auto owner=std::make_shared<TxnDB>(std::make_shared<UserDB>());auto& db=*owner;TxnDB::runtimeLookup=[owner]{return owner;};JSONArray results;
    for(const auto& value:std::get<std::shared_ptr<JSONArray>>(request.get("ops").value)->elements()){
        auto& op=*std::get<std::shared_ptr<JSONObject>>(value.value);JSONValue result="OK";
        try{
            if(op.containsKey("fixture")){
                auto fixture=row(op.get("id").asString(),int(std::get<int64_t>(op.get("fixture").value)));fixture->getTransaction()->mHaveCheckedMonotonic=true;fixture->getTransaction()->mIsMonotonic=true;
                fixture->getWitness()->addCoinProof(CoinProof(Coin(MiniData::fromHex("0x01"),MiniData::fromHex("0xab"),MiniNumber::ONE,Token::TOKENID_MINIMA),MMRProof()));Signature signature;signature.addSignatureProof(SignatureProof(MiniData::fromHex("0x11"),MiniData::fromHex("0x22"),MMRProof()));fixture->getWitness()->addSignature(signature);db.addCompleteTransaction(fixture);
            }else if(op.containsKey("line"))result=CommandRunner::getRunner().runMultiCommand(op.containsKey("caller")?op.get("caller").asString():"0x00",op.get("line").asString());
            else{
                auto command=CommandRunner::getRunner().getCommandOnly(op.get("name").asString());if(!command)throw std::runtime_error("Missing command");for(const auto& [key,value]:std::get<std::shared_ptr<JSONObject>>(op.get("params").value)->entries())command->getParams()->put(key,value);
                if(op.containsKey("meta")&&op.get("meta").asBool()){JSONObject meta;meta.put("name",command->getName().value());meta.put("help",command->getHelp().value());meta.put("fullhelp",command->getFullHelp().value());JSONArray valid;for(const auto& p:command->getValidParams())valid.add(p);meta.put("valid",valid);meta.put("fresh",command->getFunction()->getJSONReply());result=meta;}else result=command->runCommand();
            }
        }catch(const CommandException& e){result=std::string("COMMAND_ERROR:")+e.what();}catch(const std::exception&){result="ERROR";}
        JSONObject snapshot;snapshot.put("result",result);snapshot.put("wire",MiniData(db.serialise()).to0xString());JSONArray flags;for(const auto& row:*db.listTxns())flags.add(row->getTransaction()->isCheckedMonotonic());snapshot.put("checked",flags);results.add(snapshot);
    }TxnDB::runtimeLookup={};return results;
}
int main(){auto logger=MinimaLogger::getRuntime();logger.println=[](const std::u16string& text){std::cerr<<unicode::encodeUTF8(text)<<'\n';};MinimaLogger::setRuntime(std::move(logger));std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);std::cout<<run(*request).toJSONString()<<'\n';}catch(const std::exception&){TxnDB::runtimeLookup={};std::cout<<"\"ERROR\"\n";}}
