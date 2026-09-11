#pragma once
#include "ArchiveFixture.hpp"
#include "org/minima/system/commands/backup/mmrsync/megammr.hpp"
#include "org/minima/system/commands/backup/mmrsync/megammrsync.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
inline const json::JSONArray& megaArray(const json::JSONObject& input,const std::string& key){return *std::get<std::shared_ptr<json::JSONArray>>(input.get(key).value);}
inline Coin megaCoin(const json::JSONObject& row,int index){
    Coin coin(MiniData::fromHex(row.getString("id")),MiniData::fromHex(row.getString("address")),MiniNumber(row.getString("amount")),MiniData::ZERO_TXPOWID);
    coin.setMMREntryNumber(MMREntryNumber(index));coin.setBlockCreated(MiniNumber::ONE);coin.setSpent(row.containsKey("spent")&&row.getBoolean("spent"));std::vector<StateVariable> states;if(row.containsKey("state"))states.emplace_back(0,row.getString("state"));coin.setState(states);return coin;
}
inline auto megaList(const json::JSONObject& input,const std::string& key){std::shared_ptr<MegaMMRSyncData::DataList> result;if(input.get(key).isNull())return result;result=std::make_shared<MegaMMRSyncData::DataList>();for(const auto& value:megaArray(input,key).elements())result->push_back(value.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(value.asString())));return result;}
inline json::JSONObject megaFixture(const json::JSONObject& input,const std::shared_ptr<MinimaDB>& db){
    if(input.getString("task")=="saved"){
        json::JSONObject out;
        MegaMMRBackup backup;MiniFile::loadObjectSlow(input.getString("path"),backup);
        out.put("backupIBD",MiniData(backup.getIBD()->serialise()).to0xString());
        auto details=commands::megammrsync::getMyDetails();
        out.put("request",MiniData(NIOManager::createNIOMessage(NIOMessage::MSG_MEGAMMRSYNC_REQ,details).serialise()).to0xString());
        auto current=NIOMessage::services.getCurrentMegaMMRIBD(details);
        out.put("currentIBD",MiniData(current->getIBD()->serialise()).to0xString());
        json::JSONArray proofs;for(const auto& proof:*current->getAllCoinProofs())proofs.add(MiniData(proof->serialise()).to0xString());out.put("proofs",proofs);
        current->setPeersList(input.getString("peers"));
        out.put("wire",MiniData(NIOManager::createNIOMessage(NIOMessage::MSG_MEGAMMRSYNC_RESP,*current).serialise()).to0xString());
        out.put("weight",commands::megammr::checkMegaMMR(std::filesystem::path(input.getString("path"))).toDecimalString());
        return out;
    }
    using namespace json;JSONObject out;auto mega=db->getMegaMMR();mega->clear();int index=0;
    for(const auto& value:megaArray(input,"coins").elements()){auto coin=std::make_shared<Coin>(megaCoin(*std::get<std::shared_ptr<JSONObject>>(value.value),index++));mega->getMMR().addEntry(MMRData::CreateMMRDataLeafNode(*coin,coin->getAmount()));mega->getAllCoins().put(coin->getCoinID().to0xString(),coin);}
    mega->getMMR().setBlockTime(MiniNumber(input.getString("time")));std::shared_ptr<TxPoWTreeNode> first,last;int height=0;
    for(const auto& values:megaArray(input,"tree").elements()){auto node=std::make_shared<TxPoWTreeNode>(*support::archiveFixtureTxPoW(++height,0));if(!first)first=node;else last->addChildNode(node);last=node;int at=0;for(const auto& value:std::get<std::shared_ptr<JSONArray>>(values.value)->elements())node->addCoinToNode(megaCoin(*std::get<std::shared_ptr<JSONObject>>(value.value),at++));}
    if(first)db->getTxPoWTree()->setRoot(first);auto ibd=std::make_shared<IBD>();int cascade=int(std::get<int64_t>(input.get("cascade").value));if(cascade>=0){auto casc=std::make_shared<Cascade>();for(int i=1;i<=cascade;++i)casc->addToTip(*support::archiveFixtureTxPoW(i,0));casc->cascadeChain();ibd->setCascade(casc);}
    for(const auto& number:megaArray(input,"blocks").elements())ibd->getTxBlocks()->push_back(std::make_shared<TxBlock>(*support::archiveFixtureTxPoW(int(std::get<int64_t>(number.value)),0)));
    MegaMMRBackup backup(mega,ibd);if(input.containsKey("badCoin")&&input.getBoolean("badCoin")&&!mega->getAllCoins().empty())mega->getAllCoins().elements().front()->resetCoinID(MiniData::fromHex("0xFFFF"));
    if(input.containsKey("walletRows")&&input.getBoolean("walletRows")){auto wallet=db->getWallet();wallet->addScript("RETURN TRUE",false,false,"0x00",true);wallet->addScript("RETURN FALSE",true,true,"0x11",false);wallet->addScript("RETURN SIGNEDBY(0x22)",true,false,"0x22",true);wallet->executeGenericSQL("INSERT INTO keys(size,depth,uses,maxuses,modifier,privatekey,publickey) VALUES (2,1,0,2,'0x00','0x00','0x11'),(2,1,0,2,'0x00','0x00','0x22'),(2,1,0,2,'0x00','0x00','0x00')");}
    auto task=input.getString("task");try{
        if(task=="check"){out.put("weight",commands::megammr::checkMegaMMR(backup).toDecimalString());if(input.containsKey("twice")&&input.getBoolean("twice"))out.put("second",commands::megammr::checkMegaMMR(backup).toDecimalString());}
        else if(task=="encode"){MegaMMRIBD value(ibd,std::make_shared<MegaMMRIBD::Proofs>());value.setPeersList(input.getString("peers", ""));out.put("ibd",MiniData(value.serialise()).to0xString());out.put("wire",MiniData(NIOManager::createNIOMessage(MiniByte(int32_t(std::get<int64_t>(input.get("type").value))),value).serialise()).to0xString());}
        else if(task=="write"){MiniFile::writeObjectToFile(input.getString("path"),backup);out.put("wire",MiniData(backup.serialise()).to0xString());}
        else if(task=="search"){auto coins=commands::megammrsync::searchMegaCoins(megaList(input,"addresses"),megaList(input,"publickeys"));JSONArray rows;for(const auto& coin:coins)rows.add(coin.toJSON());out.put("coins",rows);if(!coins.empty())coins.front().resetCoinID(MiniData::fromHex("0xEEEE"));}
        else if(task=="peers"){commands::megammrsync command;command.updateP2PDB(input.get("peers").isNull()?std::nullopt:std::optional(input.getString("peers")));out.put("saved",true);}
        else if(task=="details"){auto data=commands::megammrsync::getMyDetails();out.put("wire",MiniData(data.serialise()).to0xString());}
        else if(task=="current"){auto data=MegaMMRSyncData(megaList(input,"addresses"),megaList(input,"publickeys"));auto current=NIOMessage::services.getCurrentMegaMMRIBD(data);out.put("wire",MiniData(current->serialise()).to0xString());}
        else {std::shared_ptr<Command> command;if(input.getString("name")=="megammr")command=std::make_shared<commands::megammr>();else command=std::make_shared<commands::megammrsync>();
            if(task=="meta"){out.put("name",command->getName().value());out.put("help",command->getHelp().value());out.put("fullHelp",command->getFullHelp().value());JSONArray params;for(const auto& value:command->getValidParams())params.add(value);out.put("params",params);command->getParams()->put("file","held");out.put("fresh",*command->getFunction()->getParams());}
            else if(task=="runner")out.put("result",CommandRunner::getRunner().runMultiCommand(input.getString("command")));
            else {*command->getParams()=*std::get<std::shared_ptr<JSONObject>>(input.get("params").value);out.put("result",command->runCommand());}
        }
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    out.put("time",mega->getMMR().getBlockTime().toString());out.put("finalized",mega->getMMR().isFinalized());out.put("mega",MiniData(mega->serialise()).to0xString());return out;
}
}
