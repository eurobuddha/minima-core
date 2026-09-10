import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import org.minima.database.*;
import org.minima.database.archive.*;
import org.minima.database.txpowdb.*;
import org.minima.database.txpowtree.*;
import org.minima.database.userprefs.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.network.*;
import org.minima.system.network.minima.*;
import org.minima.system.network.p2p.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class NIOManagerStateOracle extends NIOClientOracle {
    static LinkedHashMap<String,NIOClient> clients;
    static ArrayList<Message> posted,p2p;
    static ArrayList<TimerMessage> timers;
    static JSONArray calls;
    static String failure;
    static boolean exists,disappear;
    static int rootReads;
    static TxPoWTreeNode root;
    static TxBlock archive;
    static long number(JSONObject value,String key,long fallback){return value.containsKey(key)?((Number)value.get(key)).longValue():fallback;}
    static boolean flag(JSONObject value,String key){return Boolean.TRUE.equals(value.get(key));}
    static void check(String stage){calls.add(stage);if(stage.equals(failure))throw new IllegalStateException("Injected "+stage);}
    static NIOClient client(String label){if(!clients.containsKey(label))throw new IllegalArgumentException("Unknown client "+label);return clients.get(label);}
    static String label(NIOClient value){if(value==null)return "NULL";for(var row:clients.entrySet())if(row.getValue()==value)return row.getKey();String label="g"+clients.size();clients.put(label,value);return label;}
    static String uid(String value){if(value==null)return null;for(var row:clients.entrySet())if(row.getValue().getUID().equals(value))return row.getKey();return value;}
    static class Manager extends NIOManager {
        Manager(){super(null);}
        public void PostMessage(Message message){if(message.exists("client"))label((NIOClient)message.getObject("client"));posted.add(message);}
        public void PostTimerMessage(TimerMessage message){if(message.exists("client"))label((NIOClient)message.getObject("client"));timers.add(message);check("timer");}
        void invoke(Message message)throws Exception{super.processMessage(message);}
    }
    static class Peers extends P2PManager {
        public void PostMessage(Message message){p2p.add(message);check("p2p");}
        public ArrayList<InetSocketAddress> getPeersCopy(){return new ArrayList<>();}
    }
    static class Tree extends TxPowTree {public TxPoWTreeNode getRoot(){check("root");return disappear&&++rootReads==2?null:root;}public TxPoWTreeNode getTip(){return null;}}
    static class Archive extends ArchiveManager {public TxBlock loadLastBlock(){check("archive");return archive;}}
    static class PoWDB extends TxPoWDB {public boolean exists(String id){check("exists");return exists;}}
    static class User extends UserDB {public String getWelcome(){return "fixture";}}
    static class DB extends MinimaDB {
        Tree tree;Archive archiveDB;PoWDB powDB;User user;
        public TxPowTree getTxPoWTree(){return tree;}public ArchiveManager getArchive(){return archiveDB;}public TxPoWDB getTxPoWDB(){return powDB;}public UserDB getUserDB(){return user;}
        public void readLock(boolean lock){check(lock?"lock":"unlock");}
    }
    static JSONArray labels(Collection<NIOClient> values){var sorted=new ArrayList<String>();for(var value:values)sorted.add(label(value));Collections.sort(sorted);var out=new JSONArray();out.addAll(sorted);return out;}
    static Object address(InetSocketAddress value){var out=new JSONObject();out.put("host",value.getHostString());out.put("address",value.isUnresolved()?null:value.getAddress().getHostAddress());out.put("port",value.getPort());out.put("text",value.toString());out.put("hash",value.hashCode());return out;}
    static JSONObject message(Message value){
        var out=new JSONObject();out.put("type",value.getMessageType());
        for(String key:new String[]{"uid","host"})if(value.exists(key))out.put(key,key.equals("uid")?uid(value.getString(key)):value.getString(key));
        if(value.exists("port"))out.put("port",value.getInteger("port"));
        for(String key:new String[]{"client","nioclient"})if(value.exists(key))out.put(key,value.getObject(key)==null?null:label((NIOClient)value.getObject(key)));
        for(String key:new String[]{"incoming","reconnect"})if(value.exists(key))out.put(key,value.getBoolean(key));
        if(value.exists("address"))out.put("address",address((InetSocketAddress)value.getObject("address")));
        return out;
    }
    static ConcurrentHashMap<String,NIOClient> pending(Manager manager)throws Exception{return (ConcurrentHashMap<String,NIOClient>)get(manager,"mConnectingClients");}
    static ConcurrentHashMap<String,NIOClient> connected(Manager manager)throws Exception{return (ConcurrentHashMap<String,NIOClient>)get(manager.getNIOServer(),"mClients");}
    static Object snapshot(Manager manager)throws Exception{
        var out=new JSONObject();out.put("pending",labels(pending(manager).values()));out.put("connected",labels(connected(manager).values()));
        var rows=new JSONArray();for(var entry:clients.entrySet()){
            var value=entry.getValue();var row=new JSONObject();row.put("label",entry.getKey());row.put("host",value.getHost());row.put("port",value.getPort());row.put("minimaport",value.getMinimaPort());row.put("incoming",value.isIncoming());row.put("valid",value.isValidGreeting());row.put("sent",value.haveSentGreeting());row.put("attempts",value.getConnectAttempts());row.put("maxima",value.getMaximaIdent());var packets=(ArrayList<MiniData>)get(value,"mMessages");if(packets==null)row.put("packets",null);else{var data=new JSONArray();for(var packet:packets)data.add(packet==null?null:packet.to0xString());row.put("packets",data);}row.put("sync",NIOMessage.mlastSyncReq.containsKey(value.getUID()));row.put("chainSync",NIOMessage.mLastChainSync.containsKey(value.getUID()));rows.add(row);
        }out.put("clients",rows);
        var queue=new JSONArray();for(var value:posted)queue.add(message(value));out.put("posted",queue);
        var peer=new JSONArray();for(var value:p2p)peer.add(message(value));out.put("p2p",peer);
        var scheduled=new JSONArray();for(var value:timers){var row=message(value);row.put("delay",get(value,"mDelay"));scheduled.add(row);}out.put("timers",scheduled);
        var invalid=new JSONArray();invalid.addAll(new TreeSet<>(P2PFunctions.mInvalidPeers));out.put("invalid",invalid);
        var disconnects=new JSONArray();for(var value:(ArrayList<String>)get(manager.getNIOServer(),"mDisconnectChannels"))disconnects.add(uid(value));out.put("disconnects",disconnects);
        out.put("calls",calls);out.put("running",manager.isRunning());out.put("serverShutdown",get(manager.getNIOServer(),"mShutDown"));out.put("traffic",manager.getTrafficListener().getBreakdown());out.put("counts",manager.getAllConnectedDetails());out.put("valid",labels(manager.getAllValidConnectedClients()));out.put("outgoing",labels(manager.getAllValidOutGoingConnectedClients()));return out;
    }
    static TxPoW pow(JSONObject config){var value=new TxPoW();value.clearBody();value.setBlockNumber(new MiniNumber(number(config,"height",2)));value.setTimeMilli(new MiniNumber(number(config,"time",0)));value.calculateTXPOWID();return value;}
    static Object runState(JSONObject request)throws Exception{
        GeneralParams.P2P_ENABLED=true;GeneralParams.TXBLOCK_NODE=false;GeneralParams.ARCHIVE=false;GeneralParams.CONNECT_LIST="";GeneralParams.MINIMA_PORT=0;GeneralParams.IS_HOST_SET=false;GeneralParams.NUMBER_DAYS_ARCHIVE=50;
        clients=new LinkedHashMap<>();posted=new ArrayList<>();p2p=new ArrayList<>();timers=new ArrayList<>();calls=new JSONArray();failure="";exists=false;disappear=false;root=null;archive=null;
        P2PFunctions.mInvalidPeers.clear();NIOMessage.mlastSyncReq.clear();NIOMessage.mLastChainSync.clear();
        var manager=(Manager)U.allocateInstance(Manager.class);var network=(NetworkManager)U.allocateInstance(NetworkManager.class);var main=(Main)U.allocateInstance(Main.class);
        set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);set(network,"mNIOManager",manager);set(network,"mP2PManager",U.allocateInstance(Peers.class));
        set(manager,"mNetworkManager",network);set(manager,"mNIOServer",new NIOServer(0,manager));set(manager,"mConnectingClients",new ConcurrentHashMap<String,NIOClient>());set(manager,"mTrafficListener",new NIOTraffic());set(manager,"THREAD_POOL",Executors.newFixedThreadPool(4));set(manager,"mRunning",true);set(manager,"mLock",new Object());manager.RECONNECT_ATTEMPTS=3;set(manager,"LASTREAD_CHECKER",120000L);set(manager,"MAX_LASTREAD_CHECKER",600000L);set(manager,"SYNC_MAX_TIME",86400000L*50);
        var db=(DB)U.allocateInstance(DB.class);db.tree=new Tree();db.archiveDB=(Archive)U.allocateInstance(Archive.class);db.powDB=(PoWDB)U.allocateInstance(PoWDB.class);db.user=new User();field(MinimaDB.class,"mMinimaDB").set(null,db);
        for(Object raw:(JSONArray)request.get("clients")){
            var config=(JSONObject)raw;String name=(String)config.get("label"),where=(String)config.get("map");NIOClient value;
            if(flag(config,"channel")){var channel=new Channel();value=new NIOClient(flag(config,"incoming"),(String)config.get("host"),(int)number(config,"port",9001),channel,new Key(channel));}
            else value=new NIOClient((String)config.get("host"),(int)number(config,"port",9001));
            set(value,"mUID",name);value.setValidGreeting(flag(config,"valid"));value.setSentGreeting(flag(config,"sent"));value.setConnectAttempts((int)number(config,"attempts",1));value.setMinimaPort((int)number(config,"minimaport",-1));if(config.containsKey("maxima"))value.setMaximaIdent((String)config.get("maxima"));clients.put(name,value);NIOMessage.mlastSyncReq.put(name,MiniNumber.ONE);NIOMessage.mLastChainSync.put(name,1L);if(!where.equals("none"))(where.equals("pending")?pending(manager):connected(manager)).put(name,value);
        }
        var states=new JSONArray();
        for(Object raw:(JSONArray)request.get("ops")){
            var op=(JSONObject)raw;String action=(String)op.get("op");calls=new JSONArray();rootReads=0;Object result="OK";
            try{
                switch(action){
                case "config":
                    if(op.containsKey("p2p"))GeneralParams.P2P_ENABLED=flag(op,"p2p");if(op.containsKey("slave"))GeneralParams.TXBLOCK_NODE=flag(op,"slave");if(op.containsKey("archive"))GeneralParams.ARCHIVE=flag(op,"archive");if(op.containsKey("shutdown"))set(main,"mShuttingdown",flag(op,"shutdown"));if(op.containsKey("restore"))set(main,"mRestoring",flag(op,"restore"));if(op.containsKey("exists"))exists=flag(op,"exists");if(op.containsKey("failure"))failure=(String)op.get("failure");if(op.containsKey("connectList"))GeneralParams.CONNECT_LIST=(String)op.get("connectList");if(op.containsKey("limit"))manager.RECONNECT_ATTEMPTS=(int)number(op,"limit",3);if(op.containsKey("invalid")){P2PFunctions.mInvalidPeers.clear();for(Object host:(JSONArray)op.get("invalid"))P2PFunctions.mInvalidPeers.add((String)host);}if(op.containsKey("root"))root=op.get("root")==null?null:new TxPoWTreeNode(pow((JSONObject)op.get("root")));if(op.containsKey("last"))archive=op.get("last")==null?null:new TxBlock(pow((JSONObject)op.get("last")));if(op.containsKey("disappear"))disappear=flag(op,"disappear");break;
                case "set":{
                    var value=client((String)op.get("client"));if(op.containsKey("attempts"))value.setConnectAttempts((int)number(op,"attempts",1));if(op.containsKey("valid"))value.setValidGreeting(flag(op,"valid"));if(op.containsKey("sent"))value.setSentGreeting(flag(op,"sent"));if(op.containsKey("maxima"))value.setMaximaIdent((String)op.get("maxima"));if(op.containsKey("host"))value.overrideHost((String)op.get("host"));if(op.containsKey("minimaport"))value.setMinimaPort((int)number(op,"minimaport",-1));break;}
                case "map":{var value=client((String)op.get("client"));var target=op.get("map").equals("pending")?pending(manager):connected(manager);if(flag(op,"add"))target.put(value.getUID(),value);else target.remove(value.getUID());break;}
                case "disconnect":manager.disconnect(op.get("client")==null?null:client((String)op.get("client")).getUID(),flag(op,"remove"));break;
                case "query":{
                    String method=(String)op.get("method"),value=(String)op.get("value");NIOClient found=switch(method){case "check"->manager.checkConnected(value,flag(op,"only"));case "host"->manager.getNIOClient(value);case "maxima"->manager.getMaximaUID(value);case "uid"->manager.getNIOClientFromUID(value);default->throw new IllegalArgumentException(method);};result=found==null?null:label(found);break;}
                case "hardShutdown":manager.hardShutDown();break;
                default:{
                    var message=new Message((String)op.get("type"));
                    if(op.containsKey("client")){if(message.isMessageType(NIOManager.NIO_TXPOWREQ)||message.isMessageType(NIOManager.NIO_SYNCTXBLOCK))message.addObject("client",op.get("client"));else message.addObject("client",op.get("client")==null?null:client((String)op.get("client")));}
                    for(String key:new String[]{"host","uid","txpowid","reason"})if(op.containsKey(key))message.addObject(key,op.get(key));
                    if(op.containsKey("port"))message.addInteger("port",(int)number(op,"port",0));if(op.containsKey("reconnect"))message.addBoolean("reconnect",flag(op,"reconnect"));
                    if(message.isMessageType(NIOManager.NIO_CHECKLASTMSG)){long now=System.currentTimeMillis();for(var value:clients.values())set(value,"mLastMessageRead",now-number(op,"age",1200000));}
                    manager.invoke(message);break;
                }
                }
            }catch(Exception error){result="ERROR";}
            var row=new JSONObject();row.put("result",result);row.put("state",snapshot(manager));states.add(new JSONParser().parse(row.toJSONString()));
        }
        ((ExecutorService)get(manager,"THREAD_POOL")).shutdown();return states;
    }
    public static void main(String[] args)throws Exception{GeneralParams.USE_SQL_COINDB=false;GeneralParams.IBDSYNC_LOGS=false;GeneralParams.NETWORKING_LOGS=false;var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(runState((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace(System.err);System.out.println("\"ERROR\"");}}
}
