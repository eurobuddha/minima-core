#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static auto str(const JSONValue& v){return v.isNull()?std::optional<std::string>():std::optional(v.asString());}
static auto row(std::optional<std::string> id,int n){auto tx=std::make_shared<Transaction>();auto witness=std::make_shared<Witness>();for(int i=0;i<n%4;++i)tx->addInput(Coin(i==0&&n%3==0?MiniData::fromHex("0x03"):MiniData(BigInt(n+i+1)),MiniData::fromHex("0xabcd"),MiniNumber(i+10),Token::TOKENID_MINIMA));for(int i=0;i<n%5;++i)tx->addOutput(Coin(Coin::COINID_OUTPUT,MiniData::fromHex("0x1234"),MiniNumber(i+1),Token::TOKENID_MINIMA));for(int i=0;i<n%3;++i)tx->addStateVariable(StateVariable(i,"[hello "+std::to_string(n)+"]"));if(n%2==0)witness->addScript(ScriptProof("RETURN TRUE"));return std::make_shared<TxnRow>(id,tx,witness);}
static JSONValue snapshot(const std::shared_ptr<TxnRow>& r){if(!r)return {};JSONObject v;v.put("id",r->getID()?JSONValue(*r->getID()):JSONValue());auto tx=r->getTransaction();v.put("inputs",tx?int64_t(tx->getAllInputs().size()):-1);v.put("outputs",tx?int64_t(tx->getAllOutputs().size()):-1);v.put("state",tx?int64_t(tx->getCompleteState().size()):-1);auto w=r->getWitness();v.put("scripts",w?int64_t(w->getAllScripts()->size()):-1);return v;}
static JSONArray ids(const std::shared_ptr<TxnDB::Rows>& rows){JSONArray list;for(const auto& r:*rows)list.add(r&&r->getID()?JSONValue(*r->getID()):JSONValue());return list;}
static JSONValue run(const JSONObject& req){auto kind=req.getString("kind");
    if(kind=="wire"){
        auto data=MiniData::fromHex(req.getString("data"));size_t offset=0;JSONObject out;bool isRow=req.get("row").asBool(),convert=req.get("convert").asBool();auto r=row("old",3);auto db=std::make_shared<TxnDB>();db->createTransaction("old");
        try{if(isRow){if(convert)r=TxnRow::convertMiniDataVersion(data);else r->readDataStream(data.bytes(),offset);out.put("result",r?JSONValue(r->toJSON(req.get("show").asBool())):JSONValue());out.put("wire",r?JSONValue(MiniData(r->serialise()).to0xString()):JSONValue());}else{if(convert)db=TxnDB::convertMiniDataVersion(data);else db->readDataStream(data.bytes(),offset);out.put("result",db?JSONValue(ids(db->listTxns())):JSONValue());out.put("wire",db?JSONValue(MiniData(db->serialise()).to0xString()):JSONValue());}}
        catch(const wire::IOException&){out.put("error","IO_ERROR");}catch(const std::exception&){out.put("error","ERROR");}
        if(!convert){out.put("consumed",int64_t(offset));out.put("partial",isRow?snapshot(r):JSONValue(ids(db->listTxns())));}return out;
    }
    auto user=std::make_shared<UserDB>();TxnDB db(user);std::vector<std::shared_ptr<TxnRow>> rows;std::vector<std::shared_ptr<TxnDB::Rows>> held;JSONArray output;
    for(const auto& op:std::get<std::shared_ptr<JSONArray>>(req.get("ops").value)->elements()){auto& o=*std::get<std::shared_ptr<JSONObject>>(op.value);JSONValue result="OK";
        try{auto cmd=o.getString("cmd");auto id=str(o.get("id"));int index=o.containsKey("index")?int(std::get<int64_t>(o.get("index").value)):0;
            if(cmd=="create"){db.createTransaction(id);rows.push_back(db.listTxns()->back());}
            else if(cmd=="fixture"){auto r=row(id,index);rows.push_back(r);db.addCompleteTransaction(r);}
            else if(cmd=="rename")rows.at(index)->setID(id);
            else if(cmd=="add")db.addCompleteTransaction(rows.at(index));
            else if(cmd=="delete")result=db.deleteTransaction(id);
            else if(cmd=="get"){auto r=db.getTransactionRow(id);auto it=std::find(rows.begin(),rows.end(),r);result=!r?-1:it==rows.end()?-1:int64_t(it-rows.begin());}
            else if(cmd=="clear")db.clearTxns();
            else if(cmd=="hold")held.push_back(db.listTxns());
            else if(cmd=="clearheld")held.at(index)->clear();
            else if(cmd=="save")db.saveDB();
            else if(cmd=="load")db.loadDB();
            else if(cmd=="stored")user->saveCustomTransactions(MiniData::fromHex(o.getString("value")));
            else if(cmd=="wire")result=MiniData(db.serialise()).to0xString();
            else if(cmd=="json")result=rows.at(index)->toJSON(o.get("show").asBool());
            else if(cmd=="clearwitness")rows.at(index)->clearWitness();
            else throw std::invalid_argument(cmd);
        }catch(const wire::IOException&){result="IO_ERROR";}catch(const std::exception&){result="ERROR";}
        JSONObject state;state.put("result",result);state.put("list",ids(db.listTxns()));JSONArray aliases;for(const auto& h:held)aliases.add(ids(h));state.put("held",aliases);JSONArray rs;for(const auto& r:rows)rs.add(snapshot(r));state.put("rows",rs);output.add(state);
    }return output;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto req=std::get<std::shared_ptr<JSONObject>>(parser::JSONParser().parse(line).value);std::cout<<run(*req).toJSONString()<<'\n';}catch(const std::exception&){std::cout<<"ERROR\n";}}
