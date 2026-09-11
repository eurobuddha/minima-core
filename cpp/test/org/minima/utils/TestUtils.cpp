#include "TestUtils.hpp"
namespace minima::testing {
std::shared_ptr<TxPoW> TestUtils::getTxPoW(const std::string& id){return getTxPoW(id,1,0);}
std::shared_ptr<TxPoW> TestUtils::getTxPoW(const std::string& id,int block,int weight){return std::make_shared<TxPoW>(id,block,weight);}
}
