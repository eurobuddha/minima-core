#pragma once
#include "org/minima/objects/TxPoW.hpp"
namespace minima::testing {
class TestUtils {
public:
    static std::shared_ptr<TxPoW> getTxPoW(const std::string& id);
    static std::shared_ptr<TxPoW> getTxPoW(const std::string& id,int block,int weight);
};
}
