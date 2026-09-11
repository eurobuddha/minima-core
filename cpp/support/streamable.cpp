#include "org/minima/utils/Streamable.hpp"
#include "org/minima/utils/sphincs/SPHINCSSignature.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/objects/mmr/MMREntry.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "support/MiniDataVersion.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "org/minima/objects/CoinProof.hpp"
#include "org/minima/objects/ScriptProof.hpp"
#include "org/minima/objects/Witness.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/system/brains/TxPoWChecker.hpp"
#include <iostream>
#include <type_traits>
using namespace minima;using namespace minima::json;
static_assert(std::is_abstract_v<Streamable> && std::has_virtual_destructor_v<Streamable>);
static_assert(std::is_base_of_v<Streamable,MiniByte> && std::is_base_of_v<Streamable,MMRProof>);
static_assert(std::is_base_of_v<Streamable,sphincs::horst::HORSTSignature> && std::is_base_of_v<Streamable,sphincs::fors::FORSSignature> && std::is_base_of_v<Streamable,sphincs::SPHINCSSignature>);
static_assert(std::is_base_of_v<Streamable,MMRData> && std::is_base_of_v<Streamable,MMREntryNumber> && std::is_base_of_v<Streamable,MMREntry> && std::is_base_of_v<Streamable,MMRProof::MMRProofChunk> && std::is_base_of_v<Streamable,SignatureProof> && std::is_base_of_v<Streamable,Signature>);
static_assert(std::is_base_of_v<Streamable,MiniData> && std::is_base_of_v<Streamable,MiniNumber> && std::is_base_of_v<Streamable,MiniString>);
static_assert(std::is_base_of_v<Streamable,StateVariable> && std::is_base_of_v<Streamable,Address> && std::is_base_of_v<Streamable,ScriptProof> && std::is_base_of_v<Streamable,Token> && std::is_base_of_v<Streamable,Coin> && std::is_base_of_v<Streamable,CoinProof>);
static_assert(std::is_base_of_v<Streamable,Witness> && std::is_base_of_v<Streamable,Transaction> && std::is_base_of_v<Streamable,TxBody>);
class FaultTransaction:public Transaction {
public:
    std::string fault;
    void writeDataStream(std::vector<uint8_t>& out) const override {
        if(fault.empty()){Transaction::writeDataStream(out);return;}
        out.push_back(42);
        if(fault=="io")throw wire::IOException("fixture");
        if(fault=="runtime")throw JavaIllegalArgumentException("fixture");
        throw JavaError("java.lang.AssertionError","fixture");
    }
};
class FaultWitness:public Witness {
public:
    void writeDataStream(std::vector<uint8_t>& out) const override {out.push_back(42);throw wire::IOException("fixture");}
};
class Failing:public Streamable {
    uint8_t values[3]={1,2,3};int fail;
public:
    explicit Failing(int n):fail(n){}
    void writeDataStream(std::vector<uint8_t>& out) const override{for(int i=0;i<3;++i){if(i==fail)throw wire::IOException("fixture write");out.push_back(values[i]);}if(fail==3)throw wire::IOException("fixture write");}
    void readDataStream(std::span<const uint8_t> bytes,size_t& offset) override{for(auto& value:values)value=wire::byte(bytes,offset);}
};
static std::string category(const std::exception& e){auto text=nativeExceptionText(e);return text.substr(0,text.find(": "));}
static std::unique_ptr<Streamable> create(const JSONObject& q){
    auto kind=q.getString("kind");
    if(kind=="body")return std::make_unique<TxBody>();
    if(kind=="transaction"){
        auto result=std::make_unique<FaultTransaction>();
        if(q.getString("op")=="badwrite"){
            auto bytes=MiniData::fromHex(q.getString("base"));size_t offset=0;result->readDataStream(bytes.bytes(),offset);auto fault=q.getString("fault");
            if(fault=="link")result->setLinkHash(MiniData(std::vector<uint8_t>(65)));
            else {JSONObject child=q;child.put("kind","coin");child.put("fault","coinid");auto coin=dynamic_cast<Coin&>(*create(child));if(fault=="input")result->addInput(coin);else result->addOutput(coin);}
        }
        return result;
    }
    if(kind=="witness"){
        auto result=std::make_unique<Witness>();
        if(q.getString("op")=="badwrite"){
            auto bytes=MiniData::fromHex(q.getString("base"));size_t offset=0;result->readDataStream(bytes.bytes(),offset);
            auto fault=q.getString("fault");JSONObject child=q;child.put("kind",fault);
            if(fault=="signature")result->getAllSignatures()->push_back(dynamic_cast<Signature&>(*create(child)));
            else if(fault=="coinproof"){child.put("fault","coinid");result->addCoinProof(dynamic_cast<CoinProof&>(*create(child)));}
            else result->getAllScripts()->push_back(dynamic_cast<ScriptProof&>(*create(child)));
        }
        return result;
    }
    if(q.getString("op")=="badwrite"&&(kind=="address"||kind=="scriptproof"||kind=="token"||kind=="coin"||kind=="coinproof")){
        MiniData bad(std::vector<uint8_t>(65));MMRProof proof(MiniNumber(7));proof.addProofChunk(true,MMRData(bad,MiniNumber(1)));
        if(kind=="address")return std::make_unique<Address>(bad);
        if(kind=="scriptproof")return std::make_unique<ScriptProof>("RETURN TRUE",proof);
        if(kind=="token")return std::make_unique<Token>(bad,MiniNumber(1),MiniNumber(1),MiniString("name"),MiniString("script"));
        auto fault=q.getString("fault");Coin coin(MiniData::fromHex("0x01"),fault=="address"?bad:MiniData::fromHex("0x02"),MiniNumber(3),MiniData::fromHex("0x00"));
        if(fault=="coinid")coin.resetCoinID(bad);if(fault=="tokenid")coin.resetTokenID(bad);if(fault=="state")coin.setSharedState(nullptr);
        if(fault=="token")coin.setToken(Token(bad,MiniNumber(1),MiniNumber(1),MiniString("name"),MiniString("script")));
        if(kind=="coin")return std::make_unique<Coin>(coin);return std::make_unique<CoinProof>(coin,fault=="proof"?proof:MMRProof());
    }
    if(kind=="state")return std::make_unique<StateVariable>(1,"[seed]");
    if(kind=="address")return std::make_unique<Address>("RETURN TRUE");
    if(kind=="scriptproof")return std::make_unique<ScriptProof>("RETURN TRUE");
    if(kind=="token")return std::make_unique<Token>(MiniData::fromHex("0x01"),MiniNumber(0),MiniNumber(1),MiniString("seed"),MiniString("RETURN TRUE"));
    if(kind=="coin")return std::make_unique<Coin>(MiniData::fromHex("0x00"),MiniNumber(0),Token::TOKENID_MINIMA);
    if(kind=="coinproof")return std::make_unique<CoinProof>(Coin(MiniData::fromHex("0x00"),MiniNumber(0),Token::TOKENID_MINIMA),MMRProof());
    if(q.getString("op")=="badwrite"){
        MMRData d(MiniData(std::vector<uint8_t>(65)),MiniNumber(7));MMRProof p(MiniNumber(7));p.addProofChunk(true,d);SignatureProof s(MiniData::fromHex("0xA1"),MiniData::fromHex("0xB2"),p);
        if(kind=="mmrdata")return std::make_unique<MMRData>(d);
        if(kind=="entry")return std::make_unique<MMREntry>(7,MMREntryNumber(4),d);
        if(kind=="chunk")return std::make_unique<MMRProof::MMRProofChunk>(true,d);
        if(kind=="signatureproof")return std::make_unique<SignatureProof>(s);
        auto result=std::make_unique<Signature>();result->addSignatureProof(s);return result;
    }
    if(kind=="data")return std::make_unique<MiniData>();
    if(kind=="number")return std::make_unique<MiniNumber>();
    if(kind=="string")return std::make_unique<MiniString>();
    if(kind=="byte")return std::make_unique<MiniByte>();
    if(kind=="proof")return std::make_unique<MMRProof>();
    if(kind=="mmrdata")return std::make_unique<MMRData>(MiniData(),MiniNumber());
    if(kind=="entrynumber")return std::make_unique<MMREntryNumber>();
    if(kind=="entry"){if(q.containsKey("nullData")&&q.getBoolean("nullData"))return std::make_unique<MMREntry>(7,MMREntryNumber(4),std::shared_ptr<MMRData>());return std::make_unique<MMREntry>(7,MMREntryNumber(4));}
    if(kind=="chunk")return std::make_unique<MMRProof::MMRProofChunk>(false,MMRData(MiniData(),MiniNumber()));
    if(kind=="signatureproof")return std::make_unique<SignatureProof>(MiniData(),MiniData(),MMRProof());
    if(kind=="signature")return std::make_unique<Signature>();
    if(kind=="horst")return std::make_unique<sphincs::horst::HORSTSignature>();
    if(kind=="fors")return std::make_unique<sphincs::fors::FORSSignature>();
    if(kind=="sphincs")return std::make_unique<sphincs::SPHINCSSignature>();
    if(kind=="failing")return std::make_unique<Failing>(int(std::get<int64_t>(q.get("fail").value)));
    throw std::invalid_argument("Unknown fixture kind");
}
static wire::Bytes data(const JSONObject& q,const std::string& name){return MiniData::fromHex(q.getString(name)).bytes();}
static JSONObject state(const Streamable& value){JSONObject out;wire::Bytes bytes={0xaa,0x55};try{value.writeDataStream(bytes);}catch(const std::exception& e){out.put("writeError",category(e));}out.put("appended",MiniData(bytes).to0xString());return out;}
class ReenterTransaction:public Transaction {
public:
    TxBody* parent=nullptr;std::vector<uint8_t>* completed=nullptr;
    void readDataStream(std::span<const uint8_t> bytes,size_t& offset) override {
        if(parent)parent->mTransaction()=std::make_shared<Transaction>();
        Transaction::readDataStream(bytes,offset);if(completed)*completed=serialise();
    }
};
static JSONObject bodyReenter(const JSONObject& q){
    TxBody body;auto initial=data(q,"initial");size_t offset=0;body.readDataStream(initial,offset);std::vector<uint8_t> completed;
    auto child=std::make_shared<ReenterTransaction>();static_cast<Transaction&>(*child)=*body.mTransaction();child->parent=&body;child->completed=&completed;body.mTransaction()=child;child.reset();
    auto wire=data(q,"wire");offset=0;JSONObject result;try{Streamable& object=body;object.readDataStream(wire,offset);}catch(const std::exception& e){result.put("readError",category(e));}
    result.put("offset",int64_t(offset));result.put("completedChild",MiniData(completed).to0xString());result.put("state",state(body));return result;
}
static JSONObject run(const JSONObject& q){
    if(q.getString("op")=="bodyReenter")return bodyReenter(q);
    JSONObject result;auto owner=create(q);Streamable& value=*owner;auto initial=data(q,"initial");size_t offset=0;if(q.getString("op")!="badwrite"&&(!q.containsKey("empty")||!q.getBoolean("empty")))value.readDataStream(initial,offset);if(auto d=dynamic_cast<MMRData*>(&value))d->setUnspendable(true);std::optional<Signature> alias;if(auto sig=dynamic_cast<Signature*>(&value))alias=*sig;auto oldList=alias?alias->getAllSignatureProofs():nullptr;auto op=q.getString("op");if(q.containsKey("cache")&&q.getBoolean("cache"))dynamic_cast<MiniData&>(value).getDataValue();
    std::optional<Witness> witnessAlias;std::shared_ptr<Witness::Signatures> oldSignatures;std::shared_ptr<Witness::CoinProofs> oldCoins;std::shared_ptr<Witness::Scripts> oldScripts;TxPoW containing;
    if(auto w=dynamic_cast<Witness*>(&value)){witnessAlias=*w;oldSignatures=w->getAllSignatures();oldCoins=w->getAllCoinProofs();oldScripts=w->getAllScripts();containing.setWitness(*w);}
    std::optional<Transaction> transactionAlias;std::shared_ptr<Transaction::Coins> oldInputs,oldOutputs;std::shared_ptr<Transaction::State> oldState;std::shared_ptr<MiniData> oldID,oldLink;
    if(auto t=dynamic_cast<Transaction*>(&value)){t->mHaveCheckedMonotonic()=true;t->mIsMonotonic()=true;t->mIsValid()=true;transactionAlias=*t;oldInputs=t->getAllInputs();oldOutputs=t->getAllOutputs();oldState=t->getCompleteState();oldID=t->getTransactionID();oldLink=t->getLinkHash();containing.setTransaction(*t);}
    std::optional<TxBody> bodyAlias;std::shared_ptr<TxBody> heldBody;std::shared_ptr<TxBody::IDs> oldBodyIDs;std::shared_ptr<MiniData> oldPRNG,oldDifficulty,oldBodyID,oldBodyTxnID,oldBodyBurnID;std::shared_ptr<Transaction> oldBodyTxn,oldBodyBurn;std::shared_ptr<Witness> oldBodyWitness,oldBodyBurnWitness;
    if(auto b=dynamic_cast<TxBody*>(&value)){
        if(q.containsKey("aliasChildren")){b->mBurnTransaction()=b->mTransaction();b->mBurnWitness()=b->mWitness();}
        bodyAlias=*b;*containing.getTxBody()=*b;heldBody=containing.getTxBody();oldBodyIDs=b->mTxPowIDList();oldPRNG=b->mPRNG();oldDifficulty=b->mTxnDifficulty();oldBodyID=oldBodyIDs->get(0);oldBodyTxn=b->mTransaction();oldBodyBurn=b->mBurnTransaction();oldBodyWitness=b->mWitness();oldBodyBurnWitness=b->mBurnWitness();oldBodyTxnID=oldBodyTxn->getTransactionID();oldBodyBurnID=oldBodyBurn->getTransactionID();
        if(q.containsKey("nullField")){auto field=q.getString("nullField");if(field=="prng")b->mPRNG().reset();else if(field=="difficulty")b->mTxnDifficulty().reset();else if(field=="transaction")b->mTransaction().reset();else if(field=="witness")b->mWitness().reset();else if(field=="burnTransaction")b->mBurnTransaction().reset();else if(field=="burnWitness")b->mBurnWitness().reset();else if(field=="ids")b->mTxPowIDList().reset();else b->mTxPowIDList()->set(1,nullptr);}
        if(q.containsKey("badField")){auto invalid=std::make_shared<MiniData>(std::vector<uint8_t>(65));if(q.getString("badField")=="prng")b->mPRNG()=invalid;else b->mTxPowIDList()->set(1,invalid);}
        if(q.containsKey("faultChild")){auto field=q.getString("faultChild");if(field=="transaction"||field=="burnTransaction"){auto child=std::make_shared<FaultTransaction>();static_cast<Transaction&>(*child)=TxBody::required(field=="transaction"?b->mTransaction():b->mBurnTransaction());child->fault="io";(field=="transaction"?b->mTransaction():b->mBurnTransaction())=child;}else {auto child=std::make_shared<FaultWitness>();static_cast<Witness&>(*child)=TxBody::required(field=="witness"?b->mWitness():b->mBurnWitness());(field=="witness"?b->mWitness():b->mBurnWitness())=child;}}
    }
    if(op=="read"){auto storage=data(q,"wire");std::span<const uint8_t> bytes=q.containsKey("selfInput")?std::span<const uint8_t>(dynamic_cast<MiniData&>(value).bytes()):std::span<const uint8_t>(storage);offset=size_t(std::get<int64_t>(q.get("offset").value));try{if(q.containsKey("hashRead"))dynamic_cast<MiniData&>(value).readHashFromStream(bytes,offset);else if(q.containsKey("expectedSize"))dynamic_cast<MiniData&>(value).readDataStream(bytes,offset,int(std::get<int64_t>(q.get("expectedSize").value)));else value.readDataStream(bytes,offset);}catch(const std::exception& e){result.put("readError",category(e));}result.put("offset",int64_t(offset));}
    else if(op=="callers"){
        result.put("data",miniDataVersion(value)->to0xString());result.put("hash",Crypto::getInstance().hashObject(value).to0xString());result.put("pairHash",Crypto::getInstance().hashObjects(value,value).to0xString());result.put("nio",NIOManager::createNIOMessage(MiniByte(13),value).to0xString());
        auto file=std::filesystem::path(q.getString("file"));try{MiniFile::writeObjectToFile(file,value);result.put("file",MiniData(MiniFile::readCompleteFile(file)).to0xString());MiniFile::writeObjectToFile(file,value,true);result.put("appendedFile",MiniData(MiniFile::readCompleteFile(file)).to0xString());MiniFile::writeDataToFile(file,data(q,"wire"));MiniFile::loadObject(file,value);}catch(...){std::filesystem::remove(file);throw;}std::filesystem::remove(file);
    }
    if(bodyAlias){auto& b=*bodyAlias;
        if(op=="bodyJSON")try{result.put("bodyJSON",b.toJSON());}catch(const std::exception& error){result.put("bodyJSONError",category(error));}
        JSONArray sameChildIDs;sameChildIDs.add(oldBodyTxnID==oldBodyTxn->getTransactionID());sameChildIDs.add(oldBodyBurnID==oldBodyBurn->getTransactionID());result.put("bodyOldChildIDSame",sameChildIDs);
        if(op=="bodyClearIDs")TxBody::required(b.mTxPowIDList()).clear();
        if(op=="bodyReplaceIDs")b.mTxPowIDList()=std::make_shared<TxBody::IDs>(std::vector<std::shared_ptr<MiniData>>{oldBodyID,nullptr,oldBodyID});
        if(op=="bodyMutateID"){auto bytes=MiniData::fromHex("0xABCD").serialise();size_t cursor=0;oldBodyID->readDataStream(bytes,cursor);}
        if(op=="bodyReset"){b.resetRandomPRNG();result.put("randomLength",int64_t(b.mPRNG()->getLength()));result.put("randomReplaced",b.mPRNG()!=oldPRNG);b.mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0xAA"));}
        if(op=="bodyClearOwner"){containing.clearBody();auto first=containing.getBlockTransactions(),second=containing.getBlockTransactions();first->add(oldBodyID);result.put("emptyListsDistinct",first!=second&&second->size()==0);}
        auto ids=[](const std::shared_ptr<TxBody::IDs>& list)->JSONValue{if(!list)return JSONValue();JSONArray result;for(const auto id:list->range())result.add(id?JSONValue(id->to0xString()):JSONValue());return result;};
        JSONArray same,children,oldChildren;for(bool sameField:{oldPRNG==b.mPRNG(),oldDifficulty==b.mTxnDifficulty(),oldBodyTxn==b.mTransaction(),oldBodyWitness==b.mWitness(),oldBodyBurn==b.mBurnTransaction(),oldBodyBurnWitness==b.mBurnWitness(),oldBodyIDs==b.mTxPowIDList()})same.add(sameField);
        for(const Streamable* child:{static_cast<Streamable*>(b.mTransaction().get()),static_cast<Streamable*>(b.mWitness().get()),static_cast<Streamable*>(b.mBurnTransaction().get()),static_cast<Streamable*>(b.mBurnWitness().get())})children.add(child?JSONValue(state(*child)):JSONValue());
        for(const Streamable* child:{static_cast<Streamable*>(oldBodyTxn.get()),static_cast<Streamable*>(oldBodyWitness.get()),static_cast<Streamable*>(oldBodyBurn.get()),static_cast<Streamable*>(oldBodyBurnWitness.get())})oldChildren.add(state(*child));
        result.put("bodySame",same);result.put("bodyChildren",children);result.put("bodyOldChildren",oldChildren);result.put("bodyIDs",ids(b.mTxPowIDList()));result.put("bodyOldIDs",ids(oldBodyIDs));result.put("bodyOldID",oldBodyID->to0xString());result.put("bodyOldPRNG",oldPRNG->to0xString());result.put("bodyOldDifficulty",oldDifficulty->to0xString());result.put("bodyAlias",state(b));result.put("heldBody",state(*heldBody));result.put("ownerHasBody",containing.hasBody());result.put("sameBlockList",containing.hasBody()&&containing.getBlockTransactions()==b.mTxPowIDList());
    }
    if(transactionAlias){
        auto& t=*transactionAlias;
        if(op=="transactionRemove")t.removeStateVariable(int(std::get<int64_t>(q.get("port").value)));
        if(op=="transactionAdd")t.addStateVariable(StateVariable(1,"[replacement]"));
        if(op=="transactionClear")t.clearState();
        if(op=="transactionFlags"){t.clearIsMonotonic();t.mIsValid()=false;}
        if(op=="transactionNullLink")t.setLinkHash(std::shared_ptr<MiniData>());
        try{if(op=="transactionFault"){dynamic_cast<FaultTransaction&>(value).fault=q.getString("fault");dynamic_cast<Transaction&>(value).calculateTransactionID();}else if(op=="transactionNullLink")t.calculateTransactionID();else if(op=="transactionJSON")t.toJSON();}catch(const std::exception& error){result.put("calculationError",category(error));}
        // Clear the artificial writer fault after observing the actual calculation path.
        dynamic_cast<FaultTransaction&>(value).fault.clear();
        if(q.containsKey("verifyMode")){
            auto mode=q.getString("verifyMode");Signature sig;SignatureProof proof(MiniData::fromHex("0xA1"),MiniData::fromHex("0xB2"),MMRProof());if(mode!="empty")sig.addSignatureProof(proof);if(mode=="excess")for(int i=1;i<9;++i)sig.addSignatureProof(proof);TreeKey key;key.setPublicKey(mode=="mismatch"?MiniData::fromHex("0x00"):proof.getRootPublicKey());
            try{bool verified;if(mode=="checkerEmpty"||mode=="checkerProof"){if(mode=="checkerProof")containing.getWitness().addSignature(sig);verified=TxPoWChecker::checkSignatures(containing);}else if(mode=="nullSignature")verified=key.verify(t.getTransactionID().get(),static_cast<const Signature*>(nullptr));else verified=key.verify(t.getTransactionID().get(),sig);result.put("verified",verified);}catch(const std::exception& error){result.put("verifyError",category(error));}
        }
        JSONArray same,counts,flags;for(bool b:{oldInputs==t.getAllInputs(),oldOutputs==t.getAllOutputs(),oldState==t.getCompleteState()})same.add(b);
        counts.add(int64_t(t.getAllInputs()->size()));counts.add(int64_t(t.getAllOutputs()->size()));counts.add(int64_t(t.getCompleteState()->size()));
        for(bool b:{t.mHaveCheckedMonotonic(),t.mIsMonotonic(),t.mIsValid()})flags.add(b);
        Transaction previous;*previous.getAllInputs()=*oldInputs;*previous.getAllOutputs()=*oldOutputs;*previous.getCompleteState()=*oldState;previous.setLinkHash(oldLink);
        result.put("oldTransaction",state(previous));result.put("transactionAlias",state(t));result.put("containingTransaction",state(containing.getTransaction()));
        result.put("sameTransactionLists",same);result.put("transactionCounts",counts);result.put("transactionFlags",flags);
        result.put("sameID",oldID==t.getTransactionID());result.put("sameLink",oldLink==t.getLinkHash());result.put("oldID",oldID->to0xString());result.put("transactionID",t.getTransactionID()?JSONValue(t.getTransactionID()->to0xString()):JSONValue());
        JSONArray ownerFlags,containingFlags;auto& ownerTransaction=dynamic_cast<Transaction&>(value);auto& contained=containing.getTransaction();for(bool b:{ownerTransaction.mHaveCheckedMonotonic(),ownerTransaction.mIsMonotonic(),ownerTransaction.mIsValid()})ownerFlags.add(b);for(bool b:{contained.mHaveCheckedMonotonic(),contained.mIsMonotonic(),contained.mIsValid()})containingFlags.add(b);result.put("ownerFlags",ownerFlags);result.put("containingFlags",containingFlags);result.put("stateSize",t.calculateStateSize());
    }
    if(witnessAlias){
        auto& w=*witnessAlias;
        if(op=="witnessClear"){w.clearSignatures();w.clearCoinProofs();w.clearScriptProofs();}
        if(op=="witnessDuplicates"){w.addSignature(oldSignatures->at(0));w.addCoinProof(oldCoins->at(0));w.addScript(oldScripts->at(0));auto keys=w.getAllSignatureKeys();keys.clear();}
        if(q.containsKey("clearHeld")){oldSignatures->clear();oldCoins->clear();oldScripts->clear();}
        JSONArray oldSizes,sameLists,currentSizes;oldSizes.add(int64_t(oldSignatures->size()));oldSizes.add(int64_t(oldCoins->size()));oldSizes.add(int64_t(oldScripts->size()));
        sameLists.add(oldSignatures==w.getAllSignatures());sameLists.add(oldCoins==w.getAllCoinProofs());sameLists.add(oldScripts==w.getAllScripts());
        currentSizes.add(int64_t(w.getAllSignatures()->size()));currentSizes.add(int64_t(w.getAllCoinProofs()->size()));currentSizes.add(int64_t(w.getAllScripts()->size()));
        Witness oldWitness;*oldWitness.getAllSignatures()=*oldSignatures;*oldWitness.getAllCoinProofs()=*oldCoins;*oldWitness.getAllScripts()=*oldScripts;result.put("oldState",state(oldWitness));
        result.put("oldSizes",oldSizes);result.put("sameLists",sameLists);result.put("currentSizes",currentSizes);result.put("witnessAlias",state(w));result.put("containingWitness",state(containing.getWitness()));
    }
    if(op=="tokenConvert"){try{auto token=Token::convertMiniDataVersion(MiniData(data(q,"wire")));result.put("converted",bool(token));}catch(const std::exception& e){result.put("convertError",category(e));}}
    if(auto a=dynamic_cast<Address*>(&value))result.put("derived",a->getMinimaAddress());if(auto p=dynamic_cast<ScriptProof*>(&value))result.put("derived",p->getAddressData().to0xString());if(auto t=dynamic_cast<Token*>(&value))result.put("derived",t->getTokenID()?JSONValue(t->getTokenID()->to0xString()):JSONValue(nullptr));if(auto s=dynamic_cast<StateVariable*>(&value))result.put("stateText",s->toString());
    if(auto e=dynamic_cast<MMREntry*>(&value))result.put("empty",e->isEmpty());if(auto d=dynamic_cast<MMRData*>(&value))result.put("unspendable",d->isUnspendable());if(alias){result.put("oldListSize",int64_t(oldList->size()));result.put("sameList",oldList==alias->getAllSignatureProofs());result.put("aliasCount",int64_t(alias->getAllSignatureProofs()->size()));}
    if(auto d=dynamic_cast<MiniData*>(&value)){result.put("numeric",d->getDataValue().toDecimalString());result.put("less128",d->isLess(MiniData::fromHex("0x80")));result.put("shiftLeft",d->shiftl(1).to0xString());result.put("shiftRight",d->shiftr(1).to0xString());}
    result.put("state",state(value));return result;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line)){JSONObject out;JSONArray logs;MinimaLogger::Runtime runtime;runtime.println=[&](const auto& line){auto s=unicode::encodeUTF8(line);auto pos=s.find("] : java.");if(pos!=s.npos){auto type=s.substr(pos+4);logs.add(type.substr(0,type.find(": ")));}};MinimaLogger::setRuntime(std::move(runtime));try{auto q=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);out=run(*q);}catch(const std::exception& e){out.put("error",category(e));out.put("message",e.what());}MinimaLogger::setRuntime({});out.put("logs",logs);std::cout<<out.toJSONString()<<'\n';}}
