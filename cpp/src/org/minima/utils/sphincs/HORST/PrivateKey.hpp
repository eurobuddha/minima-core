#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include <memory>
namespace minima::sphincs::horst {
class PrivateKey {
    std::vector<std::shared_ptr<MiniData>> mPrivateKeys;
public:
    explicit PrivateKey(const MiniData* seed);
    explicit PrivateKey(const MiniData& seed):PrivateKey(&seed){}
    int getSize() const {return int(mPrivateKeys.size());}
    std::shared_ptr<MiniData> getKey(int position) const;
};
}
