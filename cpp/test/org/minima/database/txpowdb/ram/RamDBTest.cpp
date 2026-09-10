// Adapted from the nonempty cases in upstream RamDBTest.java.
#include "org/minima/database/txpowdb/ram/RamDB.hpp"
#include <stdexcept>
using namespace minima;
static void require(bool value){if(!value)throw std::runtime_error("RamDBTest assertion");}
int main(){
    RamDB db;require(db.getSize()==0);
    for(int i=0;i<3;++i){auto txpow=std::make_shared<TxPoW>("0x00",0,1);require(db.addTxPoW(txpow)==(i==0));require(db.getSize()==1);}
    db.addTxPoW(std::make_shared<TxPoW>("0x01",0,1));require(db.getSize()==2);
    db.addTxPoW(std::make_shared<TxPoW>("0x02",0,1));require(db.getSize()==3);
    for(auto id:{"0x00","0x01","0x02"})require(bool(db.getTxPoW(id)));
    require(!db.getTxPoW("0x10"));
    auto original=db.getCompleteMemPool();db.MAX_TIME=0;db.cleanDBAt(INT64_MAX);require(db.getSize()==0&&original->size()==3);
}
