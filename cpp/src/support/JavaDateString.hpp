#pragma once
#include "JavaCalendarFields.hpp"
namespace minima {
inline std::string javaDateString(int64_t millis) {
    auto fields=javaCalendarFields(millis);auto& calendar=fields.calendar;
    const auto& local=fields.local;auto year=fields.year,month=fields.month,day=fields.day,offset=fields.offset;
    auto dstOffset=fields.dstOffset;UErrorCode error=U_ZERO_ERROR;
    auto type=dstOffset!=0?UCAL_SHORT_DST:UCAL_SHORT_STANDARD;
    // TimeZone.getDisplayName is independent of the represented Date. ICU's
    // calendar display-name lookup otherwise loses metazone names far ahead.
    ucal_setMillis(calendar.get(),double(std::time(nullptr))*1000,&error);
    int32_t length=ucal_getTimeZoneDisplayName(calendar.get(),type,"en_US",nullptr,0,&error);
    if(error!=U_BUFFER_OVERFLOW_ERROR&&U_FAILURE(error))throw std::runtime_error(u_errorName(error));error=U_ZERO_ERROR;
    std::u16string zone(size_t(length),u'\0');ucal_getTimeZoneDisplayName(calendar.get(),type,"en_US",reinterpret_cast<UChar*>(zone.data()),length,&error);if(U_FAILURE(error))throw std::runtime_error(u_errorName(error));
    auto zoneName=unicode::encodeUTF8(zone);
    if(zoneName.starts_with("GMT+")||zoneName.starts_with("GMT-")){
        // Some ICU locales omit a conventional short name (for example JST).
        // Reuse the host abbreviation only when its seasonal offset matches.
        std::time_t now=std::time(nullptr);std::tm sample{};gmtime_r(&now,&sample);
        for(int sampleMonth:{0,6}){sample.tm_mon=sampleMonth;sample.tm_mday=15;sample.tm_hour=12;sample.tm_min=sample.tm_sec=0;auto sampleTime=timegm(&sample);std::tm seasonal{};if(localtime_r(&sampleTime,&seasonal)&&seasonal.tm_gmtoff==offset&&seasonal.tm_zone){std::string candidate=seasonal.tm_zone;if(!candidate.empty()&&std::all_of(candidate.begin(),candidate.end(),[](char c){return (c>='A'&&c<='Z')||(c>='a'&&c<='z');})){zoneName=candidate;break;}}}
    }
    static const char* days[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};static const char* months[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    std::ostringstream out;out.imbue(std::locale::classic());out<<days[local.tm_wday]<<' '<<months[month-1]<<' '<<std::setfill('0')<<std::setw(2)<<day<<' '<<std::setw(2)<<local.tm_hour<<':'<<std::setw(2)<<local.tm_min<<':'<<std::setw(2)<<local.tm_sec<<' '<<zoneName<<' '<<year;return out.str();
}
}
