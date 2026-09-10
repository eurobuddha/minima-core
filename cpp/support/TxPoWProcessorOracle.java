import java.io.*;
import java.util.*;
import org.minima.database.*;
import org.minima.database.archive.*;
import org.minima.database.cascade.*;
import org.minima.database.txpowdb.*;
import org.minima.database.txpowdb.ram.*;
import org.minima.database.txpowdb.onchain.*;
import org.minima.database.txpowtree.*;
import org.minima.database.wallet.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.system.Main;
import org.minima.system.brains.*;
import org.minima.system.network.*;
import org.minima.system.network.minima.*;
import org.minima.system.params.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
// Actual pinned processor/checker/tree/MMR. Storage and external receivers are
// controlled, as in NIOMessageOracle/IBDOracle. No node or worker constructors.
public class TxPoWProcessorOracle extends NIOClientOracle {
    static JSONArray events;static int calls,failAt;static boolean shutdown,restore,relevant,sync;
    static boolean flag(JSONObject o,String key){return Boolean.TRUE.equals(o.get(key));}
    static int num(JSONObject o,String key,int fallback){return o.containsKey(key)?((Number)o.get(key)).intValue():fallback;}
    static long number(JSONObject o,String key,long fallback){return o.containsKey(key)?((Number)o.get(key)).longValue():fallback;}
    static void event(String name,Object... args){var row=new JSONArray();row.add(name);row.addAll(Arrays.asList(args));events.add(row);if(++calls==failAt)throw new RuntimeException("fixture receiver failure");}
    static class OnChain extends TxPoWOnChainDB {public boolean addOnChainTxPoW(String block,MiniNumber number,String tx){event("onChainDB",block,number.toString(),tx);return true;}}
    static class Pool extends TxPoWDB {
        RamDB ram;ArrayList<TxPoW> order=new ArrayList<>();OnChain onchain=new OnChain();
        Pool()throws Exception{ram=(RamDB)get(this,"mRamDB");}
        public boolean addTxPoW(TxPoW tx){event("add",tx.getTxPoWID());if(!ram.exists(tx.getTxPoWID())){ram.addTxPoW(tx);order.add(tx);}return relevant;}
        public TxPoW getTxPoW(String id){event("get",id);return ram.getTxPoW(id);}
        public boolean exists(String id){event("exists",id);return ram.exists(id);}
        public ArrayList<TxPoW> getChildBlocks(String id){event("children",id);var out=new ArrayList<TxPoW>();for(var tx:order)if(tx.isBlock()&&tx.getParentID().to0xString().equals(id))out.add(tx);return out;}
        public void clearMainChainTxns(){event("clearChain");super.clearMainChainTxns();}
        public void setOnMainChain(String id){event("onChain",id);super.setOnMainChain(id);}
        public void setInCascade(String id){event("inCascade",id);super.setInCascade(id);}
        public TxPoWOnChainDB getOnChainDB(){return onchain;}
    }
    static class Archive extends ArchiveManager {
        TxBlock last;
        public boolean saveBlock(TxBlock block){event("save",block.getTxPoW().getTxPoWID());return true;}
        public TxBlock loadLastBlock(){event("last");return last;}
        public int checkForCleanDB(){event("clean");return 0;}
        public void checkCascadeRequired(Cascade cascade){event("checkCascade",cascade==null?-1:cascade.getLength());}
    }
    static class Keys extends Wallet {public boolean isAddressRelevant(String address){return false;}public boolean isKeyRelevant(String key){return false;}}
    static class DB extends MinimaDB {
        Pool pool;Archive archive=new Archive();Keys keys=new Keys();
        DB()throws Exception{pool=new Pool();}
        public TxPoWDB getTxPoWDB(){return pool;}public ArchiveManager getArchive(){return archive;}public Wallet getWallet(){return keys;}
        public void writeLock(boolean lock){event("writeLock",lock);super.writeLock(lock);}
        public void setIBDCascade(Cascade value){event("setCascade",value==null?-1:value.getLength());super.setIBDCascade(value);}
    }
    static class Server extends NIOServer {
        Server(){super(0,null);}
        public void sendMessage(String uid,MiniData data){event("send",uid,data.to0xString());}
        public void sendMessageAll(MiniData data){event("send","",data.to0xString());}
    }
    static class Manager extends NIOManager {
        Manager(){super(null);}
        public void PostMessage(Message message){event("nio",message.getMessageType(),message.getString("client"));}
        public void disconnect(String uid,boolean remove){event("disconnect",uid,remove);}
    }
    static class Owner extends Main {
        Owner()throws Exception{super();}
        public boolean isShuttingDown(){return shutdown;}public boolean isShuttongDownOrRestoring(){return shutdown||restore;}
        public void setSyncIBD(boolean value){event("sync",value);sync=value;}
        public void PostMessage(Message message){var tx=(TxPoW)message.getObject("txpow");event("main",message.getMessageType(),tx==null?null:tx.getTxPoWID());}
        public void PostNotifyEvent(String name,JSONObject data){if(!name.equals("MINIMALOG"))event("notify",name,data);}
    }
    static class Processor extends TxPoWProcessor {
        void process(Message message)throws Exception{super.processMessage(message);}Message next(){return getNextMessage();}
    }
    static JSONObject describe(Message message){var out=new JSONObject();out.put("type",message.getMessageType());for(String key:new String[]{"uid","notifyfinish"})if(message.exists(key))out.put(key,message.getObject(key));return out;}
    static Object fixture()throws Exception{
        GeneralParams.USE_SQL_COINDB=false;GeneralParams.BLOCK_LOGS=false;GeneralParams.IS_MEGAMMR=false;events=new JSONArray();failAt=-1;var db=new DB();field(MinimaDB.class,"mMinimaDB").set(null,db);db.getUserDB().setHashRate(new MiniNumber(1000000));
        var parent=ArchiveManagerOracle.txpow(1,0);parent.setBlockDifficulty(Magic.MIN_TXPOW_WORK);int rootNonce=0;do{parent.setNonce(new MiniNumber(rootNonce++));parent.calculateTXPOWID();}while(!parent.isBlock());var root=new TxBlock(parent);var mmr=new MMR();mmr.addEntry(MMRData.CreateMMRDataLeafNode(new MiniNumber(1),new MiniNumber(1)));root.getPreviousPeaks().addAll(mmr.getPeaks());var node=new TxPoWTreeNode(root,false);db.getTxPoWTree().setRoot(node);
        var blocks=new ArrayList<TxBlock>();blocks.add(root);var tx=ArchiveManagerOracle.txpow(70,0);tx.setTxDifficulty(Magic.MIN_TXPOW_WORK);tx.setHeaderBodyHash();tx.calculateTXPOWID();
        for(int i=0;i<7;i++){
            if(i==5)db.getTxPoWTree().setRoot(new TxPoWTreeNode(root,false));
            var tip=db.getTxPoWTree().getTip();var pow=TxPoWGenerator.generateTxPoW(new Transaction(),new Witness());pow.setTimeMilli(new MiniNumber(pow.getBlockNumber().getAsLong()*1000+(i>=5?100:0)));pow.getTxBody().mPRNG=new MiniData("0x12");if(i==1)pow.addBlockTxPOW(tx.getTxPoWIDData());pow.setHeaderBodyHash();
            int nonce=0;do{pow.setNonce(new MiniNumber(nonce++));pow.calculateTXPOWID();}while(!pow.isBlock());
            var list=new ArrayList<TxPoW>();if(i==1)list.add(tx);var block=new TxBlock(tip.getMMR(),pow,list);blocks.add(block);tip.addChildNode(new TxPoWTreeNode(block,false));db.getTxPoWTree().recalculateTree();
        }
        var result=new JSONObject();var wires=new JSONArray();for(var block:blocks)wires.add(MiniData.getMiniDataVersion(block).to0xString());result.put("blocks",wires);result.put("txs",new JSONArray(Arrays.asList(MiniData.getMiniDataVersion(tx).to0xString())));return result;
    }
    static Object run(JSONObject input)throws Exception{
        if(input.containsKey("fixture"))return fixture();
        events=new JSONArray();calls=0;failAt=-1;shutdown=restore=relevant=sync=false;
        GeneralParams.USE_SQL_COINDB=false;GeneralParams.BLOCK_LOGS=false;GeneralParams.IBDSYNC_LOGS=false;GeneralParams.TXBLOCK_NODE=false;GeneralParams.TEST_PARAMS=false;GeneralParams.NOTIFY_ALL_TXPOW=false;GeneralParams.IS_MEGAMMR=false;GeneralParams.NO_SYNC_IBD=false;GeneralParams.DB_IGNORE_LOCKS=false;GeneralParams.NETWORKING_LOGS=false;
        GlobalParams.MINIMA_CASCADE_START_DEPTH=new MiniNumber(num(input,"depth",1000));GlobalParams.MINIMA_CASCADE_FREQUENCY=new MiniNumber(num(input,"frequency",1000));
        var owner=(Owner)U.allocateInstance(Owner.class);var manager=(Manager)U.allocateInstance(Manager.class);var network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(network,"mNIOManager",manager);set(owner,"mNetwork",network);set(manager,"mNIOServer",new Server());set(manager,"mTrafficListener",new NIOTraffic());field(Main.class,"mMainInstance").set(null,owner);
        var db=new DB();field(MinimaDB.class,"mMinimaDB").set(null,db);
        var blocks=new ArrayList<TxBlock>();var txs=new ArrayList<TxPoW>();for(Object raw:(JSONArray)input.get("blocks")){var block=TxBlock.ReadFromStream(new DataInputStream(new ByteArrayInputStream(new MiniData((String)raw).getBytes())));blocks.add(block);txs.add(block.getTxPoW());}for(Object raw:(JSONArray)input.get("txs"))txs.add(TxPoW.ReadFromStream(new DataInputStream(new ByteArrayInputStream(new MiniData((String)raw).getBytes()))));
        var processor=(Processor)U.allocateInstance(Processor.class);set(processor,"mLock",new Object());set(processor,"mMessages",new LinkedList<Message>());set(processor,"mRunning",true);set(processor,"mFirstIBD",System.currentTimeMillis());set(processor,"MAX_FIRST_IBD_TIME",300000L);var rows=new JSONArray();
        for(Object raw:(JSONArray)input.get("ops")){
            var op=(JSONObject)raw;events=new JSONArray();calls=0;failAt=num(op,"fail",-1);
            if(op.containsKey("shutdown"))shutdown=flag(op,"shutdown");if(op.containsKey("restore"))restore=flag(op,"restore");if(op.containsKey("relevant"))relevant=flag(op,"relevant");
            for(String[] pair:new String[][]{{"blockNode","TXBLOCK_NODE"},{"test","TEST_PARAMS"},{"notifyAll","NOTIFY_ALL_TXPOW"},{"mega","IS_MEGAMMR"},{"noSync","NO_SYNC_IBD"},{"logs","BLOCK_LOGS"},{"ibdLogs","IBDSYNC_LOGS"}})if(op.containsKey(pair[0]))field(GeneralParams.class,pair[1]).set(null,flag(op,pair[0]));
            int index=num(op,"tx",0);TxPoW tx=index<0?null:txs.get(index);TxBlock block=index<0||index>=blocks.size()?null:blocks.get(index);var ibd=new IBD();if(op.containsKey("list"))for(Object idx:(JSONArray)op.get("list")){int n=((Number)idx).intValue();ibd.getTxBlocks().add(n<0?null:blocks.get(n));}if(op.containsKey("cascade")){var cascade=new Cascade();int n=num(op,"cascade",-1);if(n>=0)cascade.addToTip(txs.get(n));cascade.cascadeChain();ibd.setCascade(cascade);}if(flag(op,"nullIBD"))ibd=null;
            String uid=op.containsKey("uid")?(String)op.get("uid"):"c0";var row=new JSONObject();row.put("before",System.currentTimeMillis());var log=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
            try{switch((String)op.get("cmd")){
                case "post":processor.postProcessTxPoW(tx);break;case "postBlock":processor.postProcessTxBlock(block);break;case "ibd":processor.postProcessIBD(ibd,uid,flag(op,"override"));break;case "archive":processor.postProcessArchiveIBD(ibd,uid);break;case "sync":processor.postProcessSyncIBD(ibd,uid);break;case "next":processor.process(processor.next());break;
                case "root":db.getTxPoWTree().setRoot(new TxPoWTreeNode(block));break;case "add":db.pool.addTxPoW(tx);break;case "last":db.archive.last=block;break;case "recalc":processor.onStartUpRecalc();break;case "ask":processor.askToSyncTxBlocks(uid);break;case "check":processor.postCheckCall();break;case "timer":processor.resetFirstIBDTimer();break;
                case "nonce":tx.setNonce(new MiniNumber(number(op,"value",0)));break;case "time":tx.setTimeMilli(new MiniNumber(number(op,"value",0)));break;case "parent":tx.setSuperParent(0,new MiniData("0x01"));break;case "chain":tx.getTxHeader().mChainID=new MiniData("0x01");break;case "height":tx.setBlockNumber(new MiniNumber(number(op,"value",0)));break;case "work":tx.setBlockDifficulty(new MiniData("0x01"));break;case "magic":tx.getMagic().mCurrentMaxTxnPerBlock=new MiniNumber(3);break;case "super":tx.setSuperParent(31,new MiniData("0x01"));break;case "link":tx.addBlockTxPOW(txs.get(num(op,"other",0)).getTxPoWIDData());break;case "stop":processor.stopMessageProcessor();break;case "info":break;default:throw new IllegalArgumentException((String)op.get("cmd"));
            }}catch(Exception e){row.put("error",e.toString());}finally{System.setOut(previous);}
            var logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int i=line.indexOf("] : ");if(i>=0){String body=line.substring(i+4);if(!body.startsWith("     "))logs.add(body);}}row.put("logs",logs);row.put("events",events);
            var tree=db.getTxPoWTree();row.put("root",tree.getRoot()==null?null:tree.getRoot().getTxPoW().getTxPoWID());row.put("tip",tree.getTip()==null?null:tree.getTip().getTxPoW().getTxPoWID());row.put("length",tree.getHeaviestBranchLength());row.put("size",tree.getSize());row.put("cascade",MiniData.getMiniDataVersion(db.getCascade()).to0xString());row.put("finished",processor.isIBDProcessFinished());row.put("sync",sync);row.put("lock",MinimaDB.mCurrentWriteLockState);
            var nodes=new JSONArray();var pool=new JSONArray();for(var t:txs){var node=tree.findNode(t.getTxPoWID());var data=new JSONArray();data.add(node!=null);if(node!=null){data.add(node.getTxPoW()==t);data.add(node.getTxPoW().getNonce().toString());data.add(MiniData.getMiniDataVersion(node.getMMR()).to0xString());}nodes.add(data);var entry=db.pool.getCompleteMemPool().get(t.getTxPoWID());var state=new JSONArray();state.add(entry!=null);if(entry!=null){state.add(entry.getTxPoW()==t);state.add(entry.isOnMainChain());state.add(entry.isInCascade());}pool.add(state);}
            var pending=new JSONArray();for(var msg:(LinkedList<Message>)get(processor,"mMessages"))pending.add(describe(msg));row.put("nodes",nodes);row.put("pool",pool);row.put("pending",pending);row.put("after",System.currentTimeMillis());rows.add(row);
        }
        var out=new JSONObject();out.put("ops",rows);return out;
    }
    public static void main(String[] args)throws Exception{var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception e){e.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
