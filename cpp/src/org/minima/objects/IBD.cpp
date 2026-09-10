#include "IBD.hpp"
#include <iostream>
#include <set>
namespace minima {
const MiniNumber IBD::MAX_BLOCKS_FOR_IBD(34000);
IBDServices IBD::services;
namespace {
template<class T> T& required(const std::shared_ptr<T>& value){if(!value)throw std::invalid_argument("Null IBD dependency");return *value;}
const TxPoW& txpow(const std::shared_ptr<TxBlock>& block){return required(block).getTxPoW();}
void prependTree(std::vector<std::shared_ptr<TxBlock>>& blocks,std::shared_ptr<TxPoWTreeNode> tip){for(;tip;tip=tip->getParent())blocks.insert(blocks.begin(),tip->getSharedTxBlock());}
}
MiniNumber IBD::getTreeRoot() const {return txpow(blocks().at(0)).getBlockNumber();}
MiniNumber IBD::getTreeTip() const {return txpow(blocks().at(blocks().size()-1)).getBlockNumber();}
bool IBD::createIBD(const Greeting& greeting){
    auto tree=services.getTree();bool valid=true;if(!required(tree).getTip())return true;services.readLock(true);
    try {
        auto greetTip=greeting.getTopBlock(),greetRoot=greeting.getRootBlock();
        if(greetTip.isEqual(MiniNumber(-1)))createCompleteIBD();
        else {
            auto myRoot=required(tree->getRoot()).getBlockNumber();auto tip=tree->getTip();auto myTip=required(tip).getBlockNumber();
            if(greetTip.isLess(myRoot)){
                MiniNumber found(-1);size_t count=0;
                for(const auto& id:greeting.getChain()){if(count%20==0){found=required(services.getArchive()).exists(id.to0xString());if(!found.isEqual(MiniNumber(-1)))break;}++count;}
                if(found.isEqual(MiniNumber(-1))&&!greeting.getChain().empty())found=required(services.getArchive()).exists(greeting.getChain().back().to0xString());
                if(!found.isEqual(MiniNumber(-1))){auto total=myRoot.sub(found);if(!total.isMore(MAX_BLOCKS_FOR_IBD)){prependTree(blocks(),tip);for(const auto& block:required(services.getArchive()).loadBlockRange(found,myRoot,true))blocks().insert(blocks().begin(),block);}else{std::clog<<"Intersection found but User too far back to sync.. too many blocks:"<<total.toString()<<" max:34000\n";createCompleteIBD();}}
                else{std::clog<<"No Archive blocks found to match New User.. \n";createCompleteIBD();}
            }else if(greetRoot.isMore(myTip)){std::clog<<"We are Too old to sync new user! greetroot"<<greetRoot.toString()<<" mytip:"<<myTip.toString()<<'\n';createCompleteIBD();}
            else {
                std::set<std::string> mine;for(;tip;tip=tip->getParent())mine.insert(tip->getTxPoW().getTxPoWID());std::optional<std::string> found;
                for(const auto& id:greeting.getChain())if(mine.contains(id.to0xString())){found=id.to0xString();break;}
                if(found){for(tip=required(services.getTree()).getTip();tip;tip=tip->getParent()){if(tip->getTxPoW().getTxPoWID()==*found)break;blocks().insert(blocks().begin(),tip->getSharedTxBlock());}}
                else{std::clog<<"[!] When creating IBD - No Crossover found whilst syncing with new node. They are on a different chain. Please check you are on the correct chain\n";valid=false;createCompleteIBD();}
            }
        }
    }catch(const std::exception& error){std::clog<<error.what()<<'\n';}
    services.readLock(false);return valid;
}
void IBD::createCompleteIBD(){mCascade=std::make_shared<Cascade>(required(services.getCascade()).deepCopy());mTxBlocks=std::make_shared<Blocks>();prependTree(blocks(),required(services.getTree()).getTip());}
void IBD::createSyncIBD(const TxPoW& lastBlock){mCascade.reset();auto archive=services.getArchive();services.readLock(true);bool early=false;
    try {if(services.isShuttingDown())early=true;else{auto range=required(archive).loadSyncBlockRange(lastBlock.getBlockNumber());blocks().insert(blocks().end(),range.begin(),range.end());}}
    catch(const std::exception& error){std::clog<<error.what()<<'\n';}services.readLock(false);if(early)return;
}
void IBD::createArchiveIBD(const MiniNumber& firstBlock){createArchiveIBD(firstBlock,services.getArchive(),false);}
void IBD::createArchiveIBD(const MiniNumber& firstBlock,const std::shared_ptr<IBDArchive>& archive,bool useLocal){
    if(services.isShuttingDown())return;services.readLock(true);
    try {
        if(!services.isShuttingDown()){
            auto start=firstBlock;
            if(firstBlock.isZero()){auto root=required(archive).loadLastBlock();if(root&&!txpow(root).getBlockNumber().isEqual(MiniNumber::ONE)){mCascade=required(archive).loadCascade();if(mCascade)start=required(mCascade->getTip()).getTxPoW().getBlockNumber().increment();}}
            auto range=required(archive).loadBlockRange(start.decrement(),start.add(MiniNumber::TWOFIVESIX),false);blocks().insert(blocks().end(),range.begin(),range.end());
            if(!useLocal){auto root=required(services.getTree()).getRoot();auto rootBlock=required(root).getBlockNumber();auto request=firstBlock;if(rootBlock.isEqual(MiniNumber::ONE)&&request.isZero()){std::clog<<"Root Tree Archive IBD - starts at genesis\n";request=MiniNumber::ONE;}
                if(rootBlock.isEqual(request)){std::clog<<"Archive request for main tree data @ "<<request.toString()<<'\n';Blocks main;prependTree(main,required(services.getTree()).getTip());auto lastAdded=txpow(main.at(0)).getBlockNumber();if(lastAdded.isEqual(request))blocks().insert(blocks().end(),main.begin(),main.end());else std::clog<<"Archive main tree data error.. root:"<<lastAdded.toString()<<" req:"<<request.toString()<<'\n';}
            }
        }
    }catch(const std::exception& error){std::clog<<error.what()<<'\n';}services.readLock(false);
}
bool IBD::checkValidData() const {
    if(hasCascadeWithBlocks()){
        if(blocks().empty()){std::clog<<"[!] Received INVALID IBD no blocks.. with a cascade\n";return false;}
        auto cascadeTip=required(mCascade->getTip()).getTxPoW().getBlockNumber(),treeStart=getTreeRoot();
        if(!treeStart.isEqual(cascadeTip.increment())){std::clog<<"[!] Received INVALID IBD with cascade tip:"<<cascadeTip.toString()<<" and tree start:"<<treeStart.toString()<<'\n';return false;}
        if(!Cascade::checkCascadeCorrect(*mCascade)){std::clog<<"[!] INCONSISTENT Cascade received! length:"<<mCascade->getLength()<<'\n';return false;}
    }return true;
}
BigInt IBD::getTotalWeight() const {MMREntryNumber total;for(const auto& block:blocks())total=total.add(txpow(block).getWeight());auto cascadeWeight=mCascade?mCascade->getTotalWeight().toBigInteger():BigInt(0);return cascadeWeight+total.toBigInteger();}
std::vector<uint8_t> IBD::serialise() const {auto out=MiniByte(hasCascade()).serialise();if(mCascade)wire::append(out,mCascade->serialise());wire::append(out,MiniNumber(int64_t(blocks().size())).serialise());try{for(const auto& block:blocks())wire::append(out,required(block).serialise());}catch(const std::bad_alloc&){std::clog<<"OUT OF MEMORY on IBD size:"<<blocks().size()<<'\n';}return out;}
void IBD::readDataStream(std::span<const uint8_t> data,size_t& offset){if(MiniByte::deserialise(data,offset).isTrue()){mCascade=std::make_shared<Cascade>();mCascade->readDataStream(data,offset);}else mCascade.reset();mTxBlocks=std::make_shared<Blocks>();auto count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)blocks().push_back(std::make_shared<TxBlock>(TxBlock::deserialise(data,offset)));}
IBD IBD::deserialise(std::span<const uint8_t> data,size_t& offset){IBD ibd;ibd.readDataStream(data,offset);return ibd;}
bool IBD::checkOurChainHeavier(const IBD& other){IBD current;current.createCompleteIBD();return checkChains(current,other);}
bool IBD::checkChains(const IBD& current,const IBD& other){
    std::vector<std::string> otherBlocks,otherCascade;for(auto it=other.blocks().rbegin();it!=other.blocks().rend();++it)otherBlocks.push_back(txpow(*it).getTxPoWID());
    for(auto tip=required(other.mCascade).getTip();tip;tip=tip->getParent())otherCascade.push_back(tip->getTxPoW().getTxPoWID());
    std::set<std::string> myBlocks,myCascade;for(const auto& block:current.blocks())myBlocks.insert(txpow(block).getTxPoWID());for(auto tip=required(current.mCascade).getTip();tip;tip=tip->getParent())myCascade.insert(tip->getTxPoW().getTxPoWID());
    std::optional<std::string> found;for(const auto* list:{&otherBlocks,&otherCascade}){for(const auto* mine:{&myBlocks,&myCascade}){for(const auto& id:*list)if(mine->contains(id)){found=id;break;}if(found)break;}if(found)break;}
    auto myWeight=found?createShortenedIBD(current,*found).getTotalWeight():current.getTotalWeight();auto theirWeight=found?createShortenedIBD(other,*found).getTotalWeight():other.getTotalWeight();bool heavier=myWeight>=theirWeight;
    if(!heavier){std::clog<<(found?"[!] Crossover found on heavier chain : "+*found:"[!] NO crossover found on heavier chain..")<<"\nYOUR  WEIGHT from Crossover : "<<myWeight.toDecimalString()<<"\nTHEIR WEIGHT from Crossover : "<<theirWeight.toDecimalString()<<'\n';}return heavier;
}
IBD IBD::createShortenedIBD(const IBD& original,const std::string& id){
    IBD result;result.mCascade=std::make_shared<Cascade>();bool found=false;
    for(auto it=original.blocks().rbegin();it!=original.blocks().rend();++it){result.blocks().insert(result.blocks().begin(),*it);if(txpow(*it).getTxPoWID()==id){found=true;break;}}
    if(!found){std::vector<std::shared_ptr<CascadeNode>> nodes;for(auto tip=required(original.mCascade).getTip();tip;tip=tip->getParent()){nodes.push_back(tip);if(tip->getTxPoW().getTxPoWID()==id)break;}for(auto it=nodes.rbegin();it!=nodes.rend();++it)result.mCascade->addToTip((*it)->getTxPoW());result.mCascade->cascadeChain();}return result;
}
void IBD::printIBD(const IBD& ibd){std::cout<<"Cascade:\n"<<required(ibd.mCascade).printCascade()<<"\nBlocks:\n";for(const auto& block:ibd.blocks()){const auto& tx=txpow(block);std::cout<<tx.getBlockNumber().toString()<<") "<<tx.getTxPoWID()<<'\n';}std::cout<<"Total Weight : "<<ibd.getTotalWeight().toDecimalString()<<"\n\n";}
}
