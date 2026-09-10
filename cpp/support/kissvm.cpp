#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/expressions/ExpressionParser.hpp"
#include "org/minima/kissvm/tokens/ScriptToken.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <iostream>
#include <sstream>
using namespace minima;using namespace minima::kissvm;
static std::string str(const std::string& hex) {auto bytes=BaseConverter::decode16(hex);return MiniString(std::span<const uint8_t>(bytes)).toString();}
static json::JSONArray value(const Value& value) {json::JSONArray out;out.add(value.getValueType());out.add(value.toString());if(auto number=dynamic_cast<const NumberValue*>(&value)) out.add(BaseConverter::encode16(number->getNumber().serialise()));return out;}
static Contract contract(const std::string& script,int max,bool rich) {
    auto txn=std::make_shared<Transaction>();txn->addStateVariable(StateVariable(0,"17"));txn->addStateVariable(StateVariable(1,"[hello]"));
    std::vector<StateVariable> prev{StateVariable(0,"12"),StateVariable(1,"[old]")};
    auto witness=std::make_shared<Witness>();std::vector<MiniData> signatures;std::optional<Token> token;
    if(rich) {
        token=Token(MiniData::fromHex("0x99"),MiniNumber(2),MiniNumber::ONE,MiniString("Token"),MiniString("RETURN TRUE"));
        for(int i=0;i<3;++i) {
            auto id=i==0?Token::TOKENID_MINIMA:i==1?token->getTokenID().value():MiniData::fromHex("0x77");
            Coin coin(MiniData::fromHex("0x1"+std::to_string(i)),MiniData::fromHex("0x2"+std::to_string(i)),MiniNumber(i==0?"7":"0.25"),id,true);if(i==1) coin.setToken(*token);txn->addInput(coin);
            auto proofcoin=coin.deepCopy();proofcoin.resetCoinID(MiniData::fromHex("0xA"+std::to_string(i)));witness->addCoinProof(CoinProof(proofcoin,MMRProof()));
            Coin output(MiniData::fromHex("0x2"+std::to_string(i)),MiniNumber(i==0?"5":"0.125"),id,i==0);if(i==1) output.setToken(*token);txn->addOutput(output);
        }
        signatures={MiniData::fromHex("0xCA"),MiniData::fromHex("0xFE")};witness->addScript(ScriptProof("RETURN TRUE"));witness->addScript(ScriptProof("LET x=42"));
    }
    Contract ctr(script,signatures,witness,txn,prev);ctr.setMaxInstructions(max);
    if(rich) {ctr.setGlobals(MiniNumber(123),MiniNumber(987654321),*txn,0,MiniNumber(100),script);ctr.setVariable("tok",std::make_shared<HexValue>(*token->getTokenID()));}
    ctr.setVariable("a",std::make_shared<NumberValue>(7));ctr.setVariable("b",std::make_shared<NumberValue>(3));ctr.setVariable("t",Value::getValue("TRUE"));ctr.setVariable("f",Value::getValue("FALSE"));
    ctr.setVariable("h",std::make_shared<HexValue>("0x00FF"));ctr.setVariable("s",std::make_shared<StringValue>("hello"));
    ctr.setGlobalVariable("@BLOCK",std::make_shared<NumberValue>(123));ctr.setGlobalVariable("@CREATED",std::make_shared<NumberValue>(100));return ctr;
}
int main() {
    std::string line;while(std::getline(std::cin,line)) {
        json::JSONArray out;
        try {
            std::istringstream input(line);std::string mode,hex;int max,rich=0;input>>mode>>hex>>max;input>>rich;auto script=str(hex);
            if(mode=="clean") out.add(Contract::cleanScript(script));
            else {
                auto ctr=contract(mode=="run"?script:"",max,rich==1);size_t offset=ctr.getCompleteTraceLog().size();std::string error;
                try {
                    if(mode=="run") ctr.run();
                    else {auto expression=ExpressionParser::getExpression(ScriptToken::tokenize(script),0);out.add(expression->toString());out.add(value(*expression->getValue(ctr)));}
                }catch(const MinimaException& e) {error=exceptionString(e);}catch(const std::exception&) {error="ERROR";}
                std::string contractError=ctr.getException();if(!contractError.empty()&&!contractError.starts_with("org.minima.kissvm.exceptions.")) contractError="ERROR";
                out.add(ctr.isParseOK());out.add(ctr.isSuccess());out.add(ctr.isSuccessSet());out.add(ctr.isException());out.add(contractError);out.add(error);
                out.add(ctr.getNumberOfInstructions());out.add(ctr.getStackDepth());out.add(ctr.isMonotonic());out.add(ctr.getAllVariables());
                out.add(error=="ERROR"||contractError=="ERROR"?"":ctr.getCompleteTraceLog().substr(offset));
            }
        }catch(const MinimaException& error) {out.add(exceptionString(error));}catch(const std::exception&) {out.add("ERROR");}
        auto text=out.toJSONString();std::cout<<BaseConverter::encode16(std::vector<uint8_t>(text.begin(),text.end()))<<'\n';
    }
}
