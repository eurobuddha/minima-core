#pragma once
#include "MiniFile.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include "json/JSONArray.hpp"
namespace minima {
class JsonDB {
protected:
    std::shared_ptr<json::JSONObject> mParams=std::make_shared<json::JSONObject>();
    json::JSONObject& params() const {if(!mParams)throw std::logic_error("Null JsonDB parameters");return *mParams;}
public:
    virtual ~JsonDB()=default;
    std::shared_ptr<json::JSONObject> getAllData() const {return mParams;}
    bool exists(const std::string& name) const;
    bool getBoolean(const std::string& name,bool fallback) const;
    void setBoolean(const std::string& name,bool value);
    MiniNumber getNumber(const std::string& name,const MiniNumber& fallback) const;
    void setNumber(const std::string& name,const MiniNumber& value);
    MiniData getData(const std::string& name,const MiniData& fallback) const;
    void setData(const std::string& name,const MiniData& value);
    std::optional<std::string> getString(const std::string& name) const;
    std::string getString(const std::string& name,const std::string& fallback) const;
    void setString(const std::string& name,const std::optional<std::string>& value);
    void setJSON(const std::string& name,std::shared_ptr<json::JSONObject> value);
    std::shared_ptr<json::JSONObject> getJSON(const std::string& name,std::shared_ptr<json::JSONObject> fallback) const;
    void setJSONArray(const std::string& name,std::shared_ptr<json::JSONArray> value);
    std::shared_ptr<json::JSONArray> getJSONArray(const std::string& name);
    virtual void loadDB(const std::filesystem::path& file);
    virtual void saveDB(const std::filesystem::path& file);
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> bytes,size_t& offset);
    static JsonDB deserialise(std::span<const uint8_t> bytes,size_t& offset) {JsonDB db;db.readDataStream(bytes,offset);return db;}
};
}
