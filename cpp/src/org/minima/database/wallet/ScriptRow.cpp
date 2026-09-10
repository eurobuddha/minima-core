#include "ScriptRow.hpp"
#include "org/minima/objects/Address.hpp"
namespace minima {
ScriptRow::ScriptRow(SqlRow& row){
    mScript=row.getString("script");mAddress=row.getString("address");mSimple=row.getInt("simple")!=0;
    mDefault=row.getInt("defaultaddress")!=0;mPublicKey=row.getString("publickey");mTrack=row.getInt("track")!=0;
}
json::JSONObject ScriptRow::toJSON() const {
    json::JSONObject result;result.put("script",mScript?json::JSONValue(*mScript):json::JSONValue());
    result.put("address",mAddress?json::JSONValue(*mAddress):json::JSONValue());
    result.put("miniaddress",Address::makeMinimaAddress(MiniData::fromHex(mAddress.value())));
    result.put("simple",mSimple);result.put("default",mDefault);
    result.put("publickey",mPublicKey?json::JSONValue(*mPublicKey):json::JSONValue());result.put("track",mTrack);return result;
}
}
