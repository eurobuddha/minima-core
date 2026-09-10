#pragma once
#include <unicode/uchar.h>
#include <unicode/uregex.h>
#include <unicode/utf16.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
namespace minima::unicode {
// Java lowercasing uses its legacy word/number grammar for final sigma,
// rather than Unicode's Case_Ignorable test. ICU provides property matching;
// this adapter applies the inspected JDK word-boundary rules first.
inline void applyJavaFinalSigma(std::u16string& text) {
    if(text.find(u'\u03a3')==std::u16string::npos)return;
    std::u16string filtered;std::vector<size_t> positions;
    for(int32_t i=0;i<int32_t(text.size());){int32_t start=i;UChar32 cp;U16_NEXT(text.data(),i,int32_t(text.size()),cp);if(u_charType(cp)!=U_FORMAT_CHAR)for(int32_t j=start;j<i;++j){filtered+=text[size_t(j)];positions.push_back(size_t(j));}}
    const std::u16string enclosing=uR"([\p{Mn}\p{Me}]*)";
    const std::u16string letter=uR"([[\p{L}\p{Mc}]-[\u3005\u4e00-\u9fa5\uf900-\ufa2d\u30a1-\u30fa\u30fd\u30fe\u3041-\u3094\u309d\u309e\u3099-\u309c\u30fb\u30fc]])"+enclosing;
    const auto digit=std::u16string(uR"([\p{N}])")+enclosing;
    const auto word=u"(?:(?:"+letter+u")+(?:[\\p{Pd}\\p{Pc}\\u00ad\\u2027\"'.](?:"+letter+u")+)*[\\u0964\\u0965]?)";
    const auto number=u"(?:(?:"+digit+u")+(?:[\"',\\u066b.](?:"+digit+u")+)*)";
    const std::u16string post=uR"([%&\u00a2\u066a\u2030\u2031]?)";
    const auto tail=u"(?:"+number+word+u")*(?:"+number+post+u")?";
    std::vector<std::u16string> patterns={u"(?:"+word+u")?"+tail,uR"([[\p{Sc}#.]-[\u00a2]])"+tail};
    using Regex=std::unique_ptr<URegularExpression,decltype(&uregex_close)>;std::vector<Regex> regexes;
    for(const auto& pattern:patterns){UErrorCode error=U_ZERO_ERROR;Regex regex(uregex_open(reinterpret_cast<const UChar*>(pattern.data()),int32_t(pattern.size()),0,nullptr,&error),uregex_close);if(U_FAILURE(error)||!regex)throw std::runtime_error(u_errorName(error));uregex_setText(regex.get(),reinterpret_cast<const UChar*>(filtered.data()),int32_t(filtered.size()),&error);if(U_FAILURE(error))throw std::runtime_error(u_errorName(error));regexes.push_back(std::move(regex));}
    auto cased=[](UChar32 cp){auto type=u_charType(cp);return type==U_LOWERCASE_LETTER||type==U_UPPERCASE_LETTER||type==U_TITLECASE_LETTER||(cp>=0x02b0&&cp<=0x02b8)||(cp>=0x02c0&&cp<=0x02c1)||(cp>=0x02e0&&cp<=0x02e4)||cp==0x0345||cp==0x037a||(cp>=0x1d2c&&cp<=0x1d61)||(cp>=0x2160&&cp<=0x217f)||(cp>=0x24b6&&cp<=0x24e9);};
    for(int32_t start=0;start<int32_t(filtered.size());){
        int32_t end=start;UChar32 first;U16_NEXT(filtered.data(),end,int32_t(filtered.size()),first);
        for(auto& regex:regexes){UErrorCode error=U_ZERO_ERROR;if(uregex_lookingAt(regex.get(),start,&error))end=std::max(end,uregex_end(regex.get(),0,&error));if(U_FAILURE(error))throw std::runtime_error(u_errorName(error));}
        int total=0;for(int32_t i=start;i<end;){UChar32 cp;U16_NEXT(filtered.data(),i,end,cp);if(cased(cp))++total;}
        int before=0;for(int32_t i=start;i<end;){int32_t index=i;UChar32 cp;U16_NEXT(filtered.data(),i,end,cp);if(cp==0x03a3)text[positions[size_t(index)]]=before>0&&before+1==total?u'\u03c2':u'\u03c3';if(cased(cp))++before;}
        start=end;
    }
}
}
