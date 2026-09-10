import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.database.*;
import org.minima.database.archive.*;
import org.minima.database.cascade.*;
import org.minima.database.txpowdb.*;
import org.minima.database.txpowtree.*;
import org.minima.database.userprefs.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.brains.*;
import org.minima.system.network.*;
import org.minima.system.network.minima.*;
import org.minima.system.network.p2p.*;
import org.minima.system.network.p2p2.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.Streamable;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Calls the unmodified pinned dispatcher. Only runtime receivers are replaced;
 * constructors that start a node, worker, miner or socket are never invoked. */
public class NIOMessageOracle extends NIOClientOracle {
    static JSONArray events,posted,timers;
    static HashMap<String,TxPoW> pow;
    static boolean exists,mempoolCoin;
    static TxPowTree tree;
    static Cascade cascade;
    static int clients;
    static boolean flag(JSONObject o,String key){return Boolean.TRUE.equals(o.get(key));}
    static long number(JSONObject o,String key,long fallback){return o.containsKey(key)?((Number)o.get(key)).longValue():fallback;}
    static TxPoW decode(Object wire)throws Exception{return TxPoW.ReadFromStream(new DataInputStream(new ByteArrayInputStream(new MiniData((String)wire).getBytes())));}
    static JSONObject event(String receiver,Message message){var out=new JSONObject();out.put("receiver",receiver);out.put("type",message.getMessageType());for(String key:new String[]{"uid","incoming","message"})if(message.exists(key))out.put(key,message.getObject(key));return out;}
    static JSONObject timer(String receiver,TimerMessage message){var out=event(receiver,message);try{out.put("delay",get(message,"mDelay"));}catch(Exception error){throw new RuntimeException(error);}return out;}
    static class Manager extends NIOManager {
        Manager(){super(null);}
        public void PostMessage(Message message){posted.add(event("nio",message));}
        public void PostTimerMessage(TimerMessage message){timers.add(timer("nio",message));}
    }
    static class Peers extends P2PManager {
        public void PostMessage(Message message){events.add(event("p2p",message));}
        public void PostTimerMessage(TimerMessage message){events.add(timer("p2p",message));}
        public ArrayList<InetSocketAddress> getPeersCopy(){return new ArrayList<>(List.of(new InetSocketAddress("127.0.0.1",9002)));}
        public float getClients(){return ((float)clients)/3.0f;}
    }
    static class PoWDB extends TxPoWDB {
        public boolean exists(String id){events.add("exists:"+id);return exists||pow.containsKey(id);}
        public TxPoW getTxPoW(String id){events.add("get:"+id);return pow.get(id);}
        public boolean checkMempoolCoins(MiniData id){return mempoolCoin;}
    }
    static class Blocks extends TxBlockDB {public TxBlock findTxBlock(String id){events.add("block:"+id);return null;}}
    static class Archive extends ArchiveManager {
        public MiniNumber exists(String id){return MiniNumber.MINUSONE;}
        public TxBlock loadBlock(String id){events.add("archive:"+id);return null;}
        public ArrayList<TxBlock> loadSyncBlockRange(MiniNumber number){events.add("archive-sync:"+number);return new ArrayList<>();}
        public ArrayList<TxBlock> loadBlockRange(MiniNumber first,MiniNumber last,boolean descending){events.add("archive-range:"+first+":"+last+":"+descending);return new ArrayList<>();}
        public TxBlock loadLastBlock(){return null;}
        public Cascade loadCascade(){return cascade;}
    }
    static class User extends UserDB {public String getWelcome(){return "fixture";}}
    static class DB extends MinimaDB {
        PoWDB powDB;Blocks blocks;Archive archive;User user;P2P2DB peers;
        public TxPowTree getTxPoWTree(){return tree;}public Cascade getCascade(){return cascade;}
        public TxPoWDB getTxPoWDB(){return powDB;}public TxBlockDB getTxBlockDB(){return blocks;}
        public ArchiveManager getArchive(){return archive;}public UserDB getUserDB(){return user;}
        public P2P2DB getP2P2DB(){return peers;}public void readLock(boolean lock){}
    }
    static class Processor extends TxPoWProcessor {
        public void postProcessIBD(IBD ibd,String uid){events.add("ibd:"+uid+":"+MiniData.getMiniDataVersion(ibd).to0xString());}
        public void postProcessSyncIBD(IBD ibd,String uid){events.add("sync:"+uid+":"+MiniData.getMiniDataVersion(ibd).to0xString());}
        public void postProcessTxPoW(TxPoW value){events.add("pow:"+value.getTxPoWID());}
        public void postProcessTxBlock(TxBlock value){events.add("txblock:"+value.getTxPoW().getTxPoWID());}
    }
    static class MainFixture extends Main {
        public void PostTimerMessage(TimerMessage message){events.add(timer("main",message));}
        public void PostNotifyEvent(String name,JSONObject data){if(name.equals("MINIMALOG"))return;events.add(name);events.add(data);}
    }
    static class Miner extends TxPoWMiner {public void mineTxPoWAsync(TxPoW value){events.add("mine:"+value.getTxPoWID());}}
    static Object runMessage(JSONObject request)throws Exception {
        events=new JSONArray();posted=new JSONArray();timers=new JSONArray();pow=new HashMap<>();exists=flag(request,"exists");tree=new TxPowTree();cascade=new Cascade();clients=(int)number(request,"clients",1);
        if(request.get("root")!=null)tree.setRoot(new TxPoWTreeNode(decode(request.get("root"))));
        if(request.containsKey("chain")){TxPoWTreeNode first=null,last=null;for(Object raw:(JSONArray)request.get("chain")){var node=new TxPoWTreeNode(decode(raw));if(last!=null)last.addChildNode(node);else first=node;last=node;}if(first!=null)tree.setRoot(first);}
        if(request.containsKey("cascade")){for(Object raw:(JSONArray)request.get("cascade"))cascade.addToTip(decode(raw));cascade.cascadeChain();}
        if(request.containsKey("mmr")&&tree.getTip()!=null)set(tree.getTip(),"mMMR",request.get("mmr")==null?null:org.minima.objects.mmr.MMR.ReadFromStream(new DataInputStream(new ByteArrayInputStream(new MiniData((String)request.get("mmr")).getBytes()))));
        field(TxPoWGenerator.class,"MEMPOOL_FULL").set(null,flag(request,"mempoolFull"));field(TxPoWGenerator.class,"MIN_MEMPOOL_BURN").set(null,new MiniNumber(number(request,"burn",0)));mempoolCoin=flag(request,"mempoolCoin");
        if(request.containsKey("db"))for(Object wire:(JSONArray)request.get("db")){var value=decode(wire);pow.put(value.getTxPoWID(),value);}
        GeneralParams.TEST_PARAMS=flag(request,"test");GeneralParams.P2P_ENABLED=flag(request,"p2p");GeneralParams.P2P2_ENABLED=flag(request,"p2p2");GeneralParams.TXBLOCK_NODE=flag(request,"slave");GeneralParams.ALLOW_GEN_MESSAGE=flag(request,"allow");GeneralParams.IS_MEGAMMR=flag(request,"mega");GeneralParams.LIMIT_IBD_30MINS=flag(request,"limitIBD");GeneralParams.ARCHIVESYNC_LIMIT_BANDWIDTH=flag(request,"limitBandwidth");GeneralParams.IS_HOST_SET=false;GeneralParams.MINIMA_PORT=0;GeneralParams.RESCUE_MEGAMMR_NODE=flag(request,"rescue")?"fixture":"";
        var manager=(Manager)U.allocateInstance(Manager.class);var main=(Main)U.allocateInstance(MainFixture.class);var network=(NetworkManager)U.allocateInstance(NetworkManager.class);
        set(main,"mNetwork",network);set(main,"mShuttingdown",flag(request,"shutdown"));set(main,"mSyncIBD",flag(request,"sync"));set(main,"AUTOMINE_TIMER",50000L);set(main,"mTxPoWProcessor",U.allocateInstance(Processor.class));set(main,"mTxPoWMiner",U.allocateInstance(Miner.class));field(Main.class,"mMainInstance").set(null,main);
        set(network,"mNIOManager",manager);set(network,"mP2PManager",U.allocateInstance(Peers.class));set(manager,"mNetworkManager",network);set(manager,"mNIOServer",new NIOServer(0,manager));set(manager,"mTrafficListener",new NIOTraffic());
        var db=(DB)U.allocateInstance(DB.class);db.powDB=(PoWDB)U.allocateInstance(PoWDB.class);db.blocks=new Blocks();db.archive=(Archive)U.allocateInstance(Archive.class);db.user=new User();db.peers=new P2P2DB();db.peers.getAllKnownPeers().add("127.0.0.1:9003");db.peers.getAllKnownPeers().add(null);field(MinimaDB.class,"mMinimaDB").set(null,db);
        var channel=new Channel();var client=new NIOClient(flag(request,"incoming"),"127.0.0.1",9001,channel,new Key(channel));set(client,"mUID","c0");client.setValidGreeting(flag(request,"valid"));client.setSentGreeting(flag(request,"sent"));if(flag(request,"peerGreeting"))client.setReceivedP2PGreeting();if(!flag(request,"missing"))((Map<String,NIOClient>)get(manager.getNIOServer(),"mClients")).put("c0",client);
        P2PFunctions.mInvalidPeers.clear();NIOMessage.mlastSyncReq.clear();NIOMessage.mLastChainSync.clear();NIOMessage.mHaveSentIBDRecently.clear();NIOMessage.LAST_TXBLOCKMINE_MSG=flag(request,"recentMine")?System.currentTimeMillis():0;
        var states=new JSONArray();for(Object raw:(JSONArray)request.get("packets")){
            var message=new NIOMessage(request.containsKey("uid")?(String)request.get("uid"):"c0",raw==null?null:new MiniData((String)raw));message.setFullAddress(request.containsKey("full")?(String)request.get("full"):"127.0.0.1:9001");message.setTrace(flag(request,"trace"),request.containsKey("filter")?(String)request.get("filter"):"");Object result="OK";try{message.run();}catch(Exception error){result="ERROR";}
            var row=new JSONObject();row.put("result",result);row.put("retained",get(message,"mData")!=null);row.put("events",events);var packets=new JSONArray();for(var packet:(ArrayList<MiniData>)get(client,"mMessages"))packets.add(packet.to0xString());row.put("packets",packets);row.put("posted",posted);row.put("timers",timers);row.put("host",client.getHost());row.put("port",client.getMinimaPort());row.put("valid",client.isValidGreeting());row.put("sent",client.haveSentGreeting());row.put("welcome",client.getWelcomeMessage());row.put("traffic",manager.getTrafficListener().getBreakdown());var invalid=new JSONArray();invalid.addAll(new TreeSet<>(P2PFunctions.mInvalidPeers));row.put("invalid",invalid);var last=NIOMessage.mlastSyncReq.get("c0");row.put("sync",last==null?null:last.toString());row.put("chainSync",NIOMessage.mLastChainSync.containsKey("c0"));row.put("ibdRecent",NIOMessage.mHaveSentIBDRecently.size());row.put("heavier",get(message,"HEAVIER_CHAIN_FOUND"));states.add(new JSONParser().parse(row.toJSONString()));
        }return states;
    }
    static Object encode(JSONObject request)throws Exception {
        String kind=(String)request.get("kind");Streamable value;
        if(kind.equals("chain")){
            var values=new JSONArray();String parent="0x00";for(int i=0;i<number(request,"count",1);i++){var spec=new JSONObject();spec.put("kind","pow");spec.put("type",4);spec.put("height",number(request,"start",1)+i);spec.put("parent",parent);spec.put("difficulty",java.math.BigInteger.ONE.shiftLeft(256).subtract(java.math.BigInteger.ONE).toString());if(flag(request,"transactions")){var ids=new JSONArray();ids.add("0xF000"+String.format("%08X",i));spec.put("ids",ids);}var encoded=(JSONObject)encode(spec);values.add(encoded);parent=(String)encoded.get("id");}return values;
        }
        if(kind.equals("consensus")){
            String[] fixture=ConsensusOracle.fixture((int)number(request,"mode",0),(String)request.get("script"),(int)number(request,"outputs",1),1024).split(" ");var txpow=decode(fixture[1]);var out=new JSONObject();out.put("mmr",new MiniData(fixture[0]).to0xString());out.put("wire",new MiniData(fixture[1]).to0xString());out.put("root",new MiniData(fixture[2]).to0xString());out.put("id",txpow.getTxPoWID());out.put("packet",NIOManager.createNIOMessage(NIOMessage.MSG_TXPOW,txpow).to0xString());return out;
        }
        switch(kind){
        case "data":value=new MiniData((String)request.get("value"));break;
        case "string":value=new MiniString((String)request.get("value"));break;
        case "number":value=new MiniNumber(request.get("value").toString());break;
        case "greeting":{var greeting=new Greeting();if(request.containsKey("version"))set(greeting,"mVersion",new MiniString((String)request.get("version")));greeting.setTopBlock(new MiniNumber(number(request,"top",-1)));if(request.containsKey("extra"))greeting.getExtraData().putAll((JSONObject)request.get("extra"));if(request.containsKey("ids"))for(Object id:(JSONArray)request.get("ids"))greeting.getChain().add(new MiniData((String)id));value=greeting;break;}
        case "pulse":{var pulse=new Pulse();for(Object id:(JSONArray)request.get("ids"))pulse.getBlockList().add(new MiniData((String)id));value=pulse;break;}
        case "ibd":{var ibd=new IBD();if(flag(request,"cascade"))ibd.setCascade(new Cascade());if(request.containsKey("cascadeBlocks")){var cascade=new Cascade();for(Object wire:(JSONArray)request.get("cascadeBlocks"))cascade.addToTip(decode(wire));cascade.cascadeChain();ibd.setCascade(cascade);}if(request.containsKey("blocks"))for(Object wire:(JSONArray)request.get("blocks"))ibd.getTxBlocks().add(new TxBlock(decode(wire)));value=ibd;break;}
        case "block":value=new TxBlock(decode(request.get("value")));break;
        case "pow":{var txpow=request.containsKey("value")?decode(request.get("value")):new TxPoW();txpow.getTxBody().mPRNG=new MiniData("0x12");for(int level=0;level<32;level++)txpow.setSuperParent(level,MiniData.ZERO_TXPOWID);txpow.setTimeMilli(new MiniNumber(number(request,"time",0)));txpow.setBlockNumber(new MiniNumber(number(request,"height",2)));if(request.containsKey("difficulty"))txpow.setBlockDifficulty(new MiniData(new java.math.BigInteger(request.get("difficulty").toString())));if(request.containsKey("parent"))txpow.setSuperParent(0,new MiniData((String)request.get("parent")));if(request.containsKey("ids"))for(Object id:(JSONArray)request.get("ids"))txpow.getBlockTransactions().add(new MiniData((String)id));if(request.containsKey("chainID"))txpow.getTxHeader().mChainID=new MiniData((String)request.get("chainID"));txpow.setHeaderBodyHash();if(flag(request,"noBody"))txpow.clearBody();txpow.calculateTXPOWID();value=txpow;break;}
        default:throw new IllegalArgumentException(kind);
        }
        var out=new JSONObject();out.put("wire",MiniData.getMiniDataVersion(value).to0xString());out.put("packet",NIOManager.createNIOMessage(new MiniByte((int)number(request,"type",0)),value).to0xString());if(value instanceof TxPoW txpow)out.put("id",txpow.getTxPoWID());return out;
    }
    public static void main(String[] args)throws Exception {GeneralParams.USE_SQL_COINDB=false;GeneralParams.NETWORKING_LOGS=false;GeneralParams.IBDSYNC_LOGS=false;var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{var request=(JSONObject)new JSONParser().parse(line);System.out.println(JSONValue.toJSONString(request.containsKey("kind")?encode(request):runMessage(request)));}catch(Exception error){error.printStackTrace(System.err);System.out.println("\"ERROR\"");}}
}
