#pragma once
#include "values/BooleanValue.hpp"
#include "values/HexValue.hpp"
#include "values/NumberValue.hpp"
#include "values/StringValue.hpp"
#include "exceptions/ExecutionException.hpp"
#include "expressions/Expression.hpp"
#include "org/minima/objects/Transaction.hpp"
#include "org/minima/objects/Witness.hpp"
#include "support/JavaHashtable.hpp"
#include "support/KissLimits.hpp"
namespace minima::kissvm {
class StatementBlock;
class MinimaFunction;
class Contract {
    std::shared_ptr<Transaction> mTransaction;
    std::shared_ptr<Witness> mWitness;
    std::string mRamScript,mCompleteLog,mExceptionString;
    std::shared_ptr<StatementBlock> mBlock;
    std::vector<HexValue> mSignatures;
    JavaHashtable<ValuePtr> mVariables;
    std::shared_ptr<JavaHashtable<ValuePtr>> mGlobals=std::make_shared<JavaHashtable<ValuePtr>>();
    std::vector<StateVariable> mPrevState;
    bool mMonotonic=true,mSuccess=false,mSuccessSet=false,mTraceON=false,mParseOK=false,mException=false;
    int32_t mNumInstructions=0,mStackDepth=0;
public:
    int MAX_INSTRUCTIONS=1024;
    static constexpr int MAX_STACK_DEPTH=64,MAX_FUNCTION_PARAMS=32,MAX_DATA_SIZE=64*1024;
    static const MiniNumber MAX_BITSHIFT;
    Contract(std::string script,const std::vector<MiniData>& signatures,std::shared_ptr<Witness> witness,std::shared_ptr<Transaction> transaction,std::vector<StateVariable> prevState={},bool trace=false);
    Contract(std::string script,const std::string& signatures,std::shared_ptr<Witness> witness,std::shared_ptr<Transaction> transaction,std::vector<StateVariable> prevState={},bool trace=false);
    void setGlobals(const MiniNumber& block,const MiniNumber& blockTimeMilli,const Transaction& txn,int input,const MiniNumber& created,const std::string& script);
    void setGlobalVariable(const std::string& name,ValuePtr value);
    ValuePtr getGlobal(const std::string& name);
    std::shared_ptr<JavaHashtable<ValuePtr>> getGlobalVariables() const {return mGlobals;}
    void setAllGlobalVariables(std::shared_ptr<JavaHashtable<ValuePtr>> globals) {mGlobals=std::move(globals);}
    bool isParseOK() const {return mParseOK;}
    bool isException() const {return mException;}
    const std::string& getException() const {return mExceptionString;}
    bool isTrace() const {return mTraceON;}
    void traceLog(const std::string& log);
    const std::string& getCompleteTraceLog() const {return mCompleteLog;}
    void incrementInstructions(int number=1);
    int getNumberOfInstructions() const {return mNumInstructions;}
    void setMaxInstructions(int maximum) {MAX_INSTRUCTIONS=maximum;}
    void resetStackDepth() {mStackDepth=0;}
    int getStackDepth() const {return mStackDepth;}
    void incrementStackDepth();
    void decrementStackDepth();
    void run();
    void setRETURNValue(bool success) {if(!mSuccessSet) {mSuccess=success;mSuccessSet=true;}}
    bool isSuccess() const {return mSuccess;}
    bool isSuccessSet() const {return mSuccessSet;}
    bool isMonotonic() const {return mMonotonic;}
    const std::string& getMiniScript() const {return mRamScript;}
    Transaction& getTransaction() {return *mTransaction;}
    Witness& getWitness() {return *mWitness;}
    json::JSONObject getAllVariables() const;
    void removeVariable(const std::string& name) {mVariables.remove(name);}
    bool existsVariable(const std::string& name) const {return mVariables.get(name)!=nullptr;}
    ValuePtr getVariable(const std::string& name) const {auto value=mVariables.get(name);return value?*value:nullptr;}
    void setVariable(const std::string& name,ValuePtr value);
    NumberValue getNumberParam(int number,MinimaFunction& function);
    HexValue getHexParam(int number,MinimaFunction& function);
    StringValue getStringParam(int number,MinimaFunction& function);
    BooleanValue getBoolParam(int number,MinimaFunction& function);
    ValuePtr getState(int port) const;
    ValuePtr getPrevState(int port) const;
    void traceVariables();
    bool checkSignature(const HexValue& signature) const;
    static std::string cleanScript(const std::string& script,bool log=false);
};
// Preserve Java exception names for contract diagnostics. Native library errors
// remain explicit until their individual Java exception types are translated.
std::string exceptionString(const std::exception& error);
}
