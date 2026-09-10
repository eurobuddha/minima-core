#pragma once
#include "JavaUnicode.hpp"
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <unicode/ucal.h>
namespace minima {
struct JavaCalendarFields {
    std::unique_ptr<UCalendar,decltype(&ucal_close)> calendar;
    std::tm local;
    int64_t year,month,day,offset;
    int32_t rawOffset,dstOffset;
};
inline JavaCalendarFields javaCalendarFields(int64_t millis,const std::u16string* zone=nullptr) {
    auto floorDiv=[](int64_t a,int64_t b){auto q=a/b;return a%b<0?q-1:q;};
    auto seconds=floorDiv(millis,1000);UErrorCode error=U_ZERO_ERROR;
    std::unique_ptr<UCalendar,decltype(&ucal_close)> calendar(ucal_open(zone?reinterpret_cast<const UChar*>(zone->data()):nullptr,zone?int32_t(zone->size()):0,"en_US",UCAL_GREGORIAN,&error),ucal_close);
    if(U_FAILURE(error)||!calendar)throw std::runtime_error(u_errorName(error));
    // ZoneInfo's transition table starts at 1900. Earlier Date instances use
    // the zone's final standard offset, not the host tzdb's local mean time.
    int32_t rawOffset=ucal_get(calendar.get(),UCAL_ZONE_OFFSET,&error),dstOffset=0;
    if(millis>=-2208988800000LL){
        double offsetDate=double(millis);
        if(millis>(int64_t(1)<<53)){
            // Preserve Gregorian recurrence without losing day/second precision
            // through ICU's floating-point date representation at extreme years.
            std::time_t timestamp=seconds;std::tm utc{};if(!gmtime_r(&timestamp,&utc))throw std::out_of_range("Unrepresentable UTC date");
            int64_t year=int64_t(utc.tm_year)+1900;utc.tm_year=int(2400+(year%400))-1900;offsetDate=double(timegm(&utc))*1000;
        }
        ucal_setMillis(calendar.get(),offsetDate,&error);rawOffset=ucal_get(calendar.get(),UCAL_ZONE_OFFSET,&error);dstOffset=ucal_get(calendar.get(),UCAL_DST_OFFSET,&error);
        // SimpleTimeZone computes transition milliseconds with signed long
        // arithmetic. If a future DST end overflows, the period is not active.
        if(dstOffset>0&&millis>INT64_MAX-370LL*86400000){
            std::time_t timestamp=seconds;std::tm probe{};
            for(int day=1;day<=370;++day){timestamp=seconds+int64_t(day)*86400;if(!localtime_r(&timestamp,&probe))break;if(int64_t(probe.tm_gmtoff)*1000==rawOffset){int64_t low=timestamp-86400,high=timestamp;while(high-low>1){std::time_t middle=low+(high-low)/2;if(!localtime_r(&middle,&probe))throw std::out_of_range("Unrepresentable transition");if(int64_t(probe.tm_gmtoff)*1000==rawOffset)high=middle;else low=middle;}if(high>INT64_MAX/1000)dstOffset=0;break;}}
        }
    }
    if(U_FAILURE(error))throw std::runtime_error(u_errorName(error));
    int64_t offset=(int64_t(rawOffset)+dstOffset)/1000;
    std::time_t localSeconds=seconds+offset;std::tm local{};if(!gmtime_r(&localSeconds,&local))throw std::out_of_range("Unrepresentable local date");
    int64_t year=int64_t(local.tm_year)+1900,month=local.tm_mon+1,day=local.tm_mday;
    if(millis<-12219292800000LL-offset*1000){auto jdn=floorDiv(seconds+offset,86400)+2440588;auto c=jdn+32082;auto d=floorDiv(4*c+3,1461);auto e=c-floorDiv(1461*d,4);auto m=floorDiv(5*e+2,153);day=e-floorDiv(153*m+2,5)+1;month=m+3-12*floorDiv(m,10);year=d-4800+floorDiv(m,10);if(year<=0)year=1-year;}
    return {std::move(calendar),local,year,month,day,offset,rawOffset,dstOffset};
}
}
