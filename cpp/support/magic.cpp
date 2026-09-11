#include "org/minima/objects/TxPoW.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
#include <functional>
using namespace minima;using namespace minima::json;
using Child=std::shared_ptr<Streamable>;
static JSONObject& object(const JSONValue& v){return *std::get<std::shared_ptr<JSONObject>>(v.value);}
static JSONArray& array(const JSONValue& v){return *std::get<std::shared_ptr<JSONArray>>(v.value);}
static int integer(const JSONObject& q,const std::string& k){return int(std::get<int64_t>(q.get(k).value));}
static std::string category(const std::exception& e){auto s=nativeExceptionText(e);return s.substr(0,s.find(':'));}
static Child get(const Magic& m,int i){switch(i){
    case 0:return m.getMaxTxPoWSize();
    case 1:return m.getMaxKISSOps();
    case 2:return m.getMaxNumTxns();
    case 3:return m.getMinTxPowWork();
    case 4:return m.mDesiredMaxTxPoWSize();
    case 5:return m.mDesiredMaxKISSVMOps();
    case 6:return m.mDesiredMaxTxnPerBlock();
    case 7:return m.mDesiredMinTxPoWWork();
    default:throw std::invalid_argument("field index");}}
static void set(Magic& m,int i,Child p){switch(i){
    case 0:m.mCurrentMaxTxPoWSize()=std::dynamic_pointer_cast<MiniNumber>(p);break;
    case 1:m.mCurrentMaxKISSVMOps()=std::dynamic_pointer_cast<MiniNumber>(p);break;
    case 2:m.mCurrentMaxTxnPerBlock()=std::dynamic_pointer_cast<MiniNumber>(p);break;
    case 3:m.mCurrentMinTxPoWWork()=std::dynamic_pointer_cast<MiniData>(p);break;
    case 4:m.setDesiredMaxTxPoWSize(std::dynamic_pointer_cast<MiniNumber>(p));break;
    case 5:m.setDesiredKISSVM(std::dynamic_pointer_cast<MiniNumber>(p));break;
    case 6:m.setDesiredMaxTxns(std::dynamic_pointer_cast<MiniNumber>(p));break;
    case 7:m.mDesiredMinTxPoWWork()=std::dynamic_pointer_cast<MiniData>(p);break;
    default:throw std::invalid_argument("field index");}}
static Child item(int i,const JSONValue& v){if(v.isNull())return nullptr;if(i==3||i==7)return std::make_shared<MiniData>(MiniData::fromHex(v.asString()));return std::make_shared<MiniNumber>(v.asString());}
static JSONValue text(const Child& p){if(!p)return JSONValue();if(auto n=std::dynamic_pointer_cast<MiniNumber>(p))return n->toString();return std::dynamic_pointer_cast<MiniData>(p)->to0xString();}
static JSONArray fields(const Magic& m){JSONArray out;for(int i=0;i<8;++i)out.add(text(get(m,i)));return out;}
static void init(Magic& m,const JSONObject& q){if(q.containsKey("values")){auto v=array(q.get("values")).elements();for(int i=0;i<8;++i)set(m,i,item(i,v.at(i)));}}
template<class T> class Fault:public T{public:std::function<void()> action;void writeDataStream(std::vector<uint8_t>& out)const override{out.push_back(0xAB);if(action)action();throw wire::IOException("fixture failure");}};
static JSONArray run(const JSONObject& q){
    auto owner=std::make_shared<Magic>();init(*owner,q);Magic alias=*owner;TxPoW containing;containing.setMagic(*owner);
    std::vector<Child> held;for(int i=0;i<8;++i)held.push_back(get(*owner,i));JSONArray output;
    for(const auto& raw:array(q.get("ops")).elements()){
        const auto& op=object(raw);auto name=op.getString("op");JSONObject result;
        try{
            if(name=="read"||name=="mutate"){
                auto wire=MiniData::fromHex(op.getString("wire")).bytes();size_t offset=op.containsKey("offset")?size_t(integer(op,"offset")):0;
                try{Streamable& target=name=="read"?static_cast<Streamable&>(Magic::required(owner)):Magic::required(held.at(integer(op,"index")));target.readDataStream(wire,offset);}catch(const std::exception& e){result.put("error",category(e));}result.put("offset",int64_t(offset));
            }else if(name=="write"){
                auto out=MiniData::fromHex("0xAA55").bytes();try{Streamable& target=Magic::required(owner);target.writeDataStream(out);}catch(const std::exception& e){result.put("error",category(e));}result.put("wire",MiniData(out).to0xString());
            }else if(name=="json")result.put("value",Magic::required(owner).toJSON());
            else if(name=="same"){Magic other;init(other,op);result.put("value",Magic::required(owner).checkSame(op.containsKey("nullOther")&&op.getBoolean("nullOther")?nullptr:&other));}
            else if(name=="next"){auto next=Magic::required(owner).calculateNewCurrent();result.put("value",fields(next));result.put("wire",MiniData(next.serialise()).to0xString());if(op.containsKey("install")&&op.getBoolean("install"))owner=std::make_shared<Magic>(std::move(next));}
            else if(name=="set")set(Magic::required(owner),integer(op,"index"),item(integer(op,"index"),op.get("value")));
            else if(name=="setHeld")set(Magic::required(owner),integer(op,"index"),held.at(integer(op,"from")));
            else if(name=="cache")result.put("value",Magic::required(std::dynamic_pointer_cast<MiniData>(held.at(integer(op,"index")))).getDataValue().toDecimalString());
            else if(name=="replace")owner=std::make_shared<Magic>();
            else if(name=="release")owner.reset();
            else if(name=="defaults"){
                Magic fresh;JSONArray same;
                for(int i=0;i<4;++i){same.add(get(alias,i)==get(alias,i+4));same.add(get(alias,i)==get(fresh,i));}
                result.put("value",same);result.put("fresh",fields(fresh));
            }else if(name=="fault"){
                int index=integer(op,"index");std::function<void()> action;
                if(op.containsKey("replaceField")){std::weak_ptr<Magic> weak=owner;action=[weak,index]{auto m=weak.lock();if(m)set(*m,index,item(index,JSONValue(index==3||index==7?"0x99":"99")));};}
                if(index==3||index==7){auto f=std::make_shared<Fault<MiniData>>();f->action=action;set(Magic::required(owner),index,f);}else{auto f=std::make_shared<Fault<MiniNumber>>();f->action=action;set(Magic::required(owner),index,f);}
            }else throw std::invalid_argument("unknown operation");
        }catch(const std::exception& e){result.put("error",category(e));}
        result.put("state",owner?JSONValue(fields(*owner)):JSONValue());result.put("alias",fields(alias));result.put("containing",fields(containing.getMagic()));
        JSONArray old,same;for(int i=0;i<8;++i){old.add(text(held[i]));same.add(owner&&get(*owner,i)==held[i]);}result.put("held",old);result.put("same",same);output.add(result);
    }
    return output;
}
int main(int argc,char** argv){if(argc>1&&std::string(argv[1])=="--demo"){Magic::main();return 0;}std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto q=JSONValue::parseWithException(line);std::cout<<run(object(q)).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
