#pragma once
#include "CommandException.hpp"
#include "org/minima/objects/Address.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
namespace minima {
class Command {
    std::optional<std::string> mName,mHelp,mCompleteCommand="",mMiniDAPPID="";
    std::shared_ptr<json::JSONObject> mParams=std::make_shared<json::JSONObject>();
public:
    Command(std::optional<std::string> name,std::optional<std::string> help):mName(std::move(name)),mHelp(std::move(help)){}
    virtual ~Command()=default;
    void setMiniDAPPID(std::optional<std::string> id){mMiniDAPPID=std::move(id);}
    const auto& getMiniDAPPID() const {return mMiniDAPPID;}
    void setCompleteCommand(std::optional<std::string> command){mCompleteCommand=std::move(command);}
    const auto& getCompleteCommand() const {return mCompleteCommand;}
    const auto& getHelp() const {return mHelp;}
    virtual std::optional<std::string> getFullHelp() const {return mHelp;}
    const auto& getName() const {return mName;}
    virtual std::vector<std::string> getValidParams() const {return {};}
    std::shared_ptr<json::JSONObject> getParams() const {return mParams;}
    json::JSONObject getJSONReply() const;
    bool existsParam(const std::string& name) const;
    std::string getParam(const std::string& name) const;
    std::string getParam(const std::string& name,const std::string& fallback) const;
    std::u16string getParamUTF16(const std::string& name) const;
    std::u16string getParamUTF16(const std::string& name,const std::u16string& fallback) const;
    std::optional<std::string> getParam(const std::string& name,std::nullptr_t) const;
    bool getBooleanParam(const std::string& name) const;
    bool getBooleanParam(const std::string& name,bool fallback) const;
    MiniNumber getNumberParam(const std::string& name) const;
    MiniNumber getNumberParam(const std::string& name,const MiniNumber& fallback) const;
    std::optional<MiniNumber> getNumberParam(const std::string& name,std::nullptr_t) const;
    MiniData getDataParam(const std::string& name) const;
    MiniData getDataParam(const std::string& name,const MiniData& fallback) const;
    std::optional<MiniData> getDataParam(const std::string& name,std::nullptr_t) const;
    std::shared_ptr<json::JSONObject> getJSONObjectParam(const std::string& name) const;
    std::shared_ptr<json::JSONObject> getJSONObjectParam(const std::string& name,std::shared_ptr<json::JSONObject> fallback) const;
    std::shared_ptr<json::JSONArray> getJSONArrayParam(const std::string& name) const;
    std::string getAddressParam(const std::string& name) const;
    std::string getAddressParam(const std::string& name,const std::string& fallback) const;
    std::optional<std::string> getAddressParam(const std::string& name,std::nullptr_t) const;
    bool isParamJSONObject(const std::string& name) const;
    bool isParamJSONArray(const std::string& name) const;
    // Source JSONObject references may be null; concrete replies are objects or null.
    virtual json::JSONValue runCommand()=0;
    virtual std::shared_ptr<Command> getFunction() const=0;
};
}
