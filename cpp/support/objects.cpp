#include "BlockFixture.hpp"
#include "CoinFixture.hpp"
#include "org/minima/objects/Address.hpp"
#include "org/minima/objects/StateVariable.hpp"
#include "org/minima/objects/Token.hpp"
#include "org/minima/objects/mmr/MMRProof.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/objects/CoinProof.hpp"
#include "org/minima/objects/ScriptProof.hpp"
#include "org/minima/objects/mmr/MMREntry.hpp"
#include "org/minima/objects/Transaction.hpp"
#include "org/minima/objects/keys/Winternitz.hpp"
#include "org/minima/objects/keys/Signature.hpp"
#include "org/minima/objects/Witness.hpp"
#include "org/minima/objects/mmr/MMR.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/objects/TxHeader.hpp"
#include "org/minima/objects/TxBody.hpp"
#include "org/minima/objects/TxPoW.hpp"
#include "org/minima/objects/TxBlock.hpp"
#include "org/minima/objects/mmr/MegaMMR.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/objects/Greeting.hpp"
#include "org/minima/objects/Pulse.hpp"
#include "org/minima/system/genesis/GenesisCoin.hpp"
#include "org/minima/system/genesis/GenesisMMR.hpp"
#include "org/minima/system/genesis/GenesisTxPoW.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
static std::string str(const std::string& hex) {auto bytes=BaseConverter::decode16(hex);return MiniString(std::span<const uint8_t>(bytes)).toString();}
template<class T> static std::string result(const T& obj) {return BaseConverter::encode16(obj.serialise())+"|"+obj.toJSON().toJSONString();}
static std::string result(const MMREntryNumber& obj) {return BaseConverter::encode16(obj.serialise())+"|"+obj.toString();}
static std::string result(const TxPoW& t) {return BaseConverter::encode16(t.serialise())+"|"+(t.hasBody()?t.toJSON().toJSONString():"HEADER_ONLY")+"|"+t.getTxPoWID()+"|"+(t.isBlock()?"true":"false")+"|"+(t.isTransaction()?"true":"false")+"|"+std::to_string(t.getSuperLevel())+"|"+std::to_string(t.getSizeinBytes())+"|"+t.getWeight().getUnscaled().toDecimalString()+":"+std::to_string(t.getWeight().getScale());}
static std::string result(const TxBlock& block) {
    json::JSONArray inputs,outputs,removed;for(const auto& c:block.getInputCoinProofs()) inputs.add(c.toJSON());
    for(const auto& c:block.getOutputCoins()) {outputs.add(c.toJSON());auto state=block.removedState(c.getCoinID().to0xString());removed.add(state?json::JSONValue(Coin::convertStateListToJSON(*state)):json::JSONValue(nullptr));}
    return BaseConverter::encode16(block.serialise())+"|"+inputs.toJSONString()+"|"+outputs.toJSONString()+"|"+removed.toJSONString();
}
static std::string result(MegaMMR& mega) {auto bytes=BaseConverter::encode16(mega.serialise());json::JSONArray coins;for(const auto& c:mega.getAllCoins().elements()) coins.add(c->toJSON());return bytes+"|"+mega.getMMR().toJSON().toJSONString()+"|"+coins.toJSONString()+"|"+(mega.isEmpty()?"true":"false");}
static std::string result(const Greeting& greeting) {return BaseConverter::encode16(greeting.serialise())+"|"+greeting.getRootBlock().toString()+"|"+greeting.getExtraData().toJSONString();}
static std::string result(const Pulse& pulse) {json::JSONArray ids;for(const auto& id:pulse.getBlockList()) ids.add(id.to0xString());return BaseConverter::encode16(pulse.serialise())+"|"+ids.toJSONString();}
using minima::support::coinFixture;
static Coin coin(int n) { return coinFixture(n); }
using minima::support::fixedPow;
using minima::support::blockFixture;
static MegaMMR megaFixture(int size,int flags,int rounds) {
    MegaMMR mega;
    for(int r=0;r<rounds;++r) {
        auto pow=fixedPow(r);pow.setBlockNumber(MiniNumber(r+1));pow.setHeaderBodyHash();pow.calculateTXPOWID();TxBlock block(std::move(pow));block.getPreviousPeaks()=mega.getMMR().getPeaks();
        if(r>0) for(const auto& c:mega.getAllCoins().elements()) if(c->getMMREntryNumber().modulo(MMREntryNumber(3)).isEqual(MMREntryNumber(0))) block.getInputCoinProofs().emplace_back(*c,mega.getMMR().getProof(c->getMMREntryNumber()));
        for(int i=0;i<size;++i) {auto c=coin(i%32);c.resetCoinID(MiniData::fromHex(BaseConverter::numberToHex(r*1000+i+1)));if(i%3==0) c=Coin(c.getCoinID(),MiniData(std::vector<uint8_t>(32)),c.getAmount(),c.getTokenID());block.getOutputCoins().push_back(std::move(c));}
        mega.addBlock(block);if(flags&8) mega.getMMR().pruneTree();
    }
    return mega;
}
// Same operation sequence as ObjectOracle.coinAliases, using real native owners.
static std::string coinAliases(int n,int growth,int mode) {
    auto c=coin(n);auto alias=c;auto detached=c.deepCopy();
    auto renamed=c.getSameCoinWithCoinID(MiniData::fromHex("0xCAFE"));
    Transaction txn;txn.addInput(c);txn.addInput(c);txn.addOutput(c);
    CoinProof cp(c,MMRProof(MiniNumber(42)));Witness witness;witness.addCoinProof(cp);
    auto held=txn.getAllInputs()->at(0);
    for(int i=0;i<growth;++i) {txn.addInput(coin(i%32));witness.addCoinProof(CoinProof(coin(i%32),MMRProof()));}
    auto removed=txn.getAllInputs()->at(0);txn.getAllInputs()->erase(txn.getAllInputs()->begin());
    auto bytes=cp.serialise();size_t offset=0;auto decoded=CoinProof::deserialise(bytes,offset);
    Coin* target=nullptr;
    switch(mode) {
        case 0:target=&c;break;case 1:target=&alias;break;case 2:target=&held;break;case 3:target=&removed;break;
        case 4:target=&txn.getAllInputs()->at(0);break;case 5:target=&txn.getAllOutputs()->at(0);break;
        case 6:target=&cp.getCoin();break;case 7:target=&witness.getAllCoinProofs()->at(0).getCoin();break;
        case 8:target=&detached;break;case 9:target=&renamed;break;case 10:target=&decoded.getCoin();break;
        default:throw std::invalid_argument("Invalid coin alias mode");
    }
    target->resetCoinID(MiniData::fromHex("0xAABB"));target->resetTokenID(MiniData::fromHex("0xCC"));
    target->setSpent(!target->getSpent());target->setStoreState(!target->storeState());
    target->setBlockCreated(MiniNumber("12345678901234567890"));
    target->setMMREntryNumber(MMREntryNumber(BigInt("1267650600228229401496703205376")));
    target->setToken(std::nullopt);target->getState().emplace_back(255,"[shared mutation]");
    auto first=result(c)+"|"+result(alias)+"|"+result(held)+"|"+result(removed)
        +"|"+result(txn.getAllInputs()->at(0))+"|"+result(txn.getAllOutputs()->at(0))
        +"|"+result(cp)+"|"+result(witness.getAllCoinProofs()->at(0))+"|"+result(cp.getMMRData())
        +"|"+result(detached)+"|"+result(renamed)+"|"+result(decoded);
    txn.getAllInputs()->clear();txn.getAllOutputs()->clear();witness.clearCoinProofs();
    held.setState({});held.setToken(coin(1).getToken());
    alias=coin((n+1)%32);
    return first+"|"+result(c)+"|"+result(cp)+"|"+result(removed)+"|"+result(alias)
        +"|"+result(detached)+"|"+result(renamed)+"|"+result(decoded);
}
int main() {
    std::string line;
    while(std::getline(std::cin,line)) {
        try {
            std::istringstream in(line);std::vector<std::string> a;std::string word;while(in>>word) a.push_back(word);
            auto cmd=a.at(0);std::string out;
            if(cmd=="genesis") {GenesisTxPoW genesis(a.at(1));genesis.setTimeMilli(MiniNumber(1788883200000LL));genesis.getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x000102"));genesis.setHeaderBodyHash();genesis.calculateTXPOWID();out=result(static_cast<const TxPoW&>(genesis))+"|"+result(GenesisCoin())+"|"+result(GenesisMMR());}
            else if(cmd=="greeting") {Greeting g;g.setTopBlock(MiniNumber(a.at(1)));g.getExtraData().put("welcome",str(a.at(3)));for(int i=0;i<std::stoi(a.at(2));++i) g.getChain().push_back(MiniData::fromHex(BaseConverter::numberToHex(i+1)));out=result(g);}
            else if(cmd=="pulse") {Pulse p;for(int i=0;i<std::stoi(a.at(1));++i) p.getBlockList().push_back(MiniData::fromHex(BaseConverter::numberToHex(i+1)));out=result(p);}
            else if(cmd=="txblock") out=result(blockFixture(std::stoi(a.at(1)),std::stoi(a.at(2)),std::stoi(a.at(3))));
            else if(cmd=="mega") {
                int flags=std::stoi(a.at(2));GeneralParams::MEGAMMR_MEGAPRUNE=flags&1;GeneralParams::MEGAMMR_MEGAPRUNE_STATE=flags&2;GeneralParams::MEGAMMR_MEGAPRUNE_TOKENS=flags&4;
                auto mega=megaFixture(std::stoi(a.at(1)),flags,std::stoi(a.at(3)));out=result(mega);
            } else if(cmd=="pow") {
                TxPoW t;t.setTimeMilli(MiniNumber(1788883200000LL));t.setNonce(MiniNumber(a.at(1)));t.setBlockDifficulty(MiniData::fromHex(a.at(2)));t.getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x000102"));
                int mode=std::stoi(a.at(3));if(mode==1) t.getTransaction().addInput(coin(0));else if(mode==2) t.getBurnTransaction().addInput(coin(0));t.addBlockTxPOW(MiniData::fromHex("0x1234"));t.setHeaderBodyHash();if(mode==3) t.clearBody();t.calculateTXPOWID();out=result(t);
            } else if(cmd=="magic") {Magic magic;magic.setDesiredMaxTxPoWSize(MiniNumber(a.at(1)));magic.setDesiredKISSVM(MiniNumber(a.at(2)));magic.setDesiredMaxTxns(MiniNumber(a.at(3)));magic.mDesiredMinTxPoWWork=MiniData::fromHex(a.at(4));out=result(magic)+"|"+result(magic.calculateNewCurrent());}
            else if(cmd=="body") {TxBody body;body.mPRNG()=std::make_shared<MiniData>(MiniData::fromHex(a.at(1)));int count=std::stoi(a.at(2));for(int i=0;i<count;++i) {body.mTxPowIDList()->add(std::make_shared<MiniData>(MiniData::fromHex(BaseConverter::numberToHex(i+1))));body.mTransaction()->addInput(coin(i));}out=result(body);}
            else if(cmd=="header") {TxHeader h;h.mTimeMilli=MiniNumber(a.at(1));int run=std::stoi(a.at(2));for(size_t i=0;i<h.mSuperParents.size();++i) if(run>0) h.mSuperParents[i]=MiniData::fromHex(BaseConverter::numberToHex(int(i)/run));h.mNonce=MiniNumber(900000);h.mBlockNumber=MiniNumber(123450);h.mTxBodyHash=Crypto::getInstance().hashObject(MiniData::fromHex("0x1234"));out=result(h);}
            else if(cmd=="treekey") {
                TreeKey key(MiniData::fromHex(a.at(1)),std::stoi(a.at(2)),std::stoi(a.at(3)));key.setUses(std::stoi(a.at(4)));auto data=MiniData::fromHex(a.at(6));out=key.getPublicKey()->to0xString()+"|"+std::to_string(key.getMaxUses());
                // ObjectOracle captures signing logs separately from its wire protocol.
                key.setLogger([](const std::string&){});
                for(int i=0;i<std::stoi(a.at(5));++i) {auto sig=key.sign(data);out+="|"+std::to_string(key.getUses())+"|"+BaseConverter::encode16(sig.serialise())+"|"+(key.verify(data,sig)?"true":"false");}
            } else if(cmd=="treeverify") {
                TreeKey key;key.setPublicKey(MiniData::fromHex(a.at(1)));auto bytes=BaseConverter::decode16(a.at(3));size_t offset=0;auto sig=Signature::deserialise(bytes,offset);out=key.verify(MiniData::fromHex(a.at(2)),sig)?"true":"false";
            } else if(cmd=="mmr") {
                int n=std::stoi(a.at(1));auto mode=a.at(2);auto tree=std::make_shared<MMR>();
                for(int i=0;i<n;++i) {auto leaf=MMRData::CreateMMRDataLeafNode(MiniString("leaf"+std::to_string(i)),MiniNumber(i%3==0?0:i+1));leaf.setUnspendable(i%2==0);tree->addEntry(std::move(leaf));}
                std::string checks;
                if(mode=="prune") tree->pruneTree();else if(mode=="scan") tree->scanUnspendableTree();
                else if(mode=="child") {
                    auto parent=tree;tree=std::make_shared<MMR>(parent);tree->addEntry(MMRData::CreateMMRDataLeafNode(MiniString("new"),MiniNumber(7)));
                    if(n>0) {MMREntryNumber number(n/2);auto leaf=parent->getEntry(0,number)->getMMRData();auto proof=parent->getProof(number);checks=std::string("|")+(tree->checkProofTimeValid(number,*leaf,proof)?"true":"false");tree->updateEntry(number,proof,MMRData(MiniData::fromHex("0x00"),MiniNumber(0)));checks+=std::string("|")+(tree->checkProofTimeValid(number,*leaf,proof)?"true":"false");}
                }
                json::JSONArray proofs;if(mode=="normal") for(int i=0;i<n;++i) {MMREntryNumber number(i);auto proof=tree->getProof(number);json::JSONObject row;row.put("proof",proof.toJSON());row.put("valid",tree->checkProof(*tree->getEntry(0,number)->getMMRData(),proof));proofs.add(row);}
                out=result(*tree)+"|"+proofs.toJSONString()+checks;
            } else if(cmd=="witness") {
                Witness witness;for(int i=0;i<std::stoi(a.at(1));++i) {
                    Signature sig;sig.addSignatureProof(SignatureProof(MiniData::fromHex(BaseConverter::numberToHex(i+1)),MiniData::fromHex("0x1234"),MMRProof()));
                    witness.addSignature(sig);witness.addSignature(sig);witness.addCoinProof(CoinProof(coin(i),MMRProof(MiniNumber(i))));witness.addScript(ScriptProof("RETURN TRUE"));witness.addScript(ScriptProof("RETURN TRUE"));
                }out=result(witness);
            } else if(cmd=="wots") {Winternitz key(MiniData::fromHex(a.at(1)));auto message=MiniData::fromHex(a.at(2));auto sig=key.sign(message);out=key.getPublicKey()->to0xString()+"|"+sig.to0xString()+"|"+(Winternitz::verify(*key.getPublicKey(),message,sig)?"true":"false");}
            else if(cmd=="verify") out=Winternitz::verify(MiniData::fromHex(a.at(1)),MiniData::fromHex(a.at(2)),MiniData::fromHex(a.at(3)))?"true":"false";
            else if(cmd=="sigproof") out=result(SignatureProof(MiniData::fromHex(a.at(1)),MiniData::fromHex(a.at(2)),MMRProof(MiniNumber(17))));
            else if(cmd=="signature") {Signature sig;for(int i=0;i<std::stoi(a.at(1));++i) sig.addSignatureProof(SignatureProof(MiniData::fromHex(BaseConverter::numberToHex(i+1)),MiniData::fromHex("0x123456"),MMRProof(MiniNumber(i))));out=result(sig);}
            else if(cmd=="txn") {
                Transaction txn;
                if(a.at(3)!="-") {Coin input(MiniData::fromHex("0x01"),MiniData::fromHex("0x02"),MiniNumber(a.at(1)),MiniData::fromHex(a.at(3)));txn.addInput(input);if(a.at(5)=="1") txn.addInput(input);}
                if(a.at(4)!="-") txn.addOutput(Coin(MiniData::fromHex("0x02"),MiniNumber(a.at(2)),MiniData::fromHex(a.at(4))));
                txn.addStateVariable(StateVariable(255,"TRUE"));txn.addStateVariable(StateVariable(0,"[first]"));txn.addStateVariable(StateVariable(128,"1.00"));txn.addStateVariable(StateVariable(0,"[replacement]"));
                // Match ObjectOracle's scoped suppression of transaction validation logs.
                bool valid;{MinimaLogger::Runtime runtime;runtime.println=[](const std::u16string&){};MinimaLogger::setRuntime(std::move(runtime));
                    struct Reset {~Reset(){MinimaLogger::setRuntime({});}} reset;valid=txn.checkValid();}
                out=result(txn)+"|"+(valid?"true":"false")+"|"+txn.sumInputs().toString()+"|"+txn.sumOutputs().toString()+"|"+txn.getBurn().toString()+"|"+std::to_string(txn.calculateStateSize())+"|"+txn.calculateCoinID(MiniData::fromHex("0xFF"),7).to0xString();
            } else if(cmd=="entry") {
                MMREntryNumber n(BigInt(a.at(1)),std::stoi(a.at(2)));
                out=result(n)+"|"+result(n.floor())+"|"+result(n.increment())+"|"+result(n.decrement())+"|"+result(n.div2())+"|"+result(n.mult2())+"|"+result(n.modulo(MMREntryNumber(2)));
            } else if(cmd=="mmrentry") {
                MMREntry entry(std::stoi(a.at(1)),MMREntryNumber(BigInt(a.at(2))),MMRData(MiniData::fromHex("0x01"),MiniNumber("7.25")));
                out=result(entry)+"|"+entry.getSibling().toString()+"|"+entry.getParentEntry().toString()+"|"+entry.getLeftChildEntry().toString()+"|"+entry.getRightChildEntry().toString()+"|"+(entry.isLeft()?"true":"false");
            } else if(cmd=="coinalias") out=coinAliases(std::stoi(a.at(1)),std::stoi(a.at(2)),std::stoi(a.at(3)));
            else if(cmd=="coin") {auto c=coin(std::stoi(a.at(1)));out=result(c)+"|"+c.toJSON(true).toJSONString()+"|"+result(c.deepCopy());}
            else if(cmd=="coinproof") {CoinProof c(coin(std::stoi(a.at(1))),MMRProof(MiniNumber(42)));out=result(c)+"|"+result(c.getMMRData());}
            else if(cmd=="scriptproof") out=result(ScriptProof(str(a.at(1))));
            else if(cmd=="address") out=result(Address(str(a.at(1))));
            else if(cmd=="mx") out=Address::makeMinimaAddress(MiniData::fromHex(a.at(1)));
            else if(cmd=="unmx") out=Address::convertMinimaAddress(a.at(1)).to0xString();
            else if(cmd=="b32") out=BaseConverter::encode32(BaseConverter::decode16(a.at(1)));
            else if(cmd=="unb32") out=BaseConverter::encode16(BaseConverter::decode32(a.at(1)));
            else if(cmd=="state") out=result(StateVariable(std::stoi(a.at(1)),str(a.at(2))));
            else if(cmd=="token") out=result(Token(MiniData::fromHex(a.at(1)),MiniNumber(a.at(2)),MiniNumber(a.at(3)),MiniString(str(a.at(4))),MiniString(str(a.at(5))),MiniNumber(a.at(6))));
            else if(cmd=="leaf") out=result(MMRData::CreateMMRDataLeafNode(MiniString(str(a.at(1))),MiniNumber(a.at(2))));
            else if(cmd=="proof") {
                MMRProof proof{MiniNumber(a.at(1))};auto leaf=MMRData::CreateMMRDataLeafNode(MiniString(str(a.at(2))),MiniNumber(a.at(3)));
                for(size_t i=4;i<a.size();i+=3) proof.addProofChunk(a.at(i)=="1",MMRData::CreateMMRDataLeafNode(MiniString(str(a.at(i+1))),MiniNumber(a.at(i+2))));
                out=result(proof)+"|"+result(proof.calculateProof(leaf));
            } else if(cmd=="read") {
                auto data=BaseConverter::decode16(a.at(2));size_t offset=0;
                if(a.at(1)=="greeting") out=result(Greeting::deserialise(data,offset));
                else if(a.at(1)=="pulse") {auto saved=MinimaLogger::getRuntime();struct Reset{MinimaLogger::Runtime previous;~Reset(){MinimaLogger::setRuntime(std::move(previous));}} reset{saved};saved.println=[](const std::u16string&){};MinimaLogger::setRuntime(std::move(saved));out=result(Pulse::deserialise(data,offset));}
                else if(a.at(1)=="txblock") out=result(TxBlock::deserialise(data,offset));
                else if(a.at(1)=="mega") {auto mega=MegaMMR::deserialise(data,offset);out=result(mega);}
                else if(a.at(1)=="pow") out=result(TxPoW::deserialise(data,offset));
                else if(a.at(1)=="magic") out=result(Magic::deserialise(data,offset));
                else if(a.at(1)=="header") out=result(TxHeader::deserialise(data,offset));
                else if(a.at(1)=="body") out=result(TxBody::deserialise(data,offset));
                else if(a.at(1)=="mmr") out=result(MMR::deserialise(data,offset));
                else if(a.at(1)=="witness") out=result(Witness::deserialise(data,offset));
                else if(a.at(1)=="sigproof") out=result(SignatureProof::deserialise(data,offset));
                else if(a.at(1)=="signature") out=result(Signature::deserialise(data,offset));
                else if(a.at(1)=="txn") out=result(Transaction::deserialise(data,offset));
                else if(a.at(1)=="entry") out=result(MMREntryNumber::deserialise(data,offset));
                else if(a.at(1)=="mmrentry") out=result(MMREntry::deserialise(data,offset));
                else if(a.at(1)=="coin") out=result(Coin::deserialise(data,offset));
                else if(a.at(1)=="coinproof") out=result(CoinProof::deserialise(data,offset));
                else if(a.at(1)=="scriptproof") out=result(ScriptProof::deserialise(data,offset));
                else if(a.at(1)=="address") out=result(Address::deserialise(data,offset));
                else if(a.at(1)=="state") out=result(StateVariable::deserialise(data,offset));
                else if(a.at(1)=="token") out=result(Token::deserialise(data,offset));
                else if(a.at(1)=="leaf") out=result(MMRData::deserialise(data,offset));
                else out=result(MMRProof::deserialise(data,offset));
                out+="|"+std::to_string(data.size()-offset);
            } else if(cmd=="json") {
                json::JSONObject obj;obj.put("text",str(a.at(1)));obj.put("flag",true);obj.put("null",nullptr);json::JSONArray arr;arr.add(17);arr.add(false);arr.add(str(a.at(2)));obj.put("array",arr);obj.put("text",str(a.at(1)));out=obj.toJSONString();
            } else throw std::invalid_argument("Unknown command");
            std::cout<<out<<'\n';
        } catch(const std::exception&) {std::cout<<"ERROR\n";}
    }
}
