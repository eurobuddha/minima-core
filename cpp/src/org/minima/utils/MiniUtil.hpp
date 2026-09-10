#pragma once
#include "json/JSONArray.hpp"
#include <optional>
namespace minima {
class MiniUtil {
public:
    using StringList=std::vector<std::optional<std::string>>;
    using UTF16StringList=std::vector<std::optional<std::u16string>>;
    struct DateFormat {
        std::string format(int64_t millis) const;
    };
    static const DateFormat DATEFORMAT;
    static std::shared_ptr<json::JSONArray> convertArrayList(const StringList& strings);
    static StringList convertJSONArray(const std::shared_ptr<json::JSONArray>& array);
    static std::shared_ptr<json::JSONArray> convertUTF16ArrayList(const UTF16StringList& strings);
    static UTF16StringList convertUTF16JSONArray(const std::shared_ptr<json::JSONArray>& array);
};
}
