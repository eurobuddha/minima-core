#include "sphincs.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/sphincs/SPHINCS.hpp"
#include "org/minima/utils/sphincs/SPHINCSUtils.hpp"
#include "support/CommandResult.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/SPHINCSHelpers.hpp"
namespace minima::commands {
sphincs::sphincs():Command("sphincs","[action:] (seed:) (data:) (privatekey:) (publickey:) (file:) - SPHINCS signature scheme functionality"){}
std::optional<std::string> sphincs::getFullHelp() const {return "\nsphincs\n"
"\n"
"SPHINCS+ generate, sign and verify.\n"
"\n"
"Generates a SPHINCS+ key signs messages and verifies.\n"
"\n"
"action:\n"
"    generate - generate a sphincs key.\n"
"    sign - sign a piece of data.\n"
"    verify - sign a piece of data.\n"
"    transaction - send from a certain private key.\n"
"\n"
"seed:\n"
"    The seed used to generate a Key.\n"
"\n"
"data:\n"
"    The data to sign / verify\n"
"\n"
"file:\n"
"    The file path to save a signature to - can be the full path or relative to your base folder\n"
"\n"
"Examples:\n"
"\n"
"sphincs action:generate seed:mysecretseed\n"
"\n"
"sphincs action:sign data:0x001122 privatekey:0x..\n"
"\n"
"sphincs action:verify data:0x001122 publickey:0x.. signature:0x..|file:thefile\n"
"\n"
"sphincs action:transaction privatekey:0x.. address:Mx.. tokenid:0x.. amount:10\n";}
std::vector<std::string> sphincs::getValidParams() const {return {"action","seed","data","privatekey","file","publickey","signature","amount","address","tokenid","mine"};}
json::JSONValue sphincs::runCommand(){
    using minima::sphincs::SPHINCS;using minima::sphincs::SPHINCSSignature;using minima::sphincs::SPHINCSUtils;
    auto result=getJSONReply();json::JSONObject response;auto action=getParam("action");
    if(action=="generate"){
        auto text=getParam("seed");MiniData seed(Crypto::getInstance().hashData(std::span(reinterpret_cast<const uint8_t*>(text.data()),text.size())));
        SPHINCS signer(seed);auto script=SPHINCSUtils::getKISSVMScript(signer.getPublicKey().get());Address address(script);
        response.put("address",address.getMinimaAddress());response.put("script",script);
        response.put("publickey",sqlRequired(signer.getPublicKey()).to0xString());response.put("privatekey",sqlRequired(signer.getPrivateKey()).to0xString());
    }else if(action=="sign"){
        auto privatekey=getDataParam("privatekey");auto message=getDataParam("data");SPHINCS signer;signer.initPrivateKey(privatekey);auto signature=signer.signMessage(message);
        if(existsParam("file")){
            auto file=MiniFile::createBaseFile(getParam("file"));std::error_code error;
            if(file.string().find('\0')==std::string::npos&&std::filesystem::exists(file,error))std::filesystem::remove(file,error);
            MiniFile::writeObjectToFile(file,signature);response.put("signaturefile",javaUnixAbsoluteFile(file));response.put("size",javaUnixFileLength(file));
        }else response.put("signature",minima::sphincs::detail::versionHex(signature));
    }else if(action=="verify"){
        auto publickey=getDataParam("publickey");auto message=getDataParam("data");SPHINCSSignature signature;
        if(existsParam("file")){
            auto file=MiniFile::createBaseFile(getParam("file"));std::error_code error;
            if(file.string().find('\0')!=std::string::npos||!std::filesystem::exists(file,error))throw CommandException("File does not exist : "+javaUnixAbsoluteFile(file));
            MiniData bytes(MiniFile::readCompleteFile(file));signature=SPHINCSSignature::convertMiniDataVersion(bytes);
        }else signature=SPHINCSSignature::convertMiniDataVersion(getDataParam("signature"));
        response.put("valid",SPHINCS::verifySignature(message,signature,publickey));
    }else if(action=="transaction"){
        auto amount=getNumberParam("amount");auto address=getAddressParam("address");auto tokenid=getDataParam("tokenid");auto privatekey=getDataParam("privatekey");bool mine=getBooleanParam("mine",false);
        SPHINCS signer;signer.initPrivateKey(privatekey);auto script=SPHINCSUtils::getKISSVMScript(signer.getPublicKey().get());Address source(script);
        auto id=MiniData::getRandomData(32).to0xString();auto child=runCommand("txncreate id:"+id);
        child=runCommand("txnaddamount id:"+id+" fromaddress:"+source.getMinimaAddress()+" address:"+address+" amount:"+amount.toString()+" tokenid:"+tokenid.to0xString());
        if(!command_result::status(child)){runCommand("txndelete id:"+id);command_result::reject(child);}
        auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();auto row=sqlRequired(db).getTransactionRow(id);auto transaction=sqlRequired(row).getTransaction();
        if(sqlRequired(transaction).getAllInputs().size()>8){runCommand("txndelete id:"+id);throw CommandException("Input Coin number too great.. MAX 8. Pls Send a smaller amount.");}
        for(auto& coin:transaction->getAllOutputs())coin.setStoreState(false);
        auto message=SPHINCSUtils::calculateTransactionID(transaction.get());auto signature=signer.signMessage(message);SPHINCSUtils::setupTransaction(transaction.get(),&signature);
        runCommand("txnscript id:"+id+" scripts:{\""+script+"\":\"\"}");runCommand("txnmmr id:"+id);
        child=runCommand("txnpost id:"+id+" mine:"+(mine?"true":"false"));runCommand("txndelete id:"+id);
        if(!command_result::status(child))command_result::reject(child);
        result.put("response",child.get("response"));return result;
    }else if(action=="test"){
        std::string text="xxx";MiniData data=MiniData::fromHex("0x00");MiniData seed(Crypto::getInstance().hashData(std::span(reinterpret_cast<const uint8_t*>(text.data()),text.size())));
        SPHINCS signer(seed);auto signature=signer.signMessage(data);auto bytes=miniDataVersion(signature);if(!bytes)throw JavaNullPointerException("");
        auto decoded=SPHINCSSignature::convertMiniDataVersion(*bytes);response.put("verify",SPHINCS::verifySignature(&data,&decoded,signer.getPublicKey().get()));
    }else throw CommandException("undefined action : "+action);
    result.put("response",response);return result;
}
std::string sphincs::getOutCoinString(const Coin& coin){return minima::sphincs::detail::formatOutputCoin(coin,"SPHINCS","SPHINCS");}
json::JSONObject sphincs::runCommand(const std::string& text){return command_result::first(text);}
std::shared_ptr<Command> sphincs::getFunction() const {return std::make_shared<sphincs>();}
}
