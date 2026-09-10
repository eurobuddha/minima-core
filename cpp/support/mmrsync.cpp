#include "ArchiveFixture.hpp"
#include "org/minima/system/commands/backup/mmrsync/MegaMMRBackup.hpp"
#include "org/minima/system/commands/backup/mmrsync/MegaMMRIBD.hpp"
#include "org/minima/system/commands/backup/mmrsync/MegaMMRSyncData.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
template<class T> static std::string hex(const std::shared_ptr<T>& value){if(!value)return "null";try{return MiniData(value->serialise()).to0xString();}catch(const std::exception&){return "ERROR";}}
template<class T> static std::string list(const std::shared_ptr<T>& values){if(!values)return "null";std::string out="[";for(const auto& value:*values)out+=hex(value)+",";return out+"]";}
static std::string state(const MegaMMRBackup& b){return hex(b.getMegaMMR())+"|"+hex(b.getIBD());}
static std::string state(const MegaMMRIBD& b){return hex(b.getIBD())+"|"+list(b.getAllCoinProofs())+"|"+(b.getPeersList()?MiniData(MiniString(*b.getPeersList()).getData()).to0xString():"null");}
static std::string state(const MegaMMRSyncData& b){return list(b.getAllAddresses())+"|"+list(b.getAllPublicKeys());}
static auto refs(const MegaMMRBackup& b){return std::pair{b.getMegaMMR(),b.getIBD()};}
static auto refs(const MegaMMRIBD& b){return std::pair{b.getIBD(),b.getAllCoinProofs()};}
static auto refs(const MegaMMRSyncData& b){return std::pair{b.getAllAddresses(),b.getAllPublicKeys()};}
template<class T> static std::string run(const std::string& op,T& value,std::istringstream& in){if(op=="make")try{return MiniData(value.serialise()).to0xString();}catch(const std::exception&){return "ERROR";}
    std::string data;in>>data;auto bytes=MiniData::fromHex(data);size_t offset=0;auto before=refs(value);std::string result="OK";try{value.readDataStream(bytes.bytes(),offset);}catch(const wire::IOException&){result="IO_ERROR";}catch(const std::exception&){result="ERROR";}auto after=refs(value);return result+"|"+std::to_string(offset)+"|"+(before.first==after.first?"true":"false")+"|"+(before.second==after.second?"true":"false")+"|"+state(value);
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::istringstream in(line);std::string op,type;int n;in>>op>>type>>n;auto ibd=std::make_shared<IBD>();for(int i=0;i<n;++i)ibd->getTxBlocks()->push_back(std::make_shared<TxBlock>(*support::archiveFixtureTxPoW(i,0)));std::string out;
    if(type=="backup"){auto mega=std::make_shared<MegaMMR>();for(int i=0;i<n;++i)mega->getMMR().addEntry(MegaMMR::getCoinData(MiniNumber(i+1)));auto value=n<0?MegaMMRBackup():MegaMMRBackup(mega,ibd);out=run(op,value,in);}
    else if(type=="ibd"){auto proofs=std::make_shared<MegaMMRIBD::Proofs>();auto value=n<0?MegaMMRIBD():MegaMMRIBD(ibd,proofs);for(int i=0;i<n;++i)proofs->push_back(std::make_shared<CoinProof>(Coin(MiniData::fromHex("0x1234"),MiniNumber(i+1),MiniData::fromHex("0x00")),MMRProof(MiniNumber(i))));if(n>=0)value.setPeersList("127.0.0.1:9001,π.example:9002");out=run(op,value,in);}
    else {auto addresses=std::make_shared<MegaMMRSyncData::DataList>(),keys=std::make_shared<MegaMMRSyncData::DataList>();auto value=n<0?MegaMMRSyncData():MegaMMRSyncData(addresses,keys);for(int i=0;i<n;++i){addresses->push_back(std::make_shared<MiniData>(std::vector<uint8_t>(i)));keys->push_back(std::make_shared<MiniData>(std::vector<uint8_t>(i+1)));}out=run(op,value,in);}
    std::cout<<out<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
