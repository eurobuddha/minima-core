#pragma once
#include "support/SqlRow.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class ScriptRow {
public:
    std::optional<std::string> mScript,mAddress;
    bool mSimple=false,mDefault=false;
    std::optional<std::string> mPublicKey;
    bool mTrack=false;
    explicit ScriptRow(SqlRow& row);
    ScriptRow(std::optional<std::string> script,std::optional<std::string> address,bool simple,bool defaultAddress,std::optional<std::string> publicKey,bool track)
        :mScript(std::move(script)),mAddress(std::move(address)),mSimple(simple),mDefault(defaultAddress),mPublicKey(std::move(publicKey)),mTrack(track){}
    const auto& getScript() const {return mScript;}
    const auto& getAddress() const {return mAddress;}
    bool isSimple() const {return mSimple;}
    bool isDefault() const {return mDefault;}
    const auto& getPublicKey() const {return mPublicKey;}
    bool isTrack() const {return mTrack;}
    json::JSONObject toJSON() const;
};
}
