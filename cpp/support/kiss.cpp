#include "org/minima/kissvm/values/Value.hpp"
#include "org/minima/kissvm/values/NumberValue.hpp"
#include "org/minima/kissvm/values/HexValue.hpp"
#include "org/minima/kissvm/values/StringValue.hpp"
#include "org/minima/kissvm/tokens/ScriptTokenizer.hpp"
#include "org/minima/kissvm/tokens/LexicalTokenizer.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <iostream>
#include <sstream>
using namespace minima;using namespace minima::kissvm;
static std::string str(const std::string& hex) {auto bytes=BaseConverter::decode16(hex);return MiniString(std::span<const uint8_t>(bytes)).toString();}
static json::JSONArray result(const Value& value) {
    json::JSONArray out;out.add(value.getValueType());out.add(value.toString());
    if(auto h=dynamic_cast<const HexValue*>(&value)) out.add(BaseConverter::encode16(h->getRawData()));
    if(auto s=dynamic_cast<const StringValue*>(&value)) out.add(BaseConverter::encode16(s->getBytes()));
    if(auto n=dynamic_cast<const NumberValue*>(&value)) out.add(BaseConverter::encode16(n->getNumber().serialise()));return out;
}
int main() {
    std::string line;
    while(std::getline(std::cin,line)) {
        std::string out;
        try {
            std::istringstream input(line);std::vector<std::string> a;std::string word;while(input>>word) a.push_back(word);const auto& command=a.at(0);json::JSONArray rows;
            if(command=="value") out=result(*Value::getValue(str(a.at(1)))).toJSONString();
            else if(command=="type") out=std::to_string(Value::getValueType(str(a.at(1))));
            else if(command=="hexnumber") out=result(HexValue(MiniNumber(str(a.at(1))))).toJSONString();
            else if(command=="size") {int n=std::stoi(a.at(2));if(n<0) throw std::invalid_argument("negative size");if(a.at(1)=="hex") out=result(HexValue(std::vector<uint8_t>(size_t(n)))).toJSONString();else {std::string text;for(int i=0;i<n;++i) text+="🐼";out=result(StringValue(text)).toJSONString();}}
            else if(command=="verify") {Value::getValue(str(a.at(1)))->verifyType(std::stoi(a.at(2)));out="OK";}
            else if(command=="same") {Value::checkSameType(*Value::getValue(str(a.at(1))),*Value::getValue(str(a.at(2))),std::stoi(a.at(3)));out="OK";}
            else if(command=="number") {NumberValue x(str(a.at(1))),y(str(a.at(2)));rows.add(result(x.add(y)));rows.add(result(x.sub(y)));rows.add(result(x.mult(y)));rows.add(result(x.div(y)));rows.add(x.isEqual(y));rows.add(x.isLess(y));rows.add(x.isMoreEqual(y));out=rows.toJSONString();}
            else if(command=="string") out=result(StringValue(str(a.at(1))).add(StringValue(str(a.at(2))))).toJSONString();
            else if(command=="tokens") {auto tokens=ScriptTokenizer(str(a.at(1)),a.at(2)=="1").tokenize();for(const auto& token:tokens) {json::JSONArray row;row.add(token.getTokenType());row.add(token.getToken());row.add(token.toString());rows.add(row);}out=rows.toJSONString();}
            else if(command=="lexical") {auto tokens=ScriptToken::tokenize(str(a.at(1)));LexicalTokenizer lexical(tokens,std::stoi(a.at(2)));rows.add(lexical.getStackDepth());lexical.incrementStackDepth();lexical.decrementStackDepth();lexical.decrementStackDepth();rows.add(lexical.getStackDepth());while(lexical.hasMoreElements()) {rows.add(lexical.getCurrentPosition());rows.add(lexical.getNextToken().toString());}rows.add(lexical.checkAllTokensUsed());if(!tokens.empty()) {lexical.goBackToken();rows.add(lexical.getNextToken().toString());}out=rows.toJSONString();}
            else if(command=="check") {auto word=str(a.at(1));rows.add(ScriptTokenizer::isNumeric(word));rows.add(ScriptTokenizer::isHex(word));rows.add(ScriptTokenizer::isVariable(word));rows.add(ScriptTokenizer::isGlobal(word));rows.add(ScriptTokenizer::isWhiteSpace(word));out=rows.toJSONString();}
            else throw std::invalid_argument("Unknown command");
        }catch(const MinimaException& error) {out=std::string("PARSE|")+error.what();}catch(const std::exception&) {out="ERROR";}
        std::cout<<BaseConverter::encode16(std::vector<uint8_t>(out.begin(),out.end()))<<'\n';
    }
}
