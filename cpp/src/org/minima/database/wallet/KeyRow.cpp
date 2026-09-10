#include "KeyRow.hpp"
namespace minima {
KeyRow::KeyRow(SqlRow& row){
    mSize=row.getInt("size");mDepth=row.getInt("depth");mUses=row.getInt("uses");mMaxUses=row.getInt("maxuses");
    mModifier=row.getString("modifier");mPrivateKey=row.getString("privatekey");mPublicKey=row.getString("publickey");
}
json::JSONObject KeyRow::toJSON() const {
    json::JSONObject result;result.put("size",mSize);result.put("depth",mDepth);result.put("uses",mUses);result.put("maxuses",mMaxUses);
    result.put("modifier",mModifier?json::JSONValue(*mModifier):json::JSONValue());
    result.put("publickey",mPublicKey?json::JSONValue(*mPublicKey):json::JSONValue());return result;
}
}
