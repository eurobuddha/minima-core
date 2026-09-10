#include "FUNCTION.hpp"
#include "../../Contract.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/objects/mmr/MMRProof.hpp"
#include "../../statements/StatementParser.hpp"
#include "../../tokens/ScriptTokenizer.hpp"
#include "../string/REPLACE.hpp"
#include "support/JavaUnicode.hpp"
#include <bit>
#include <algorithm>

namespace minima::kissvm {
ValuePtr FUNCTION::runFunction(Contract& contract) {
    checkMinParamNumber(requiredParams());
    auto script=contract.getStringParam(0,*this).getUTF16();
    auto check=[](const std::u16string& text) {int count=0;bool in=false;for(auto c:text) {if(c==u'$') in=false;else if(!in) {++count;in=true;}}if(count-1>64) throw ExecutionException("Too many replacements in FUNCTION, max 64");};check(script);
    for(int i=1;i<getParameterNum();++i) {auto value=getParameter(i)->getValue(contract);auto replacement=value->getValueType()==Value::VALUE_SCRIPT?u"["+static_cast<StringValue&>(*value).getUTF16()+u"]":unicode::decodeUTF8(value->toString());script=REPLACE::safeReplaceAll(script,unicode::decodeUTF8("$"+std::to_string(i)),replacement);check(script);}
    contract.removeVariable(FUNCTION_RETURN);
    try {auto tokens=ScriptTokenizer(unicode::encodeUTF8(script)).tokenize();auto block=StatementParser::parseTokens(tokens,contract.getStackDepth());block->run(contract);}
    catch(const ExecutionException&) {throw;}catch(const std::exception& error) {throw ExecutionException(exceptionString(error));}
    if(contract.existsVariable(FUNCTION_RETURN)) return contract.getVariable(FUNCTION_RETURN);return std::make_shared<BooleanValue>(true);
}

}
