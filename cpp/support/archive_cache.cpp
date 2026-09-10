#include "ArchiveFixture.hpp"
#include "org/minima/database/archive/TxBlockDB.hpp"
#include "org/minima/database/txpowdb/sql/TxPoWList.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
static std::string key(int i){std::string s;for(int bit=0;bit<7;++bit)s+=((i>>bit)&1)==0?"BB":"Aa";return s;}
static std::string ids(const std::vector<std::shared_ptr<TxBlock>>& list){std::string out;for(const auto& b:list)out+=b->getTxPoW().getTxPoWID()+",";return out;}
static auto txpow(int i,int mode){return minima::support::archiveFixtureTxPoW(i,mode);}
static std::string boolean(bool value){return value?"true":"false";}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::istringstream in(line);std::string op,out;in>>op;
    if(op=="make"){int n,mode;in>>n>>mode;auto list=std::make_shared<TxPoWList::Rows>();TxPoWList wrapped(list);for(int i=0;i<n;++i)list->push_back(txpow(i,mode));out=MiniData(wrapped.serialise()).to0xString();}
    else if(op=="list"){std::string hex;bool convert;in>>hex>>std::boolalpha>>convert;auto bytes=MiniData::fromHex(hex);size_t offset=0;auto list=std::make_shared<TxPoWList>(std::make_shared<TxPoWList::Rows>());list->mTxPoWs->push_back(txpow(2,0));try{if(convert)list=TxPoWList::convertMiniDataVersion(bytes);else list->readDataStream(bytes.bytes(),offset);out=list?MiniData(list->serialise()).to0xString():"null";}catch(const wire::IOException&){out="IO_ERROR";}catch(const std::exception&){out="ERROR";}if(!convert)out+="|"+std::to_string(offset)+"|"+std::to_string(list->mTxPoWs->size());}
    else if(op=="cache"){int n,minimum,mode;in>>n>>minimum>>mode;TxBlockDB db;std::vector<std::shared_ptr<TxBlock>> rows;for(int i=0;i<n;++i){auto b=std::make_shared<TxBlock>(TxPoW(key(i),i%37-5,1,true,"0x0"+std::to_string(i%5),false));rows.push_back(b);db.addTxBlock(b);if(mode&1)db.addTxBlock(b);}
        for(const auto& b:rows)out+=boolean(db.findTxBlock(b->getTxPoW().getTxPoWID())==b)+",";out+="|";for(int i=0;i<5;++i)out+=ids(db.getChildBlocks("0x0"+std::to_string(i)))+";";
        if(mode&2)for(int i=0;i<n;i+=3){auto& t=rows[i]->getTxPoW();t.setTimeMilli(MiniNumber::ZERO);t.setNonce(MiniNumber(i));t.calculateTXPOWID();}
        db.clearOld(MiniNumber(minimum));out+="|";for(int i=0;i<5;++i)out+=ids(db.getChildBlocks("0x0"+std::to_string(i)))+";";for(const auto& b:rows)out+=boolean(db.findTxBlock(b->getTxPoW().getTxPoWID())==b)+",";db.clearAll();out+="|";for(int i=0;i<5;++i)out+=std::to_string(db.getChildBlocks("0x0"+std::to_string(i)).size())+",";
    }else throw std::invalid_argument(op);std::cout<<out<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
