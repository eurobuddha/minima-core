#include "CoinFixture.hpp"
#include "org/minima/utils/sphincs/TransactionTest.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
#include <sstream>
using namespace minima;using Object=json::JSONObject;using minima::sphincs::TransactionTest;
static MiniData data(const json::JSONValue& v){return MiniData::fromHex(v.asString());}
static Object error(const std::exception& e){Object r;auto text=nativeExceptionText(e);auto type=text.substr(0,text.find(": "));r.put("exception",type);r.put("message",type=="java.lang.NullPointerException"||type=="java.io.EOFException"?"":e.what());return r;}
static std::optional<Coin> coin(const Object& q){
    if(q.containsKey("nullCoin")&&q.get("nullCoin").asBool())return std::nullopt;auto base=support::coinFixture(q.containsKey("index")?int(std::get<int64_t>(q.get("index").value)):0);
    Coin c(base.getCoinID(),q.containsKey("address")?data(q.get("address")):base.getAddress(),q.containsKey("amount")?MiniNumber(q.get("amount").asString()):base.getAmount(),q.containsKey("tokenID")?data(q.get("tokenID")):base.getTokenID(),q.containsKey("keep")?q.get("keep").asBool():base.storeState());
    c.setToken(base.getToken());if(q.containsKey("scale"))c.setToken(Token(MiniData::fromHex("0x99"),MiniNumber(q.get("scale").asString()),MiniNumber::ONE,MiniString("fixture"),MiniString("RETURN TRUE")));return c;
}
int main(int argc,char** argv){if(argc>1&&std::string(argv[1])=="--demo"){TransactionTest::main();return 0;}std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line)){auto q=std::get<std::shared_ptr<Object>>(json::JSONValue::parseWithException(line).value);Object r;std::ostringstream printed;auto previous=std::cout.rdbuf(printed.rdbuf());
        try{if(q->get("action").asString()=="log")TransactionTest::log(q->get("message").isNull()?std::nullopt:std::optional<std::string>(q->get("message").asString()));else{auto c=coin(*q);r.put("value",TransactionTest::getOutCoinString(c?&*c:nullptr));}}catch(const std::exception& e){r.put("error",error(e));}
        std::cout.rdbuf(previous);r.put("printed",printed.str());std::cout<<r.toJSONString()<<'\n';
    }
}
