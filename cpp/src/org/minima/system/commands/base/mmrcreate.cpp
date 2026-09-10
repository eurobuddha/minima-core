#include "mmrcreate.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/objects/mmr/MMR.hpp"
namespace minima::commands {
mmrcreate::mmrcreate():Command("mmrcreate","[nodes:[]] - Create an MMR Tree of data. Nodes can be STRING / HEX"){}
std::optional<std::string> mmrcreate::getFullHelp() const {return std::string(
"\nmmrcreate\n"
"\n"
"Create an MMR Tree of data. Can be used in MAST contracts.\n"
"\n"
"Must specify a JSON array of string/HEX data for the leaf nodes.\n"
"\n"
"They could be a list of public keys.. and then in a script you can check if the given key is one in the set.\n"
"\n"
"OR you can have different scripts.. and then you can execute any number of scripts from the same UTXO.\n"
"\n"
"Returns the MMR data and proof for each leaf node and the MMR root hash.\n"
"\n"
"nodes:\n"
"    JSON array of string/HEX data for the leaf nodes.\n"
"\n"
"Examples:\n"
"\n"
"mmrcreate nodes:[\"RETURN TRUE\",\"RETURN FALSE\"]\n"
"\n"
"mmrcreate nodes:[\"0xFF..\",\"0xEE..\"]\n");}
std::vector<std::string> mmrcreate::getValidParams() const {return {"nodes"};}
json::JSONValue mmrcreate::runCommand(){
    auto result=getJSONReply();auto nodes=getJSONArrayParam("nodes");if(!nodes)throw std::runtime_error("Null nodes array");MMR tree;std::vector<mmrleafnode> leaves;
    for(const auto& node:nodes->elements()){
        auto full=node.asString();auto colon=full.find(':');auto input=full.substr(0,colon);auto sum=colon==std::string::npos?MiniNumber::ZERO:MiniNumber(full.substr(colon+1));
        auto leaf=input.starts_with("0x")?MMRData::CreateMMRDataLeafNode(MiniData::fromHex(input),sum):MMRData::CreateMMRDataLeafNode(MiniString(input),sum);
        mmrleafnode record;record.mEntry=int32_t(leaves.size());record.mInput=input;record.mLeafData=std::make_shared<MMRData>(leaf);leaves.push_back(record);tree.addEntry(std::move(leaf));
    }
    auto root=tree.getRoot();json::JSONArray rows;
    for(const auto& leaf:leaves){json::JSONObject row;row.put("entry",leaf.mEntry);row.put("data",*leaf.mInput);row.put("value",leaf.mLeafData->getValue().toString());row.put("proof",MiniData(tree.getProof(MMREntryNumber(leaf.mEntry)).serialise()).to0xString());rows.add(row);}
    json::JSONObject response;response.put("nodes",rows);response.put("total",int32_t(leaves.size()));if(!root)throw std::runtime_error("Null MMR root");response.put("root",root->toJSON());result.put("response",response);return result;
}
std::shared_ptr<Command> mmrcreate::getFunction() const {return std::make_shared<mmrcreate>();}
}
