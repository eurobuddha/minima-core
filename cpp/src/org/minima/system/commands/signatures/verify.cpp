#include "verify.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
namespace minima::commands {
verify::verify():Command("verify","[publickey:] [data:] [signature:] - Verify a signature"){}
std::optional<std::string> verify::getFullHelp() const {return std::string(
"\nverify\n"
"\n"
"Verify a signature. Returns valid true or false.\n"
"\n"
"data:\n"
"    The 0x HEX data to verify the signature for.\n"
"\n"
"publickey:\n"
"    The public key of the signer.\n"
"\n"
"signature:\n"
"    The signature of the data.\n"
"\n"
"Examples:\n"
"\n"
"verify data:0xCD34.. publickey:0xFED5 signature:0x4827..\n");}
std::vector<std::string> verify::getValidParams() const {return {"publickey","data","signature"};}
json::JSONValue verify::runCommand(){
    auto result=getJSONReply();auto data=getDataParam("data"),publickey=getDataParam("publickey"),bytes=getDataParam("signature");
    auto signature=Signature::convertMiniDataVersion(bytes);if(!signature)throw std::runtime_error("Null decoded signature");auto root=signature->getRootPublicKey();
    if(!publickey.isEqual(root))throw CommandException("Signature publickey is different : "+root.to0xString());
    TreeKey checker;checker.setPublicKey(root);if(!checker.verify(data,*signature)){result.put("status",false);result.put("message","Signature NOT valid");}else result.put("response","Signature valid");return result;
}
std::shared_ptr<Command> verify::getFunction() const {return std::make_shared<verify>();}
}
