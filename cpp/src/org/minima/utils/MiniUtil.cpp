#include "MiniUtil.hpp"
#include "support/JavaUnicode.hpp"
#include <unicode/udat.h>
#include <memory>
#include <ctime>
#include <sstream>
#include <iomanip>
namespace minima {
const MiniUtil::DateFormat MiniUtil::DATEFORMAT;
std::string MiniUtil::DateFormat::format(int64_t millis) const {
    if(millis<-(int64_t(1)<<53)||millis>(int64_t(1)<<53)) {
        auto floorDiv=[](int64_t a,int64_t b){auto q=a/b;return a%b<0?q-1:q;};
        auto seconds=floorDiv(millis,1000);std::time_t raw=seconds;std::tm local{};
        if(!localtime_r(&raw,&local))throw std::out_of_range("Unrepresentable date");
        int64_t year=int64_t(local.tm_year)+1900,month=local.tm_mon+1,day=local.tm_mday;
        if(millis<0){auto jdn=floorDiv(seconds+local.tm_gmtoff,86400)+2440588;auto c=jdn+32082;auto d=floorDiv(4*c+3,1461);auto e=c-floorDiv(1461*d,4);auto m=floorDiv(5*e+2,153);day=e-floorDiv(153*m+2,5)+1;month=m+3-12*floorDiv(m,10);year=d-4800+floorDiv(m,10);}
        if(year<=0)year=1-year;std::ostringstream result;result.imbue(std::locale::classic());result<<std::setfill('0')<<std::setw(2)<<day<<'_'<<std::setw(2)<<month<<'_'<<std::setw(4)<<year<<'_'<<std::setw(2)<<local.tm_hour<<std::setw(2)<<local.tm_min<<std::setw(2)<<local.tm_sec;return result.str();
    }
    UErrorCode error=U_ZERO_ERROR;const std::u16string pattern=u"dd_MM_yyyy_HHmmss";
    std::unique_ptr<UDateFormat,decltype(&udat_close)> format(udat_open(UDAT_PATTERN,UDAT_PATTERN,"en",nullptr,0,reinterpret_cast<const UChar*>(pattern.data()),int32_t(pattern.size()),&error),udat_close);
    if(U_FAILURE(error)||!format)throw std::runtime_error(u_errorName(error));
    auto length=udat_format(format.get(),double(millis),nullptr,0,nullptr,&error);
    if(error!=U_BUFFER_OVERFLOW_ERROR&&U_FAILURE(error))throw std::runtime_error(u_errorName(error));error=U_ZERO_ERROR;
    std::u16string output(size_t(length),u'\0');udat_format(format.get(),double(millis),reinterpret_cast<UChar*>(output.data()),length,nullptr,&error);
    if(U_FAILURE(error))throw std::runtime_error(u_errorName(error));return unicode::encodeUTF8(output);
}
std::shared_ptr<json::JSONArray> MiniUtil::convertArrayList(const StringList& strings) {auto array=std::make_shared<json::JSONArray>();for(const auto& s:strings)array->add(s?json::JSONValue(*s):json::JSONValue(nullptr));return array;}
MiniUtil::StringList MiniUtil::convertJSONArray(const std::shared_ptr<json::JSONArray>& array) {if(!array)throw std::invalid_argument("Null array");StringList result;for(const auto& value:array->elements())result.push_back(value.isNull()?std::nullopt:std::optional(value.asString()));return result;}
std::shared_ptr<json::JSONArray> MiniUtil::convertUTF16ArrayList(const UTF16StringList& strings) {auto array=std::make_shared<json::JSONArray>();for(const auto& s:strings)array->add(s?json::JSONValue(*s):json::JSONValue(nullptr));return array;}
MiniUtil::UTF16StringList MiniUtil::convertUTF16JSONArray(const std::shared_ptr<json::JSONArray>& array) {if(!array)throw std::invalid_argument("Null array");UTF16StringList result;for(const auto& value:array->elements())result.push_back(value.isNull()?std::nullopt:std::optional(value.asUTF16()));return result;}
}
