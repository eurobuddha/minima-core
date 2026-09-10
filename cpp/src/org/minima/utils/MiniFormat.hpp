#pragma once
#include "org/minima/objects/base/MiniNumber.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class MiniFormat {
public:
    static json::JSONArray convertToJSON(const std::string& text);
    static std::string JSONPretty(const json::JSONArray& array,bool debug=false);
    static std::string JSONPretty(const json::JSONObject& object,bool debug=false);
    static std::string formatSize(int64_t bytes);
    static std::string zeroPad(int32_t total,const MiniNumber& number);
    static std::string ConvertMilliToTime(int64_t millis);
    static std::string createRandomString(int32_t length);
};
}
