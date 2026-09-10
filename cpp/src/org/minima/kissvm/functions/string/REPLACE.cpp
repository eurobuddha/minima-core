#include "REPLACE.hpp"
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
ValuePtr REPLACE::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto main=contract.getStringParam(0,*this);auto search=contract.getStringParam(1,*this);auto replacement=contract.getStringParam(2,*this);
    return std::make_shared<StringValue>(REPLACE::safeReplaceAll(main.getUTF16(),search.getUTF16(),replacement.getUTF16()));
}

namespace {
// A quoted search pattern has no capturing groups. Java still interprets the
// replacement as Matcher syntax: backslash escapes and group zero are allowed.
std::u16string expandReplacement(const std::u16string& replacement,const std::u16string& matched) {
    std::u16string result;
    for(size_t i=0;i<replacement.size();++i) {
        char16_t c=replacement[i];
        if(c==u'\\') {if(++i==replacement.size()) throw std::invalid_argument("character to be escaped is missing");result+=replacement[i];}
        else if(c==u'$') {
            if(++i==replacement.size()) throw std::invalid_argument("Illegal group reference: group index is missing");
            if(replacement[i]!=u'0') throw std::invalid_argument("Invalid replacement group reference");
            while(i+1<replacement.size()&&replacement[i+1]==u'0') ++i;result+=matched;
        } else result+=c;
    }return result;
}
std::u16string replaceQuoted(const std::u16string& original,const std::u16string& search,const std::u16string& replacement,bool first) {
    std::u16string result;size_t copied=0,position=0;
    while(position<=original.size()) {
        auto found=original.find(search,position);if(found==std::u16string::npos) break;
        result+=original.substr(copied,found-copied);result+=expandReplacement(replacement,search);copied=found+search.size();
        if(!first&&result.size()>Contract::MAX_DATA_SIZE) throw ExecutionException("Replace String too long! "+std::to_string(result.size()));
        if(first) break;position=search.empty()?found+1:copied;
    }
    result+=original.substr(copied);if(!first&&result.size()>Contract::MAX_DATA_SIZE) throw ExecutionException("Replace String too long! "+std::to_string(result.size()));return result;
}
}
std::u16string REPLACE::safeReplaceAll(const std::u16string& original,const std::u16string& search,const std::u16string& replacement) {return replaceQuoted(original,search,replacement,false);}
std::string REPLACE::safeReplaceAll(const std::string& original,const std::string& search,const std::string& replacement) {return unicode::encodeUTF8(safeReplaceAll(unicode::decodeUTF8(original),unicode::decodeUTF8(search),unicode::decodeUTF8(replacement)));}
std::u16string REPLACE::replaceFirst(const std::u16string& original,const std::u16string& search,const std::u16string& replacement) {return replaceQuoted(original,search,replacement,true);}

}
