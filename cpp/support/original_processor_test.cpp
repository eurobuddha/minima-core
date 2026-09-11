#include "../test/org/minima/system/brains/TxPoWProcessorTest.hpp"
#include "support/JavaExceptions.hpp"
#include <chrono>
#include <iostream>
#include <limits>
#include <vector>
using namespace minima;
using minima::testing::TxPoWProcessorTest;
int main() {
    TxPoWProcessorTest{}.testTxPoWProcessor();TxPoWProcessorTest{}.testTxPoWProcessor2();
    std::cout<<"original_methods=2;original_assertions=0\n";
    const auto start=std::chrono::steady_clock::now();TxPoWProcessorTest::delay();
    if(std::chrono::steady_clock::now()-start<std::chrono::milliseconds(90))throw std::runtime_error("Default delay returned too early");
    std::cout<<"default=100ms\n";
    for(int64_t value:std::vector<int64_t>{0,1,-1,-100,std::numeric_limits<int64_t>::min()}) {
        std::string result="returned";
        try {TxPoWProcessorTest::delay(value);}catch(const JavaIllegalArgumentException& e){result=javaExceptionText(e);}
        std::cout<<"delay("<<value<<")="<<result<<'\n';
    }
    // Supply the runtime interrupt boundary, as existing MainServices fixtures do.
    // These are adapter checks, not proof of a native Thread.interrupt facility.
    bool interrupted=false;
    const auto original=TxPoWProcessorTest::sleep;
    TxPoWProcessorTest::sleep=[&](int64_t millis) {
        if(millis<0)throw JavaIllegalArgumentException("timeout value is negative");
        if(interrupted){interrupted=false;throw JavaTypedException("java.lang.InterruptedException","sleep interrupted");}
        original(millis);
    };
    for(int64_t value:std::vector<int64_t>{0,1,std::numeric_limits<int64_t>::max(),-1}) {
        interrupted=true;std::string result="returned";
        try {TxPoWProcessorTest::delay(value);}catch(const JavaIllegalArgumentException& e){result=javaExceptionText(e);}
        std::cout<<"interrupted("<<value<<")="<<result<<";status="<<(interrupted?"true":"false")<<'\n';interrupted=false;
    }
    TxPoWProcessorTest::sleep=[](int64_t){throw JavaTypedException("fixture.UnrelatedException","must escape");};
    bool escaped=false;try{TxPoWProcessorTest::delay();}catch(const JavaTypedException& e){escaped=e.className()=="fixture.UnrelatedException";}
    if(!escaped)throw std::runtime_error("Delay swallowed an unrelated exception");
    TxPoWProcessorTest::sleep=original;
}
