import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.system.Main;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.*;
import org.minima.system.network.p2p.*;
import org.minima.system.network.p2p.params.P2PParams;
import org.minima.system.params.GeneralParams;
import org.minima.objects.base.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class P2PManagerOracle extends P2PFunctionsOracle {
    static P2PDB db;static int saves;static boolean dbFailure,saveFailure;
    static class DB extends MinimaDB {
        public P2PDB getP2PDB(){if(dbFailure)throw new IllegalStateException("DB receiver failure");return db;}
        public void saveP2PDB(){++saves;if(saveFailure)throw new IllegalStateException("Save failure");}
    }
    static JSONObject event(Message message){var row=new JSONObject();row.put("type",message.getMessageType());if(message.exists("address"))row.put("address",snapshot((InetSocketAddress)message.getObject("address")));if(message.exists("force"))row.put("force",message.getBoolean("force"));return row;}
    static class ManagerFixture extends P2PManager {
        JSONArray timers;
        void process(Message message)throws Exception{super.processMessage(message);}
        public void PostTimerMessage(TimerMessage message){var row=event(message);try{row.put("delay",get(message,"mDelay"));}catch(Exception error){throw new RuntimeException(error);}timers.add(row);}
    }
    static void stack(Object processor)throws Exception{set(processor,"mMessages",new LinkedList<Message>());set(processor,"mLock",new Object());set(processor,"mRunning",true);}
    static String text(JSONObject request,String key,String fallback){return request.containsKey(key)?(String)request.get(key):fallback;}
    static Object runManager(JSONObject request)throws Exception {
        if(request.containsKey("encode"))return PeersCheckerOracle.runChecker(request);
        events=new JSONArray();posted=new JSONArray();saves=0;dbFailure=false;saveFailure=false;
        GeneralParams.TEST_PARAMS=false;GeneralParams.ALLOW_ALL_IP=flag(request,"allow");GeneralParams.TXBLOCK_NODE=false;GeneralParams.NETWORKING_LOGS=false;GeneralParams.IS_MOBILE=false;
        GeneralParams.MINIMA_HOST=text(request,"host","127.0.0.1");GeneralParams.MINIMA_PORT=(int)num(request,"port",9001);GeneralParams.IS_HOST_SET=flag(request,"hostSet");GeneralParams.IS_ACCEPTING_IN_LINKS=!request.containsKey("accept")||flag(request,"accept");GeneralParams.NOCONNECT=flag(request,"noconnect");GeneralParams.P2P_ADDNODES=text(request,"addnodes","");GeneralParams.P2P_ROOTNODE=text(request,"root","");
        P2PParams.TGT_NUM_LINKS=(int)num(request,"max",5);P2PParams.TGT_NUM_NONE_P2P_LINKS=(int)num(request,"maxNone",100);P2PParams.MIN_NUM_CONNECTIONS=(int)num(request,"min",2);P2PParams.LOOP_DELAY=(int)num(request,"loopDelay",600000);P2PParams.LOOP_DELAY_VARIABILITY=(int)num(request,"variability",30000);P2PParams.SAVE_DATA_DELAY=300000;P2PParams.NODE_NOT_ACCEPTING_CHECK_DELAY=600000;P2PParams.DEFAULT_NODE_LIST=new ArrayList<>();
        if(request.containsKey("defaults"))for(Object raw:(JSONArray)request.get("defaults"))P2PParams.DEFAULT_NODE_LIST.add(address(raw));
        P2PParams.LOG_LEVEL=P2PFunctions.Level.DEBUG;P2PPeersChecker.MAX_VERIFIED_PEERS=(int)num(request,"cap",250);P2PFunctions.clearInvalidPeers();field(P2PFunctions.class,"mLocalAddresses").set(null,new HashSet<String>());for(var pair:new Object[][]{{UtilFuncs.class,"rnd"},{WalkLinksFuncs.class,"random"},{P2PManager.class,"rand"}})((Random)field((Class<?>)pair[0],(String)pair[1]).get(null)).setSeed(num(request,"seed",5564));
        var nio=(P2PFunctionsOracle.Manager)U.allocateInstance(P2PFunctionsOracle.Manager.class);var network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(network,"mNIOManager",nio);var main=(Main)U.allocateInstance(MainFixture.class);set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);set(nio,"mNIOServer",new NIOServer(0,nio));set(nio,"mTrafficListener",new NIOTraffic());set(nio,"mConnectingClients",new java.util.concurrent.ConcurrentHashMap<String,NIOClient>());
        var clients=new ArrayList<NIOClient>();if(request.containsKey("clients"))for(Object raw:(JSONArray)request.get("clients")){var row=(JSONObject)raw;var channel=new Channel();var client=new NIOClient(flag(row,"incoming"),(String)row.get("host"),(int)num(row,"port",9001),channel,new Key(channel));set(client,"mUID",row.get("uid"));set(client,"mTimeConnected",0L);client.setValidGreeting(flag(row,"valid"));clients.add(client);((Map<String,NIOClient>)get(flag(row,"connected")?nio.getNIOServer():nio,flag(row,"connected")?"mClients":"mConnectingClients")).put(client.getUID(),client);}
        db=new P2PDB();if(request.containsKey("db"))db.getAllData().putAll((JSONObject)request.get("db"));field(MinimaDB.class,"mMinimaDB").set(null,U.allocateInstance(DB.class));
        var manager=(ManagerFixture)U.allocateInstance(ManagerFixture.class);stack(manager);manager.timers=new JSONArray();var state=new P2PState();set(manager,"state",state);set(manager,"EXCLUDE_FROM_CLEAR",new ArrayList<String>(List.of(P2PManager.P2P_LOOP,P2PManager.P2P_ASSESS_CONNECTIVITY,P2PManager.P2P_SAVE_DATA)));set(manager,"firstgo",flag(request,"first"));set(manager,"mLastNotifyNoPeers",0L);set(manager,"NOTIFY_NOPEERS_TIMER",180000L);manager.INITIAL_PEERS_LIST_NUM=(int)num(request,"initial",0);manager.P2P_HEALTH_CHECK_TIMER=num(request,"healthDelay",600000);
        var checker=(P2PPeersChecker)U.allocateInstance(P2PPeersChecker.class);stack(checker);set(checker,"verifiedPeers",new HashSet<InetSocketAddress>());set(checker,"unverifiedPeers",new HashSet<InetSocketAddress>());set(checker,"p2PManager",manager);set(manager,"mPeersChecker",checker);
        manager.PostTimerMessage(new TimerMessage(10000,P2PManager.P2P_LOOP));manager.PostTimerMessage(new TimerMessage(P2PParams.NODE_NOT_ACCEPTING_CHECK_DELAY,P2PManager.P2P_ASSESS_CONNECTIVITY));manager.PostTimerMessage(new TimerMessage(300000,P2PManager.P2P_SAVE_DATA));
        state.setDoingDiscoveryConnection(flag(request,"discovery"));state.setNoConnect(flag(request,"stateNoConnect"));state.setStartupComplete(flag(request,"startup"));state.setHostSet(flag(request,"stateHostSet"));state.setAcceptingInLinks(!request.containsKey("stateAccept")||flag(request,"stateAccept"));if(request.containsKey("my"))state.setMyMinimaAddress(address(request.get("my")));if(request.containsKey("known"))for(Object raw:(JSONArray)request.get("known"))state.getKnownPeers().add(address(raw));
        if(request.containsKey("maps")){var maps=(JSONObject)request.get("maps");for(String name:new String[]{"in","out","none","not","all"})if(maps.containsKey(name)){var links=map(maps.get(name));switch(name){case "in":state.setInLinks(links);break;case "out":state.setOutLinks(links);break;case "none":state.setNoneP2PLinks(links);break;case "not":state.setNotAcceptingConnP2PLinks(links);break;case "all":state.setAllLinks(links);break;}}}
        for(String key:new String[]{"verified","unverified"})if(request.containsKey(key))for(Object raw:(JSONArray)request.get(key))(key.equals("verified")?checker.getVerifiedPeers():checker.getUnverifiedPeers()).add(address(raw));
        var states=new JSONArray();for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;String kind=(String)op.get("op");Object result="OK";if(flag(op,"interrupt"))Thread.currentThread().interrupt();try{switch(kind){
        case "status":result=manager.getStatus(flag(op,"full"));break;case "address":result=manager.getP2PAddress();break;case "clients":{float value=manager.getClients();result=Float.isNaN(value)?"NaN":Float.isInfinite(value)?value>0?"Infinity":"-Infinity":Double.valueOf(value);break;}
        case "have":result=manager.haveAnyPeers();break;case "update":manager.updateP2PPeersList();break;case "shutdown":manager.shutdown();break;case "initial":manager.INITIAL_PEERS_LIST_NUM=(int)num(op,"value",0);break;case "failures":dbFailure=flag(op,"db");saveFailure=flag(op,"save");break;
        case "post":for(int i=0;i<num(op,"count",1);++i)manager.PostMessage(new Message((String)op.get("type")));break;
        case "copy":{var peers=manager.getPeersCopy();result=snapshots(peers);peers.clear();break;}
        case "process":{var message=new Message((String)op.get("type"));if(op.containsKey("uid"))message.addString("uid",(String)op.get("uid"));if(op.containsKey("address"))message.addObject("address",address(op.get("address")));if(op.containsKey("client")){int index=(int)num(op,"client",-1);message.addObject("client",index<0?null:clients.get(index));}if(op.containsKey("message"))message.addObject("message",op.get("message"));manager.process(flag(op,"nullMessage")?null:message);break;}
        default:throw new IllegalArgumentException(kind);
        }}catch(Exception error){result="ERROR";}
        var peer=(JSONObject)swapState(state);peer.put("all",peerMapSnapshot(state.getAllLinks()));peer.put("accept",state.isAcceptingInLinks());peer.put("noconnect",state.isNoConnect());peer.put("startup",state.isStartupComplete());peer.put("max",state.getMaxNumP2PConnections());peer.put("maxNone",state.getMaxNumNoneP2PConnections());peer.put("delay",state.getLoopDelay());
        var row=new JSONObject();if(op.containsKey("interrupt"))row.put("threadInterrupted",Thread.interrupted());row.put("result",result);row.put("state",peer);row.put("events",events);row.put("posted",posted);var checkerPosts=new JSONArray();for(var message:(LinkedList<Message>)get(checker,"mMessages"))checkerPosts.add(event(message));row.put("checker",checkerPosts);var pending=new JSONArray();for(var message:(LinkedList<Message>)get(manager,"mMessages"))pending.add(message==null?null:message.getMessageType());row.put("pending",pending);row.put("db",db.getAllData());row.put("saves",saves);row.put("initial",manager.INITIAL_PEERS_LIST_NUM);row.put("running",manager.isRunning());row.put("checkerRunning",checker.isRunning());row.put("timers",manager.timers);var queues=new JSONArray();var greetings=new JSONArray();for(var client:clients){var queue=new JSONArray();for(var packet:(ArrayList<MiniData>)get(client,"mMessages"))queue.add(packet.to0xString());queues.add(queue);greetings.add(client.hasReceivedP2PGreeting());}row.put("queues",queues);row.put("greetings",greetings);row.put("traffic",nio.getTrafficListener().getBreakdown());row.put("read",nio.getTrafficListener().getTotalRead());row.put("write",nio.getTrafficListener().getTotalWrite());row.put("verified",snapshots(new ArrayList<>(checker.getVerifiedPeers())));row.put("unverified",snapshots(new ArrayList<>(checker.getUnverifiedPeers())));states.add(new JSONParser().parse(row.toJSONString()));
        }return states;
    }
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(runManager((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace(System.err);System.out.println("\"ERROR\"");}}
}
