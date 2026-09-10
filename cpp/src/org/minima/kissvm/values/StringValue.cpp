#include "StringValue.hpp"
#include "support/KissLimits.hpp"
#include "support/JavaUnicode.hpp"
#include <stdexcept>
namespace minima::kissvm {
StringValue::StringValue(const std::string& script):StringValue(unicode::decodeUTF8(script)){}
// MiniString(String) round-trips through UTF-8 even before serialization.
// An isolated surrogate therefore becomes '?' when the value is constructed.
StringValue::StringValue(const std::u16string& script):mScript(unicode::encodeUTF8(script)),mUTF16(unicode::decodeUTF8(mScript.toString())) {auto length=getBytes().size();if(length>limits::MAX_DATA_SIZE) throw std::invalid_argument("MAX String length reached : "+std::to_string(length)+"/"+std::to_string(limits::MAX_DATA_SIZE));}
std::string StringValue::toString() const {return mScript.toString();}
StringValue StringValue::add(const StringValue& value) const {return StringValue(mUTF16+value.mUTF16);}
}
