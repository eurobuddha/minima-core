#pragma once
#include "org/minima/database/userprefs/UserDB.hpp"
#include <functional>
namespace minima {
class Authorizer {
public:
    static std::function<std::shared_ptr<UserDB>()> userDB;
    static json::JSONObject checkAuchCredentials(const std::optional<std::string>& header);
    static json::JSONObject checkAuchCredentials(const std::optional<std::string>& header,UserDB& users);
};
}
