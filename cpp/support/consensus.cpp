#include "org/minima/system/brains/TxPoWChecker.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/network/minima/RelayPolicy.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
static std::string str(bool b){return b?"true":"false";}
template<class F> static std::string result(F op){try {if constexpr(std::is_same_v<decltype(op()),bool>)return str(op());else return op();}catch(const std::exception&){return "ERROR";}}
template<class T> static T read(std::istream& in){std::string s;in>>s;auto bytes=BaseConverter::decode16(s);size_t offset=0;return T::deserialise(bytes,offset);}
static std::string flags(const TxPoW& t){const auto& a=t.getTransaction();const auto& b=t.getBurnTransaction();return str(a.mHaveCheckedMonotonic())+":"+str(a.mIsMonotonic())+":"+str(a.mIsValid())+":"+str(b.mHaveCheckedMonotonic())+":"+str(b.mIsMonotonic())+":"+str(b.mIsValid());}
static std::string generate(std::istream& in){
    auto mmr=read<MMR>(in);auto source=read<TxPoW>(in);int count,own,reject;std::string hash,time,max;in>>count>>own>>reject>>hash>>time>>max;
    TxPoW parent;parent.setBlockNumber(mmr.getBlockTime());parent.setTimeMilli(MiniNumber(time));parent.setBlockDifficulty(Magic::MIN_TXPOW_WORK);parent.getMagic().mCurrentMaxTxnPerBlock=MiniNumber(max);parent.getMagic().mDesiredMaxTxnPerBlock=MiniNumber(max);parent.getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x12"));for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i)parent.setSuperParent(i,MiniData::ZERO_TXPOWID);parent.setHeaderBodyHash();parent.calculateTXPOWID();
    auto tip=std::make_shared<TxPoWTreeNode>(parent);*tip->getMMR()=mmr;
    std::vector<std::shared_ptr<TxPoW>> pool;std::vector<std::string> removed;
    for(int i=0;i<count;++i){auto t=std::make_shared<TxPoW>(source.deepCopy());t->setNonce(MiniNumber(i));t->calculateTXPOWID();for(int j=0;j<reject;++j)t->incrementCheckRejectNumber();pool.push_back(t);}
    auto& svc=TxPoWGenerator::services;svc={};svc.getTip=[tip]{return tip;};svc.getRoot=[tip]{return tip;};svc.getHashRate=[hash]{return MiniNumber(hash);};svc.getMagicDesiredKISSVM=[]{return Magic::DEFAULT_KISSVM_OPERATIONS;};svc.getMagicMaxTxPoWSize=[]{return Magic::DEFAULT_TXPOW_SIZE;};svc.getMagicMaxTxns=[]{return Magic::DEFAULT_TXPOW_TXNS;};svc.getAllUnusedTxns=[pool]{return pool;};svc.removeMemPoolTxPoW=[&removed](const std::string& id){removed.push_back(id);};
    // Both test timestamps force Java's clock into a deterministic clamp branch.
    auto generated=own?TxPoWGenerator::generateTxPoWAt(source.getTransaction(),source.getWitness(),source.getBurnTransaction(),source.getBurnWitness(),MiniNumber("2000000000000")):TxPoWGenerator::generateTxPoWAt(Transaction{},Witness{},std::nullopt,std::nullopt,MiniNumber("2000000000000"));
    generated.getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x12"));generated.calculateTXPOWID();
    auto out=BaseConverter::encode16(generated.serialise())+"|"+str(TxPoWGenerator::isMempoolFull())+"|"+TxPoWGenerator::getMinMempoolBurn().toString()+"|";
    for(const auto& t:pool)out+=std::to_string(t->getCheckRejectNumber())+":"+flags(*t)+",";
    out+="|";for(const auto& id:removed)out+=id+",";svc={};return out;
}
int main(){std::cout<<std::unitbuf;GeneralParams::USE_SQL_COINDB=false;TxPoWChecker::services.getCascadeTip=[](){return std::shared_ptr<CascadeNode>{};};std::string line;while(std::getline(std::cin,line))try{
    std::istringstream in(line);std::string op,out;in>>op;
    if(op=="generate"){out=generate(in);}
    else if(op=="check"){
        auto mmr=read<MMR>(in);auto t=read<TxPoW>(in);auto b=read<TxPoW>(in);int max;int64_t state;in>>max>>state;GeneralParams::MAX_RELAY_OUTPUTCOINS=max;
        out=result([&]{return TxPoWChecker::checkTxPoWBasic(t);})+"|"+result([&]{return TxPoWChecker::checkSignatures(t);})+"|"+result([&]{return TxPoWChecker::checkMMR(mmr,t,false);})+"|"+result([&]{return RelayPolicy::checkAllPolicies(t,state);});
        out+="|"+result([&]{return TxPoWChecker::checkTxPoWScripts(mmr,t,b);});out+="|"+result([&]{return flags(t);});
        b.setBlockNumber(MiniNumber(21));out+="|"+result([&]{return TxPoWChecker::checkTxPoWScripts(mmr,t,b);});out+="|"+result([&]{return flags(t);});
        out+="|"+result([&]{return TxPoWChecker::checkTxPoWSimple(mmr,t,b,false);})+"|"+result([&]{TxPoWGenerator::precomputeTransactionCoinID(t.getTransaction());return BaseConverter::encode16(t.getTransaction().serialise());});
    }else if(op=="difficulty"){std::string n;in>>n;out=TxPoWGenerator::calculateDifficultyData(MiniNumber(n)).to0xString();}
    else if(op=="divide"){std::string a,b;int precision;in>>a>>b>>precision;out=MMREntryNumber::dividePositive(BigInt(a),BigInt(b),precision).toBigInteger().toDecimalString();}
    else if(op=="chain"){
        int back,count,mutate;in>>back>>count>>mutate;std::vector<std::shared_ptr<TxPoWTreeNode>> nodes;
        for(int i=0;i<count;++i){std::string height,time,difficulty;in>>height>>time>>difficulty;TxPoW t;t.setBlockNumber(MiniNumber(height));t.setTimeMilli(MiniNumber(time));t.setBlockDifficulty(MiniData::fromHex(difficulty));t.getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x12"));t.setHeaderBodyHash();t.calculateTXPOWID();auto n=std::make_shared<TxPoWTreeNode>(t);if(back!=-7)n->getMMR()->addEntry(MMRData::CreateMMRDataLeafNode(MiniNumber(1),MiniNumber(1)));if(i>0)nodes.back()->addChildNode(n);nodes.push_back(n);}
        auto tip=nodes.at(size_t(count-1));TxPoWGenerator::services.getRoot=[root=nodes.front()]{return root;};
        out=result([&]{return TxPoWGenerator::getMedianTimeBlock(tip,back)->getBlockNumber().toString();})+"|"+result([&]{return TxPoWGenerator::getChainSpeed(tip,MiniNumber(back)).toString();})+"|"+result([&]{return TxPoWGenerator::getBlockDifficulty(tip).to0xString();});
        TxPoW b;b.setBlockNumber(tip->getBlockNumber().increment());b.setTimeMilli(tip->getTxPoW().getTimeMilli());b.getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x12"));b.setMagic(tip->getTxPoW().getMagic().calculateNewCurrent());
        for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i)b.setSuperParent(i,MiniData::ZERO_TXPOWID);
        int level=0;for(int i=count-1;i>=0;--i)while(nodes[size_t(i)]->getTxPoW().getSuperLevel()>=level)b.setSuperParent(level++,nodes[size_t(i)]->getTxPoW().getTxPoWIDData());
        if(mutate>=0)b.setSuperParent(mutate,MiniData::fromHex("0x1234"));
        out+="|"+result([&]{return TxPoWChecker::checkParents(tip,b);});
        try{b.setBlockDifficulty(TxPoWGenerator::getBlockDifficulty(tip));}catch(const std::exception&){}
        out+="|"+result([&]{return TxPoWChecker::checkTxBlockOnly(tip,TxBlock(b));});
        out+="|"+result([&]{TxPoWTreeNode node(TxBlock(tip->getMMR(),b,{}),false);if(!node.getMMR()->getRoot()) throw std::invalid_argument("Empty MMR");b.setMMRRoot(node.getMMR()->getRoot()->getData());b.setMMRTotal(node.getMMR()->getRoot()->getValue());std::vector<TxPoW> txns;return TxPoWChecker::checkTxPoWBlock(tip,b,txns);});
        TxPoWGenerator::services.getRoot={};
    }else throw std::invalid_argument(op);
    std::cout<<out<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
