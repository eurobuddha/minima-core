// Active assertions from pinned NumberValueTests. Java's case 3 is absent:
// each comparison loop stops there, so written cases 4-6 never execute.
#include "org/minima/kissvm/values/NumberValue.hpp"
#include "support/JavaExceptions.hpp"
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static int assertions=0;
static void require(bool ok){++assertions;if(!ok)throw std::runtime_error("NumberValueTests assertion "+std::to_string(assertions));}
template<class F>static void arithmeticError(F operation){bool caught=false;try{operation();}catch(const JavaArithmeticException&){caught=true;}require(caught);}
static constexpr int64_t imin=std::numeric_limits<int32_t>::min(),imax=std::numeric_limits<int32_t>::max();
static constexpr int64_t lmin=std::numeric_limits<int64_t>::min(),lmax=std::numeric_limits<int64_t>::max();
static void constructors(){
    int32_t i=0x00FFFFFF;int64_t l=0x00FFFFFFFFFFFFFFLL;MiniNumber m(int64_t(i)+64);
    NumberValue a(i),at(std::to_string(i)),b(l),bt(std::to_string(l)),c(m),ct(m.toString());
    require(a.getNumber().getAsInt()==at.getNumber().getAsInt());
    require(b.getNumber().getAsLong()==bt.getNumber().getAsLong());
    require(c.getNumber().getAsBigInteger()==ct.getNumber().getAsBigInteger());
    for(const auto* value:{&a,&at,&b,&bt})require(value->getValueType()==NumberValue::VALUE_NUMBER);
}
static void arithmetic(){
    NumberValue a(0),b(0),expected(0);
    require(a.add(b).isEqual(expected));require(b.add(a).isEqual(expected));
    a=NumberValue(123456789);b=NumberValue(76543211);expected=NumberValue(200000000);
    require(a.add(b).isEqual(expected));require(b.add(a).isEqual(expected));
    a=NumberValue(imax);b=NumberValue(imin);expected=NumberValue(-1);
    require(a.add(b).isEqual(expected));require(b.add(a).isEqual(expected));
    a=NumberValue(imax);b=NumberValue(1);expected=NumberValue(1LL+imax);
    require(a.add(b).isEqual(expected));require(b.add(a).isEqual(expected));
    a=NumberValue(lmax);b=NumberValue(lmin);expected=NumberValue(-1);
    require(a.add(b).isEqual(expected));require(b.add(a).isEqual(expected));
    a=NumberValue(lmax);b=NumberValue(1);expected=NumberValue(MiniNumber(1).add(MiniNumber(lmax)));
    require(!expected.getNumber().isEqual(MiniNumber(lmax)));
    require(a.add(b).isEqual(expected));require(b.add(a).isEqual(expected));

    a=NumberValue(0);b=NumberValue(0);expected=NumberValue(0);
    require(a.sub(b).isEqual(expected));
    a=NumberValue(123456789);b=NumberValue(23456789);expected=NumberValue(100000000);
    require(a.sub(b).isEqual(expected));require(!b.sub(a).isEqual(expected));
    a=NumberValue(imin);b=NumberValue(1);expected=NumberValue(imin-1LL);
    require(a.sub(b).isEqual(expected));require(!b.sub(a).isEqual(expected));
    a=NumberValue(imin);b=NumberValue(imax);expected=NumberValue(imin-imax);
    require(a.sub(b).isEqual(expected));require(!b.sub(a).isEqual(expected));
    a=NumberValue(imax);b=NumberValue(imin);expected=NumberValue(imax-imin);
    require(a.sub(b).isEqual(expected));require(!b.sub(a).isEqual(expected));

    a=NumberValue(0);b=NumberValue(0);expected=NumberValue(0);
    require(a.mult(b).isEqual(expected));
    a=NumberValue(12345);b=NumberValue(67890);expected=NumberValue(838102050);
    require(a.mult(b).isEqual(expected));require(b.mult(a).isEqual(expected));
    a=NumberValue(1000000);b=NumberValue(1000000);expected=NumberValue(1000000LL*1000000LL);
    require(a.mult(b).isEqual(expected));require(b.mult(a).isEqual(expected));
    for(int64_t boundary:{imin,imax,lmin,lmax}){
        a=NumberValue(boundary);b=NumberValue(1);expected=NumberValue(boundary);
        require(a.mult(b).isEqual(expected));require(b.mult(a).isEqual(expected));
    }
    arithmeticError([]{NumberValue first(0),second(0);first.div(second);});
    arithmeticError([]{NumberValue first(0),second(0);second.div(first);});
    a=NumberValue(0);b=NumberValue(1);expected=NumberValue(0);require(a.div(b).isEqual(expected));
}
static void ordered(const NumberValue& a,const NumberValue& b){
    require(a.isLess(b));require(!b.isLess(a));require(a.isLessEqual(b));require(!b.isLessEqual(a));
    require(!a.isMore(b));require(b.isMore(a));require(!a.isMoreEqual(b));require(b.isMoreEqual(a));
}
static void comparisons(){
    // Both Java int and long routes use the native int64_t constructor.
    // Preserve two executions; do not activate Java's unreachable routes.
    for(int route=1;route<=2;++route){
        NumberValue a(0),b(0);
        require(!a.isLess(b));require(!b.isLess(a));require(a.isLessEqual(b));require(b.isLessEqual(a));
        require(!a.isMore(b));require(!b.isMore(a));require(a.isMoreEqual(b));require(b.isMoreEqual(a));
    }
    for(int route=1;route<=2;++route)ordered(NumberValue(5),NumberValue(10));
    ordered(NumberValue(imin),NumberValue(imax));ordered(NumberValue(lmin),NumberValue(lmax));
}
static void strings(){
    require(NumberValue(int32_t(0)).toString()=="0");require(NumberValue(int64_t(0)).toString()=="0");
    require(NumberValue(int32_t(1)).toString()=="1");require(NumberValue(int64_t(1)).toString()=="1");
    require(NumberValue(imin).toString()==std::to_string(imin));require(NumberValue(imax).toString()==std::to_string(imax));
}
// Extra production error observations, separate from the 99 pinned assertions.
// Compared byte-for-byte with NumberValueOriginalTests' Java observations.
template<class F>static void errorObservation(F operation){
    try{operation();}catch(const JavaArithmeticException& e){std::cout<<javaExceptionText(e)<<'\n';return;}
    throw std::runtime_error("Expected supplemental arithmetic error");
}
int main(){
    constructors();arithmetic();comparisons();strings();
    if(assertions!=99)throw std::runtime_error("Mapped assertion count changed: "+std::to_string(assertions));
    std::cout<<"Original NumberValueTests methods passed: 4 (99 executed assertions)\n";
    for(const std::string a:{"0","1","-1"})for(const std::string b:{"0","0.000"})
        errorObservation([&]{return NumberValue(a).div(NumberValue(b));});
    errorObservation([]{return MiniNumber(0).pow(-1);});
    for(int exponent:{-1000000000,std::numeric_limits<int>::min(),1000000000,std::numeric_limits<int>::max()})
        errorObservation([&]{return MiniNumber::TEN.pow(exponent);});
}
