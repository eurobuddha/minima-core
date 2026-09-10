#include "Maths.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/JavaInteger.hpp"
#include "support/fdlibm/fdlibm.hpp"
#include <bit>
#include <cmath>
namespace minima {
const BigInt Maths::BI_TWO(2);
const MiniNumber Maths::BD_TWO(2);
double Maths::log2(double value) {return fdlibm::log10(value)/fdlibm::log10(2.0);}
double Maths::log2BI(const BigInt& value) {
    const auto& bytes=value.toJavaBytes();
    size_t first=0;
    const bool negative=value.isNegative();
    auto magnitude=[&](size_t index){return unsigned(negative?uint8_t(~bytes[index]):bytes[index]);};
    while(first<bytes.size()&&magnitude(first)==0) ++first;
    const int n=first==bytes.size()?0:int((bytes.size()-first-1)*8+std::bit_width(magnitude(first)));
    auto bit=[&](int index){size_t offset=size_t(index)/8;return offset<bytes.size()?(bytes[bytes.size()-1-offset]&(1u<<(index%8)))!=0:negative;};
    uint64_t mask=uint64_t(1)<<52,mantissa=0;
    int j=0;
    for(int i=1;i<54;++i) {j=n-i;if(j<0) break;if(bit(j)) mantissa|=mask;mask>>=1;}
    if(j>0&&bit(j-1)) ++mantissa;
    const double fraction=double(mantissa)/double(uint64_t(1)<<52);
    // Java evaluates multiply and add separately, including in optimized builds.
    volatile double logarithm=fdlibm::log(fraction)*1.44269504088896340735992468100189213742664595415298;
    return double(n-1)+logarithm;
}
int Maths::getSuperLevel(const MiniData& difficulty,const MiniData& actual) {
    double logarithm=log2BI(difficulty.getDataValue()/actual.getDataValue());
    int level=std::isnan(logarithm)?0:logarithm>=INT32_MAX?INT32_MAX:logarithm<=INT32_MIN?INT32_MIN:int(logarithm);
    if(level<0) level=0;
    const auto maximum=std::bit_cast<int32_t>(uint32_t(GlobalParams::MINIMA_CASCADE_LEVELS)-1u);
    if(level>maximum) level=maximum;
    return level;
}
int Maths::compareVersions(const std::string& first,const std::string& second) {
    auto split=[](const std::string& value){
        std::vector<std::string> levels;size_t start=0;
        while(true) {auto end=value.find('.',start);if(end==std::string::npos){levels.push_back(value.substr(start));break;}levels.push_back(value.substr(start,end-start));start=end+1;}
        if(!value.empty()) while(!levels.empty()&&levels.back().empty()) levels.pop_back();
        return levels;
    };
    auto left=split(first),right=split(second);
    for(size_t i=0;i<std::max(left.size(),right.size());++i) {
        int a=i<left.size()?javaParseInt(left[i]):0,b=i<right.size()?javaParseInt(right[i]):0;
        if(a!=b) return a<b?-1:1;
    }
    return 0;
}
}
