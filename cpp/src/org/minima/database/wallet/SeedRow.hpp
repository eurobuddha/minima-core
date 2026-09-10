#pragma once
#include "support/SqlRow.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class SeedRow {
public:
    std::optional<std::string> mPhrase,mSeed;
    explicit SeedRow(SqlRow& row);
    SeedRow(std::optional<std::string> phrase,std::optional<std::string> seed):mPhrase(std::move(phrase)),mSeed(std::move(seed)){}
    const auto& getPhrase() const {return mPhrase;}
    const auto& getSeed() const {return mSeed;}
    json::JSONObject toJSON() const;
};
}
