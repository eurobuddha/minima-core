#include "support/NativeThrowable.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include <iostream>
#include <sstream>

using namespace minima;
extern "C" [[gnu::noinline]] void minima_test_throw_origin(int depth) {
    if(depth==0)throw wire::IOException(std::string("origin\0tail",11));
    minima_test_throw_origin(depth-1);
    // Make the caller frame observable even under optimized builds.
    asm volatile("" : : "r"(depth) : "memory");
}
int main() {
    try {
        try {minima_test_throw_origin(160);}
        catch(const wire::IOException& original) {
            if(original.nativeFrameAddresses().size()<160)throw std::runtime_error("Native stack was truncated");
            auto copy=original;
            if(copy.nativeFrameAddresses()!=original.nativeFrameAddresses())throw std::runtime_error("Copy recaptured stack");
            try {throw copy;}catch(const wire::IOException& caught) {
                if(caught.nativeFrameAddresses()!=original.nativeFrameAddresses())throw std::runtime_error("Throw recaptured stack");
                NativeThrowable snapshot(caught);std::ostringstream printed;snapshot.printStackTrace(printed);
                if(printed.str().find(std::string("origin\0tail",11))==std::string::npos)throw std::runtime_error("NUL message lost");
                if(printed.str().find("minima_test_throw_origin")==std::string::npos)throw std::runtime_error("Original throwing function absent");
            }
        }
        try {json::parser::JSONParser().parse(std::string("{"));}
        catch(const json::parser::ParseException& error) {
            auto frames=error.nativeStackTrace();if(frames.empty())throw std::runtime_error("Parser stack absent");
            auto copy=error;if(copy.nativeFrameAddresses()!=error.nativeFrameAddresses())throw std::runtime_error("Parser copy recaptured stack");
        }
        JavaNullPointerException null("null");JavaArrayIndexOutOfBoundsException bounds("bounds");JavaIllegalArgumentException argument("arg");
        if(null.nativeStackTrace().empty()||bounds.nativeStackTrace().empty()||argument.nativeStackTrace().empty())throw std::runtime_error("Typed exception stack absent");
        std::cout<<"Native construction-site stacks survive deep calls, copies and throws; parser and typed exceptions capture frames.\n";
        return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
