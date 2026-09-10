#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include <array>
#include <optional>
namespace minima {
class Winternitz {
    std::optional<std::array<MiniData,34>> mPrivateKey;
    std::optional<MiniData> mPublicKey;
    static MiniData chain(MiniData value,int rounds);
    static std::array<int,34> digits(const MiniData& data);
public:
    Winternitz()=default;
    explicit Winternitz(const MiniData& seed);
    const std::optional<MiniData>& getPublicKey() const {return mPublicKey;}
    MiniData sign(const MiniData& data) const;
    static bool verify(const MiniData& publicKey,const MiniData& data,const MiniData& signature);
};
}
