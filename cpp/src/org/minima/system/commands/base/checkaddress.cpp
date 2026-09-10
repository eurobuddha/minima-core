#include "checkaddress.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "support/Base64.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaLowerCase.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
namespace minima::commands {
checkaddress::checkaddress():Command("checkaddress","[address:] - Check an address is valid"){}
std::vector<std::string> checkaddress::getValidParams() const {return {"address"};}
json::JSONValue checkaddress::runCommand(){
    auto result=getJSONReply();auto address=getAddressParam("address");
    if(address.starts_with("0x")&&address.size()!=66)throw CommandException("Invalid Length for 0x address should be 66 chars long : "+std::to_string(address.size()));
    if(!address.starts_with("Mx")&&!address.starts_with("0x"))throw CommandException("Address does not start with 0x or Mx");
    auto& wallet=TxPoWTreeNode::services;
    if(!wallet.isAddressRelevant||!wallet.isAddressSimple)throw std::logic_error("checkaddress requires Wallet bindings");
    auto data=MiniData::fromHex(address);auto hex=data.to0xString();json::JSONObject response;
    response.put("original",address);response.put("0x",hex);response.put("Mx",Address::makeMinimaAddress(data));response.put("relevant",wallet.isAddressRelevant(hex));response.put("simple",wallet.isAddressSimple(hex));result.put("response",response);return result;
}
std::shared_ptr<Command> checkaddress::getFunction() const {return std::make_shared<checkaddress>();}
}
