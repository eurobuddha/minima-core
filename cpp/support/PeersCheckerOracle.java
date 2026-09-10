import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.txpowtree.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.network.minima.*;
import org.minima.system.network.p2p.*;
import org.minima.system.network.p2p.params.P2PParams;
import org.minima.system.params.GeneralParams;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class PeersCheckerOracle extends P2PFunctionsOracle {
    static TxPowTree tree;static boolean tipFailure,offline;static JSONArray parentEvents;
    static JSONObject event(Message message){var row=new JSONObject();row.put("type",message.getMessageType());row.put("address",snapshot((InetSocketAddress)message.getObject("address")));for(String key:new String[]{"force","forcelog"})if(message.exists(key))row.put(key,message.getBoolean(key));return row;}
    static class Parent extends P2PManager {public void PostMessage(Message message){parentEvents.add(event(message));}}
    static class DB extends MinimaDB {public TxPowTree getTxPoWTree(){events.add("tip");if(tipFailure)throw new IllegalStateException("Tip failure");return tree;}}
    static class Checker extends P2PPeersChecker {
        JSONArray queued,timers;boolean timerFailure;
        Checker(){super(null);}
        void process(Message message)throws Exception{super.processMessage(message);}
        public void PostMessage(Message message){queued.add(event(message));}
        public void PostTimerMessage(TimerMessage message){if(timerFailure)throw new IllegalStateException("Timer receiver failure");var row=event(message);try{row.put("delay",get(message,"mDelay"));}catch(Exception error){throw new RuntimeException(error);}timers.add(row);}
    }
    static Object runChecker(JSONObject request)throws Exception {
        if(request.containsKey("encode")){var greeting=new Greeting();set(greeting,"mVersion",new MiniString((String)request.get("version")));greeting.getExtraData().putAll((JSONObject)request.get("extra"));return MiniData.getMiniDataVersion(NIOManager.createNIOMessage(new MiniByte((int)num(request,"type",12)),greeting)).to0xString();}
        if(request.containsKey("pow")){var value=new TxPoW();value.getTxBody().mPRNG=new MiniData("0x12");for(int level=0;level<32;level++)value.setSuperParent(level,MiniData.ZERO_TXPOWID);value.setTimeMilli(MiniNumber.ZERO);value.setBlockNumber(new MiniNumber(num(request,"height",1)));value.setHeaderBodyHash();value.calculateTXPOWID();var result=new JSONObject();result.put("wire",MiniData.getMiniDataVersion(value).to0xString());result.put("id",value.getTxPoWID());return result;}
        events=new JSONArray();parentEvents=new JSONArray();var nio=NIOClientOracle.install();set(nio,"mNIOServer",new NIOServer(0,nio));set(nio,"mConnectingClients",new java.util.concurrent.ConcurrentHashMap<String,NIOClient>());var network=Main.getInstance().getNetworkManager();var main=(Main)U.allocateInstance(MainFixture.class);set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);
        if(flag(request,"connected")){var channel=new Channel();var client=new NIOClient(false,"8.8.8.8",9001,channel,new Key(channel));set(client,"mUID","c0");((Map<String,NIOClient>)get(nio.getNIOServer(),"mClients")).put("c0",client);}
        if(flag(request,"missingManager"))set(network,"mNIOManager",null);if(flag(request,"noMain"))field(Main.class,"mMainInstance").set(null,null);
        GeneralParams.TEST_PARAMS=flag(request,"test");GeneralParams.ALLOW_ALL_IP=flag(request,"allow");GeneralParams.NETWORKING_LOGS=false;GeneralParams.PEERSCHECKER_lOG=flag(request,"logs");P2PParams.LOG_LEVEL=P2PFunctions.Level.values()[(int)num(request,"level",2)];P2PPeersChecker.MAX_VERIFIED_PEERS=(int)num(request,"max",250);field(P2PFunctions.class,"mLocalAddresses").set(null,new HashSet<String>());offline=flag(request,"offline");tipFailure=flag(request,"tipFailure");
        tree=new TxPowTree();TxPoWTreeNode first=null,last=null;if(request.containsKey("chain"))for(Object raw:(JSONArray)request.get("chain")){var pow=TxPoW.ReadFromStream(new DataInputStream(new ByteArrayInputStream(new MiniData((String)raw).getBytes())));var node=new TxPoWTreeNode(pow);if(last==null)first=node;else last.addChildNode(node);last=node;}if(first!=null)tree.setRoot(first);field(MinimaDB.class,"mMinimaDB").set(null,U.allocateInstance(DB.class));
        var checker=(Checker)U.allocateInstance(Checker.class);checker.queued=new JSONArray();checker.timers=new JSONArray();checker.timerFailure=flag(request,"timerFailure");set(checker,"unverifiedPeers",new HashSet<InetSocketAddress>());set(checker,"verifiedPeers",new HashSet<InetSocketAddress>());set(checker,"p2PManager",flag(request,"missingParent")?null:U.allocateInstance(Parent.class));set(checker,"PEERS_LOOP_TIMER",num(request,"loopDelay",21600000));
        for(String key:new String[]{"verified","unverified"})if(request.containsKey(key)){var peers=key.equals("verified")?checker.getVerifiedPeers():checker.getUnverifiedPeers();for(Object raw:(JSONArray)request.get(key))peers.add(address(raw));}
        var verified=checker.getVerifiedPeers();var unverified=checker.getUnverifiedPeers();var states=new JSONArray();for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;Object result="OK";try{String kind=(String)op.get("op");var address=address(op.get("address"));switch(kind){
        case "have":result=checker.haveAnyPeers();break;case "admit":checker.checkUnverifiedPeer(address);break;case "verify":verified.add(address);break;case "unverify":unverified.add(address);break;case "clearPending":unverified.clear();break;case "clear":verified.clear();unverified.clear();break;case "identity":result=verified==checker.getVerifiedPeers()&&unverified==checker.getUnverifiedPeers();break;
        default:{String type=switch(kind){case "add"->P2PPeersChecker.PEERS_ADDPEERS;case "check"->P2PPeersChecker.PEERS_CHECKPEERS;case "loop"->P2PPeersChecker.PEERS_LOOP;case "force"->P2PPeersChecker.PEERS_FORCEFULLCHECK;case "init"->P2PPeersChecker.PEERS_INIT;default->(String)op.get("type");};var message=new Message(type).addObject("address",address);for(String key:new String[]{"force","forcelog"})if(op.containsKey(key))message.addObject(key,op.get(key));checker.process(flag(op,"nullMessage")?null:message);}
        }}catch(Exception error){result="ERROR";}
        var row=new JSONObject();row.put("result",result);row.put("events",events);row.put("posted",checker.queued);row.put("parent",parentEvents);row.put("timers",checker.timers);row.put("verified",snapshots(new ArrayList<>(verified)));row.put("unverified",snapshots(new ArrayList<>(unverified)));row.put("traffic",nio.traffic.getBreakdown());row.put("read",nio.traffic.getTotalRead());row.put("write",nio.traffic.getTotalWrite());states.add(new JSONParser().parse(row.toJSONString()));}return states;
    }
    public static void main(String[] args)throws Exception{
        URL.setURLStreamHandlerFactory(protocol->protocol.equals("https")?new URLStreamHandler(){protected URLConnection openConnection(URL url)throws IOException{events.add("available");if(offline)throw new IOException("Offline fixture");return new URLConnection(url){public void connect(){}public InputStream getInputStream(){return new ByteArrayInputStream(new byte[0]);}};}}:null);
        var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(runChecker((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace(System.err);System.out.println("\"ERROR\"");}
    }
}
