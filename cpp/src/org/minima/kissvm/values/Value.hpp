#pragma once
#include <memory>
#include <string>
namespace minima::kissvm {
class Value {
public:
    static constexpr int VALUE_HEX=1,VALUE_NUMBER=2,VALUE_SCRIPT=4,VALUE_BOOLEAN=8;
    virtual ~Value();
    virtual int getValueType() const=0;
    virtual std::string toString() const=0;
    void verifyType(int type) const;
    static std::shared_ptr<Value> getValue(const std::string& text);
    static int getValueType(const std::string& text);
    static std::string getValueTypeString(int type);
    static void checkSameType(const Value& a,const Value& b);
    static void checkSameType(const Value& a,const Value& b,int type);
};
}
