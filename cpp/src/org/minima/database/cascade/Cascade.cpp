#include "org/minima/utils/MiniFile.hpp"
#include "Cascade.hpp"
#include "support/JavaIOExceptionText.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include <bit>
#include <iostream>
namespace minima {
void Cascade::addToTip(const TxPoW& txpow) {auto node=std::make_shared<CascadeNode>(txpow);node->setParent(mTip);mTip=std::move(node);}
int32_t Cascade::getLength() const {uint32_t length=0;for(auto current=mTip;current;current=current->getParent()) ++length;return std::bit_cast<int32_t>(length);}
void Cascade::cascadeChain() {
    if(!mTip) return;
    auto newcascade=mTip,current=mTip->getParent();mTotalWeight=newcascade->getCurrentWeight();
    int32_t level=0,count=1;
    while(current) {
        if(current->getSuperLevel()>=level) {
            current->setLevel(level);newcascade->setParent(current);newcascade=current;
            mTotalWeight=mTotalWeight.add(newcascade->getCurrentWeight());
            count=std::bit_cast<int32_t>(uint32_t(count)+1u);
            if(count>=GlobalParams::MINIMA_CASCADE_LEVEL_NODES && level<GlobalParams::MINIMA_CASCADE_LEVELS-1) {++level;count=0;}
        }
        current=current->getParent();
    }
    newcascade->setParent(nullptr);
}
Cascade Cascade::deepCopy() const {auto bytes=serialise();size_t offset=0;return deserialise(bytes,offset);}
std::string Cascade::printCascade() const {
    std::vector<std::shared_ptr<CascadeNode>> nodes;for(auto current=mTip;current;current=current->getParent()) nodes.push_back(current);
    std::string result;for(auto it=nodes.rbegin();it!=nodes.rend();++it) result+=(*it)->toString()+"\n";return result;
}
std::vector<uint8_t> Cascade::serialise() const {
    auto bytes=MiniNumber(getLength()).serialise();for(auto current=mTip;current;current=current->getParent()) wire::append(bytes,current->serialise());return bytes;
}
void Cascade::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    int32_t length=MiniNumber::deserialise(data,offset).getAsInt();mTip.reset();mTotalWeight=MMREntryNumber(0);
    std::shared_ptr<CascadeNode> current;
    for(int32_t i=0;i<length;++i) {auto node=std::make_shared<CascadeNode>(CascadeNode::deserialise(data,offset));if(current) current->setParent(node);else mTip=node;current=node;}
    cascadeChain();
}
Cascade Cascade::deserialise(std::span<const uint8_t> data,size_t& offset) {Cascade cascade;cascade.readDataStream(data,offset);return cascade;}
std::optional<Cascade> Cascade::convertMiniDataVersion(const MiniData& data) {
    return convertMiniDataVersion(data,[](const wire::IOException& error){std::clog<<javaIOExceptionText(error)<<'\n';});
}
std::optional<Cascade> Cascade::convertMiniDataVersion(const MiniData& data,const std::function<void(const wire::IOException&)>& log) {
    try {size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException& error) {log(error);return std::nullopt;}
}
bool Cascade::checkPastNodeExists(std::shared_ptr<CascadeNode> node,const std::string& id) {for(;node;node=node->getParent()) if(node->getTxPoW().getTxPoWID()==id) return true;return false;}
bool Cascade::checkCascadeCorrect(const Cascade& cascade) {
    return checkCascadeCorrect(cascade,[](const std::string& text){std::clog<<text<<'\n';});
}
bool Cascade::checkCascadeCorrect(const Cascade& cascade,const std::function<void(const std::string&)>& log) {
    auto current=cascade.getTip();if(current && current->getLevel()!=0) {log("Cascade does not start at level 0.. "+std::to_string(current->getLevel()));return false;}
    int32_t oldlevel=0,counter=0;
    for(;current;current=current->getParent(),counter=std::bit_cast<int32_t>(uint32_t(counter)+1u)) {
        int32_t level=current->getLevel();if(level!=oldlevel) {
            if(level!=oldlevel+1) {log("NEXT level up is not a single increment @ clevel:"+std::to_string(level)+" oldlevel:"+std::to_string(oldlevel));return false;}
            counter=0;oldlevel=level;
        }
        auto parent=current->getParent();bool last=!parent || parent->getLevel()!=level,foundzero=false;
        for(int32_t i=level;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i) {
            auto id=current->getTxPoW().getSuperParent(i).to0xString();
            if(id=="0x00") foundzero=true;
            else {
                auto suffix=" @ slevel "+std::to_string(i)+"/"+std::to_string(level)+" counter:"+std::to_string(counter);
                if(foundzero) {log("NON zero node found in cascade after first zero node.."+id+suffix);return false;}
                if((!last || i>level) && !checkPastNodeExists(current,id)) {log("Parent not found in cascade.. "+id+suffix);return false;}
            }
        }
    }
    return true;
}
}

namespace minima {
void Cascade::loadDB(const std::filesystem::path& file) {MiniFile::loadObjectSlow(file,*this);}
void Cascade::saveDB(const std::filesystem::path& file) {MiniFile::saveObjectDirect(file,*this);}
}
