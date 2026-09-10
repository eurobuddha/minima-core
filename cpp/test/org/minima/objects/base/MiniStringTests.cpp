// Port of the pinned MiniStringTests, including its unannotated helper.
#include "org/minima/objects/base/MiniString.hpp"
#include "support/ByteIO.hpp"
#include <memory>
#include <stdexcept>
#include <string>
using namespace minima;
namespace {
int assertions=0;
void require(bool condition){++assertions;if(!condition)throw std::runtime_error("MiniStringTests assertion");}
void testValueString(){
    auto i=std::make_shared<MiniString>("MinimaTest");
    auto j=std::make_shared<MiniString>(*i);
    require(bool(i));require(bool(j));
    // Native string/vector results cannot be null. Preserve these source smoke
    // checks by materializing their returned objects; do not substitute equality.
    auto text=std::make_unique<std::string>(i->toString());require(bool(text));
    auto bytes=std::make_unique<std::vector<uint8_t>>(i->getData());require(bool(bytes));
}
void testReadFromStream(){
    try{
        auto i=std::make_shared<MiniString>("test");auto bytes=i->serialise();size_t offset=0;
        // Java invokes this static method through i and discards the result.
        (void)MiniString::deserialise(bytes,offset);require(bool(i));
    }catch(const wire::IOException& e){require(std::string(e.what()).find("input too large")!=std::string::npos);}
}
void testWriteDataStreamString(){
    try{auto i=std::make_shared<MiniString>("t");(void)i->serialise();require(bool(i));}
    catch(const wire::IOException& e){require(std::string(e.what()).find("input too large")!=std::string::npos);}
}
}
int main(int argc,char** argv){
    if(argc>1){if(std::string(argv[1])!="--unannotated-helper")throw std::invalid_argument("mode");testWriteDataStreamString();if(assertions!=1)throw std::runtime_error("helper count");}
    else{testValueString();testReadFromStream();if(assertions!=5)throw std::runtime_error("JUnit assertion count");}
}
