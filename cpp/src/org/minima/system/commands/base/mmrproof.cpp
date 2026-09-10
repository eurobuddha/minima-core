#include "mmrproof.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/objects/mmr/MMR.hpp"
namespace minima::commands {
static std::string trim(const std::string& text){size_t first=0,last=text.size();while(first<last&&uint8_t(text[first])<=32)++first;while(last>first&&uint8_t(text[last-1])<=32)--last;return text.substr(first,last-first);}
mmrproof::mmrproof():Command("mmrproof","[data:] [proof:] [root:] - Check an MMR proof"){}
std::optional<std::string> mmrproof::getFullHelp() const {return std::string(
"\nmmrproof\n"
"\n"
"Check an MMR Proof.\n"
"\n"
"Can be used to check MMR Proof of coins, scripts or a custom MMR tree created with the 'mmrcreate' command.\n"
"\n"
"Returns true if the proof is valid or false if not.\n"
"\n"
"data:\n"
"    String/HEX data of an MMR leaf node.\n"
"\n"
"proof:\n"
"    The MMR proof of the data from the 'mmrcreate' command.\n"
"\n"
"root:\n"
"    The root hash of the MMR tree from the 'mmrcreate' command.\n"
"\n"
"Examples:\n"
"\n"
"mmrproof data:0xCD34.. proof:0xFED5.. root:0xDAE6..\n");}
std::vector<std::string> mmrproof::getValidParams() const {return {"data","proof","root"};}
json::JSONValue mmrproof::runCommand(){
    auto result=getJSONReply();if(!existsParam("root")||!existsParam("proof")||!existsParam("data"))throw CommandException("MUST Specify data, root and proof");
    auto checkdata=getParam("data"),input=checkdata;MiniNumber sum;auto colon=checkdata.find(':');if(colon!=std::string::npos){input=trim(checkdata.substr(0,colon));sum=MiniNumber(trim(checkdata.substr(colon+1)));}
    auto fullroot=getParam("root"),rootstr=fullroot;MiniNumber rootsum;colon=fullroot.find(':');if(colon!=std::string::npos){rootstr=trim(fullroot.substr(0,colon));rootsum=MiniNumber(trim(fullroot.substr(colon+1)));}
    auto proofstr=getParam("proof");auto leaf=input.starts_with("0x")?MMRData::CreateMMRDataLeafNode(MiniData::fromHex(input),sum):MMRData::CreateMMRDataLeafNode(MiniString(input),sum);
    MMRData root(MiniData::fromHex(rootstr),rootsum);auto proof=MMRProof::convertMiniDataVersion(MiniData::fromHex(proofstr));auto calculated=proof.calculateProof(leaf);
    json::JSONObject response;response.put("input",input);response.put("leaf",leaf.toJSON());response.put("finaldata",calculated.toJSON());response.put("valid",calculated.isEqual(root));result.put("response",response);return result;
}
std::shared_ptr<Command> mmrproof::getFunction() const {return std::make_shared<mmrproof>();}
}
