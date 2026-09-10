#include "org/minima/database/txpowdb/ram/RamDB.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
static std::string boolean(bool b){return b?"true":"false";}
static std::string text(const std::string& hex){auto data=BaseConverter::decode16(hex);return MiniString(data).toString();}
static std::string hex(const std::string& s){return BaseConverter::encode16(std::vector<uint8_t>(s.begin(),s.end()));}
static std::string ids(const std::vector<std::shared_ptr<TxPoW>>& list){std::string s;for(const auto& t:list)s+=t->getTxPoWID()+",";return s;}
static auto transaction(int i){auto t=std::make_shared<TxPoW>();t->setTimeMilli(MiniNumber(0));t->setNonce(MiniNumber(i));t->getTxBody()->mPRNG=MiniData::fromHex("0x12");if(i%5!=0){Coin c(MiniData(BigInt(i+1)),MiniData::fromHex("0x1122"),MiniNumber(i%13+1),Token::TOKENID_MINIMA);t->getTransaction().addInput(c);t->getWitness().addCoinProof(CoinProof(c,MMRProof{}));}t->setHeaderBodyHash();t->calculateTXPOWID();return t;}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{
    std::istringstream in(line);std::string op,out;in>>op;
    if(op=="map"){
        JavaConcurrentMap<int> map;int count;in>>count;
        for(int i=0;i<count;++i){std::string cmd,key,ret;in>>cmd;std::optional<int> value;
            if(cmd=="put"){int n;in>>key>>n;value=map.put(text(key),n);}
            else if(cmd=="get"){in>>key;value=map.get(text(key));}
            else if(cmd=="remove"){in>>key;value=map.remove(text(key));}
            else{map.clear();ret="clear";}
            if(ret.empty())ret=value?std::to_string(*value):"null";
            out+=ret+":"+std::to_string(map.size())+":";for(const auto& k:map.keys())out+=hex(k)+"="+std::to_string(map.get(k).value())+",";out+="|";
        }
    }else if(op=="ram"){
        int count,max,mode;in>>count>>max>>mode;RamDB db;db.MAX_UNUSED_TXNS=max;std::vector<std::shared_ptr<TxPoW>> txns;
        for(int i=0;i<count;++i){auto t=transaction(i);txns.push_back(t);out+=boolean(db.addTxPoW(t))+":";out+=boolean(db.addTxPoW(t))+",";if((mode&1)&&i%3==0)db.setOnMainChain(t->getTxPoWID());if((mode&2)&&i%7==0)db.setInCascade(t->getTxPoWID());}
        out+="|"+std::to_string(db.getSize())+"|"+ids(db.getAllUnusedTxns())+"|";
        for(int i=0;i<count;++i)out+=boolean(db.checkForCoinID(MiniData(BigInt(i+1))))+",";
        db.cleanUnusedTxn();out+="|"+std::to_string(db.getSize())+"|"+ids(db.getAllUnusedTxns())+"|";db.clearMainChainTxns();out+=ids(db.getAllUnusedTxns())+"|";
        for(const auto& t:txns)out+=boolean(bool(db.getTxPoW(t->getTxPoWID())))+",";
        auto old=db.getCompleteMemPool();db.MAX_TIME=0;db.cleanDB();out+="|"+std::to_string(db.getSize())+":"+std::to_string(old->size());db.wipeRamDB();out+="|"+std::to_string(db.getSize());
    }else throw std::invalid_argument(op);
    std::cout<<out<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
