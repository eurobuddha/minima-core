#include "Contract.hpp"
#include "exceptions/MinimaParseException.hpp"
#include "functions/MinimaFunction.hpp"
#include "statements/StatementParser.hpp"
#include "tokens/ScriptTokenizer.hpp"
#include "support/JavaUnicode.hpp"
#include "support/JavaExceptions.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <bit>
#include <iostream>
#include <algorithm>
namespace minima::kissvm {
const MiniNumber Contract::MAX_BITSHIFT(256);
namespace {
int32_t addInt(int32_t a,int32_t b) {return std::bit_cast<int32_t>(uint32_t(a)+uint32_t(b));}
std::string trim(std::string text) {auto first=text.find_first_not_of(std::string("\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37 ",33));if(first==std::string::npos) return "";auto last=text.size();while(last>first&&uint8_t(text[last-1])<=32) --last;return text.substr(first,last-first);}
std::string arrayName(std::string name) {if(name.find(',')!=std::string::npos) {std::replace(name.begin(),name.end(),',',' ');return "( "+trim(name)+" )";}return name;}
bool contains(const std::vector<std::string>& list,const std::string& item) {return std::find(list.begin(),list.end(),item)!=list.end();}
}
std::string exceptionString(const std::exception& error) {
    if(dynamic_cast<const ExecutionException*>(&error)) return std::string("org.minima.kissvm.exceptions.ExecutionException: ")+error.what();
    if(dynamic_cast<const MinimaParseException*>(&error)) return std::string("org.minima.kissvm.exceptions.MinimaParseException: ")+error.what();
    if(dynamic_cast<const MinimaException*>(&error)) return std::string("org.minima.kissvm.exceptions.MinimaException: ")+error.what();
    if(dynamic_cast<const JavaNullPointerException*>(&error)) return javaExceptionText(error);
    return std::string("NativeException: ")+error.what();
}
Contract::Contract(std::string script,const std::string&,std::shared_ptr<Witness> witness,std::shared_ptr<Transaction> transaction,std::vector<StateVariable> previous,bool trace):Contract(std::move(script),std::vector<MiniData>{},std::move(witness),std::move(transaction),std::move(previous),trace){}
Contract::Contract(std::string script,const std::vector<MiniData>& signatures,std::shared_ptr<Witness> witness,std::shared_ptr<Transaction> transaction,std::vector<StateVariable> previous,bool trace):mTransaction(std::move(transaction)),mWitness(std::move(witness)),mRamScript(std::move(script)),mPrevState(std::move(previous)),mTraceON(trace) {
    if(!mTransaction||!mWitness) throw std::invalid_argument("Null Contract transaction or witness");
    traceLog("Contract   : "+mRamScript);traceLog("Size       : "+std::to_string(unicode::decodeUTF8(mRamScript).size()));
    traceLog("Transaction   : "+mTransaction->toString());traceLog("Witness       : "+mWitness->toString());
    for(const auto& sig:signatures) {traceLog("Signature : "+sig.to0xString());mSignatures.emplace_back(sig);}
    for(const auto& state:mTransaction->getCompleteState()) traceLog("State["+std::to_string(state.getPort())+"] : "+state.toString());
    for(const auto& state:mPrevState) traceLog("PrevState["+std::to_string(state.getPort())+"] : "+state.toString());
    try {
        auto tokens=ScriptTokenizer(mRamScript).tokenize();int count=0;
        for(const auto& token:tokens) traceLog(std::to_string(count++)+") Token : ["+token.getTokenTypeString()+"] "+token.getToken());
        resetStackDepth();mBlock=StatementParser::parseTokens(tokens,0);traceLog("Script token parse OK.");mParseOK=true;
    } catch(const std::exception& error) {mException=true;mExceptionString=exceptionString(error);traceLog("PARSE ERROR : "+mExceptionString);}
}
void Contract::setGlobals(const MiniNumber& block,const MiniNumber& time,const Transaction& txn,int input,const MiniNumber& created,const std::string& script) {
    const auto& coin=txn.getAllInputs().at(input);
    setGlobalVariable("@BLOCK",std::make_shared<NumberValue>(block));setGlobalVariable("@BLOCKMILLI",std::make_shared<NumberValue>(time));
    setGlobalVariable("@CREATED",std::make_shared<NumberValue>(created));setGlobalVariable("@COINAGE",std::make_shared<NumberValue>(block.sub(created)));
    setGlobalVariable("@INPUT",std::make_shared<NumberValue>(int64_t(input)));setGlobalVariable("@COINID",std::make_shared<HexValue>(coin.getCoinID()));
    auto amount=coin.getAmount();if(!coin.getTokenID().isEqual(Token::TOKENID_MINIMA)) {
        if(!coin.getToken())throw JavaNullPointerException("null input token");
        amount=coin.getToken()->getScaledTokenAmount(amount);
    }
    setGlobalVariable("@AMOUNT",std::make_shared<NumberValue>(amount));setGlobalVariable("@ADDRESS",std::make_shared<HexValue>(coin.getAddress()));
    setGlobalVariable("@TOKENID",std::make_shared<HexValue>(coin.getTokenID()));setGlobalVariable("@SCRIPT",std::make_shared<StringValue>(script));
    setGlobalVariable("@TOTIN",std::make_shared<NumberValue>(int64_t(txn.getAllInputs().size())));setGlobalVariable("@TOTOUT",std::make_shared<NumberValue>(int64_t(txn.getAllOutputs().size())));
}
void Contract::setGlobalVariable(const std::string& name,ValuePtr value) {if(!value) throw std::invalid_argument("Null global value");mGlobals->put(name,value);traceLog("Global ["+name+"] : "+value->toString());}
ValuePtr Contract::getGlobal(const std::string& name) {auto value=mGlobals->get(name);if(!value) throw ExecutionException("Global not found - "+name);if(name=="@BLOCK"||name=="@BLOCKMILLI"||name=="@COINAGE") mMonotonic=false;return *value;}
void Contract::traceLog(const std::string& log) {if(mTraceON) std::clog<<"INST["<<mNumInstructions<<"] STACK["<<mStackDepth<<"] - "<<log<<'\n';mCompleteLog+="INST["+std::to_string(mNumInstructions)+"] - "+log+"\n";}
void Contract::incrementInstructions(int number) {mNumInstructions=addInt(mNumInstructions,number);if(mNumInstructions>MAX_INSTRUCTIONS) throw ExecutionException("MAX instruction number reached! "+std::to_string(mNumInstructions));}
void Contract::incrementStackDepth() {mStackDepth=addInt(mStackDepth,1);if(mStackDepth>MAX_STACK_DEPTH) throw ExecutionException("Stack depth too deep! (MAX "+std::to_string(MAX_STACK_DEPTH)+") "+std::to_string(mStackDepth));}
void Contract::decrementStackDepth() {mStackDepth=addInt(mStackDepth,-1);}
void Contract::run() {
    if(!mParseOK) {traceLog("Script parse FAILED. Please fix and retry.");return;}resetStackDepth();
    try {traceLog("Start executing the contract");mBlock->run(*this);}
    catch(const std::exception& error) {mException=true;mExceptionString=exceptionString(error);traceLog("Execution Error - "+mExceptionString);mSuccess=false;mSuccessSet=true;}
    traceLog("Contract instructions : "+std::to_string(mNumInstructions));traceLog(std::string("Contract finished     : ")+(mSuccess?"true":"false"));
}
json::JSONObject Contract::getAllVariables() const {json::JSONObject variables;for(const auto& key:mVariables.keys()) variables.put(arrayName(key),mVariables.get(key)->get()->toString());return variables;}
void Contract::setVariable(const std::string& name,ValuePtr value) {if(!value) throw std::invalid_argument("Null variable value");mVariables.put(name,std::move(value));traceVariables();}
ValuePtr Contract::getState(int port) const {auto value=mTransaction->getStateValue(port);if(!value) throw ExecutionException("State Variable does not exist "+std::to_string(port));return Value::getValue(value->toString());}
ValuePtr Contract::getPrevState(int port) const {for(const auto& state:mPrevState) if(state.getPort()==port) return Value::getValue(state.toString());throw ExecutionException("PREVSTATE Missing : "+std::to_string(port));}
void Contract::traceVariables() {std::string log="{ ";for(const auto& key:mVariables.keys()) log+=arrayName(key)+" = "+mVariables.get(key)->get()->toString()+", ";traceLog(log+"}");}
bool Contract::checkSignature(const HexValue& signature) const {return std::any_of(mSignatures.begin(),mSignatures.end(),[&](const auto& sig){return sig.isEqual(signature);});}
std::string Contract::cleanScript(const std::string& input,bool log) {
    std::string script;bool space=false;for(char c:input) {if(ScriptTokenizer::isWhiteSpace(std::string(1,c))) {if(!space) script+=' ';space=true;}else {script+=c;space=false;}}script=trim(script);
    std::string result;bool whites=true;std::string previous;bool hasPrevious=false;
    try {
        for(const auto& token:ScriptTokenizer(script,true).tokenize()) {
            auto text=token.getToken();if(log) MinimaLogger::log(token.toString());
            if(token.getTokenType()==ScriptToken::TOKEN_COMMAND) {if(!result.ends_with(" ")) result+=' ';result+=text+" ";whites=true;}
            else if(contains(ScriptTokenizer::TOKENS_BOOLEAN_OPERATOR,text)) {result+=" "+text+" ";whites=true;}
            else if(text.starts_with("0x")) {
                for(size_t i=2;i<text.size();++i) if(text[i]>='a'&&text[i]<='z') text[i]-=32;
                if((hasPrevious&&previous.ends_with(")"))||!whites) result+=' ';result+=text;whites=false;
            } else if(text.starts_with("(")||text.starts_with("[")) {
                bool needSpace=hasPrevious&&(whites?(previous.ends_with(")")||previous.ends_with("]")):contains(ScriptTokenizer::TOKENS_ENDOFWORD,previous));
                if(needSpace) result+=' ';result+=text;whites=true;
            } else {
                bool lastClose=hasPrevious&&(previous.ends_with(")")||previous.ends_with("]"));
                if(lastClose&&!contains(ScriptTokenizer::TOKENS_AFTERBRACKET,text)) {result+=" "+text;whites=false;}
                else if(ScriptTokenizer::isWhiteSpace(text)||contains(ScriptTokenizer::TOKENS_ENDOFWORD,text)) {result+=text;whites=true;}
                else {if(!whites) result+=' ';result+=text;whites=false;}
            }
            previous=token.getToken();hasPrevious=true;
        }
    } catch(const MinimaParseException& error) {MinimaLogger::log("Clean Script Error @ "+input+" "+exceptionString(error));return input;}
    return trim(result);
}
NumberValue Contract::getNumberParam(int number,MinimaFunction& function) {auto value=function.getParameter(number)->getValue(*this);if(value->getValueType()!=Value::VALUE_NUMBER) throw ExecutionException("Incorrect Parameter type - should be NumberValue @ "+std::to_string(number)+" "+function.getName());return static_cast<NumberValue&>(*value);}

HexValue Contract::getHexParam(int number,MinimaFunction& function) {auto value=function.getParameter(number)->getValue(*this);if(value->getValueType()!=Value::VALUE_HEX) throw ExecutionException("Incorrect Parameter type - should be HEXValue @ "+std::to_string(number)+" "+function.getName());return static_cast<HexValue&>(*value);}

StringValue Contract::getStringParam(int number,MinimaFunction& function) {auto value=function.getParameter(number)->getValue(*this);if(value->getValueType()!=Value::VALUE_SCRIPT) throw ExecutionException("Incorrect Parameter type - should be ScriptValue @ "+std::to_string(number)+" "+function.getName());return static_cast<StringValue&>(*value);}

BooleanValue Contract::getBoolParam(int number,MinimaFunction& function) {auto value=function.getParameter(number)->getValue(*this);if(value->getValueType()!=Value::VALUE_BOOLEAN) throw ExecutionException("Incorrect Parameter type - should be BooleanValue @ "+std::to_string(number)+" "+function.getName());return static_cast<BooleanValue&>(*value);}

}
