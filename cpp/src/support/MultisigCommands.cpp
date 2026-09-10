#include "MultisigCommands.hpp"
#include "MultisigContract.hpp"
#include "CommandResult.hpp"
#include "StateText.hpp"
#include "JavaUnixFile.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/commands/backup/vault.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include <bit>
namespace minima::commands::multisig_detail {
json::JSONValue run(Command& command,bool readOnly){
    auto result=command.getJSONReply();auto action=command.getParam("action");
    auto multisigAddress=Address(::minima::MULTISIG_CONTRACT).getAddressData().to0xString();
    auto randomID=MiniData::getRandomData(32).to0xString();
    auto search=[&]{auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();return TxPoWSearcher::searchCoins(tip,true,false,MiniData::ZERO_TXPOWID,false,MiniNumber::ZERO,true,MiniData::fromHex(multisigAddress),false,MiniData::ZERO_TXPOWID,false);};
    auto idHash=[&]{return Crypto::getInstance().hashObject(MiniString(command.getParam("id"))).to0xString();};
    auto runChildren=[](const std::string& text){return CommandRunner::getRunner().runMultiCommand(text);};
    if(action=="create"&&!readOnly){
        std::string id;if(command.existsParam("id"))id=idHash();auto root=command.getParam("root","0x21");
        if(root!="0x21"&&(!root.starts_with("0x")||unicode::decodeUTF8(root).size()!=66))throw CommandException("Invalid root key : "+root);
        auto amount=command.getNumberParam("amount");auto tokenID=command.getParam("tokenid","0x00");auto required=command.getNumberParam("required");
        std::vector<std::string> keys;auto values=command.getJSONArrayParam("publickeys");
        for(const auto& value:sqlRequired(values).elements()){
            if(value.isNull())throw JavaNullPointerException("null multisig key");if(!value.isString())throw JavaClassCastException("Cannot cast public key to String");
            auto key=value.asString();if(!key.starts_with("0x")||value.asUTF16().size()!=66)throw CommandException("Invalid public key : "+key);keys.push_back(key);
        }
        auto count=std::bit_cast<int32_t>(uint32_t(keys.size()));if(count<required.getAsInt())throw CommandException("Cannot have LESS keys than required!");
        std::string state="{";if(!id.empty())state+="\"0\":\""+id+"\",";
        state+="\"1\":\""+root+"\",\"2\":\""+required.toString()+"\",\"3\":\""+std::to_string(count)+"\"";
        uint32_t port=4;for(const auto& key:keys){state+=",\""+std::to_string(std::bit_cast<int32_t>(port))+"\":\""+key+"\"";++port;}state+="}";
        std::string text="send ";if(command.existsParam("password"))text+="password:"+command.getParam("password")+" ";
        text+="tokenid:"+tokenID+" amount:"+amount.toString()+" address:"+multisigAddress+" state:"+state;
        auto sent=command_result::first(text);
        if(command_result::status(sent)){json::JSONObject response;response.put("send",sent.get("response"));response.put("id",id);result.put("response",response);}
        else{result.put("status",false);auto message=sent.get("message"),error=sent.get("error");result.put("message",!message.isNull()?message:!error.isNull()?error:json::JSONValue(sent));}
    }else if(action=="getkey"){
        auto row=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getDefaultAddress();auto key=sqlRequired(row).getPublicKey();json::JSONObject response;response.put("publickey",key?json::JSONValue(*key):json::JSONValue());result.put("response",response);
    }else if(action=="list"){
        auto coins=search();json::JSONArray response;
        if(command.existsParam("id")){auto id=idHash();for(const auto& coin:coins)for(const auto& state:coin.getState())if(state.getData().toString()==id)response.add(coin.toJSON());}
        else for(const auto& coin:coins)response.add(coin.toJSON());result.put("response",response);
    }else if(action=="spend"){
        std::string coinID;
        if(command.existsParam("id")){
            auto id=idHash();auto coins=search();bool found=false;
            for(const auto& coin:coins){for(const auto& state:coin.getState())if(state.getData().toString()==id){coinID=coin.getCoinID().to0xString();found=true;break;}if(found)break;}
            if(!found)throw CommandException("MultiSig with id not found : "+id);
        }else coinID=command.getParam("coinid");
        auto coin=TxPoWSearcher::searchCoin(MiniData::fromHex(coinID),false);if(!coin)throw CommandException("CoinID not found : "+coinID);
        auto amount=command.getNumberParam("amount");auto change=MiniNumber::ZERO;
        if(coin->getTokenID().isEqual(MiniData::ZERO_TXPOWID))change=coin->getAmount().sub(amount);
        else{auto token=coin->getToken();if(!token)throw JavaNullPointerException("null multisig token");change=token->getScaledTokenAmount(coin->getAmount()).sub(amount);}
        auto tokenID=coin->getTokenID().to0xString();auto address=readOnly?command.getParam("address"):command.getAddressParam("address");auto coinAddress=coin->getAddress().to0xString();
        if(address==coinAddress)throw CommandException("CANNOT send funds back to yourself!");
        auto filename=command.getParam("file","multispend_"+std::to_string(TimerMessage::currentTimeMillis())+".txn");
        std::string text="txncreate id:"+randomID+";txninput  id:"+randomID+" coinid:"+coinID+";txnoutput id:"+randomID+" storestate:false amount:"+amount.toString()+" address:"+address+" tokenid:"+tokenID+";";
        if(change.isMore(MiniNumber::ZERO)){
            for(const auto& state:coin->getState())text+="txnstate id:"+randomID+" port:"+std::to_string(state.getPort())+" value:"+state.getData().toString()+";";
            text+="txnoutput id:"+randomID+" storestate:true amount:"+change.toString()+" address:"+coinAddress+" tokenid:"+tokenID+";";
        }
        text+="txnexport id:"+randomID+" file:"+filename+";txndelete id:"+randomID;result.put("response",runChildren(text));
    }else if(action=="sign"&&!readOnly){
        auto file=command.getParam("file");auto actual=MiniFile::createBaseFile(file);auto parent=actual==actual.root_path()?std::filesystem::path():actual.parent_path();auto name="signed_"+actual.filename().string();
        auto signedFile=parent.empty()?std::filesystem::path(name):javaUnixChildFile(parent,name);
        auto imported=command_result::first("txnimport id:"+randomID+" file:"+file+";");
        if(!command_result::status(imported)){auto error=imported.get("error");if(error.isNull())throw JavaNullPointerException("null import error");throw CommandException(send_detail::stateText(error));}
        std::string text;auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();auto row=sqlRequired(db).getTransactionRow(randomID);auto transaction=sqlRequired(row).getTransaction();auto& inputs=sqlRequired(transaction).getAllInputs();
        if(inputs.empty())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index 0 out of bounds for length 0");
        for(const auto& state:inputs.front().getState()){auto key=state.getData().toString();if(sqlRequired(wallet).getKeyFromPublic(key))text+="txnsign id:"+randomID+" publickey:"+key+";";}
        text+="txnexport id:"+randomID+" file:"+javaUnixAbsoluteFile(signedFile)+";txndelete id:"+randomID;
        bool passwordLock=false;if(command.existsParam("password")&&!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable()){vault::passowrdUnlockDB(command.getParam("password"));passwordLock=true;}
        auto replies=runChildren(text);if(passwordLock)vault::passwordLockDB(command.getParam("password"));result.put("response",replies);
    }else if(action=="post"){
        auto file=command.getParam("file");result.put("response",runChildren("txnimport id:"+randomID+" file:"+file+";txnpost   id:"+randomID+" mine:true auto:true;txndelete id:"+randomID));
    }else if(action=="view"){
        auto file=command.getParam("file");result.put("response",runChildren("txnimport id:"+randomID+" file:"+file+";txndelete id:"+randomID));
    }else throw CommandException("Invalid action : "+action);
    return result;
}
}
