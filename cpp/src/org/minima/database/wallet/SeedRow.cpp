#include "SeedRow.hpp"
namespace minima {
SeedRow::SeedRow(SqlRow& row){mPhrase=row.getString("phrase");mSeed=row.getString("seed");}
json::JSONObject SeedRow::toJSON() const {
    json::JSONObject result;result.put("phrase",mPhrase?json::JSONValue(*mPhrase):json::JSONValue());
    result.put("seed",mSeed?json::JSONValue(*mSeed):json::JSONValue());return result;
}
}
