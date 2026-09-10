import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.system.Main;
import org.minima.system.network.p2p.*;
import org.minima.system.network.p2p2.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class P2P2RuntimeOracle extends P2PManagerOracle {
    static P2P2DB db2;static P2PDB old;static int calls,failAt;static boolean offline;
    static void access(String name){events.add(name);if(++calls==failAt)throw new IllegalStateException("DB receiver failure");}
    static class DB extends MinimaDB {public P2P2DB getP2P2DB(){access("db2");return db2;}public P2PDB getP2PDB(){access("oldDB");return old;}}
    static class Main2 extends MainFixture {
        public void PostNotifyEvent(String name,JSONObject data){if(name.equals("MINIMALOG")){String message=(String)data.get("message");if(message.contains(" : Peer added : ")||message.contains(" : Peer already added : "))return;}super.PostNotifyEvent(name,data);}
    }
    static class Manager2 extends P2P2Manager {
        JSONArray timers;
        void process(Message message)throws Exception{super.processMessage(message);}
        public void PostTimerMessage(TimerMessage message){var row=new JSONObject();row.put("type",message.getMessageType());try{row.put("delay",get(message,"mDelay"));}catch(Exception error){throw new RuntimeException(error);}timers.add(row);}
    }
    static Object run2(JSONObject request)throws Exception {
        var setup=new JSONObject();setup.putAll(request);setup.put("ops",new JSONArray());runPeer(setup);
        var network=Main.getInstance().getNetworkManager();var main=(Main)U.allocateInstance(Main2.class);set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);
        P2P2Functions.clearInvalidPeers();P2PFunctions.clearInvalidPeers();var locals=new HashSet<String>();if(request.containsKey("locals"))for(Object value:(JSONArray)request.get("locals"))locals.add((String)value);field(P2P2Functions.class,"mLocalAddresses").set(null,locals);field(P2PFunctions.class,"mLocalAddresses").set(null,new HashSet<String>());
        if(request.containsKey("invalid"))for(Object value:(JSONArray)request.get("invalid"))P2P2Functions.mInvalidPeers.add((String)value);
        db2=new P2P2DB();old=new P2PDB();if(request.containsKey("peers"))for(Object value:(JSONArray)request.get("peers"))db2.getAllKnownPeers().add((String)value);if(request.containsKey("old"))old.getAllData().putAll((JSONObject)request.get("old"));if(request.containsKey("data"))db2.getAllData().putAll((JSONObject)request.get("data"));calls=0;failAt=(int)num(request,"dbFailAt",-1);offline=flag(request,"offline");field(MinimaDB.class,"mMinimaDB").set(null,U.allocateInstance(DB.class));
        var manager=(Manager2)U.allocateInstance(Manager2.class);stack(manager);manager.timers=new JSONArray();manager.P2P2_LOOP_TIMER=300000;manager.P2P2_LOOP_TIMER_SLOW=21600000;manager.NUMBER_DESIRED_CONNECTIONS=(int)num(request,"desired",3);manager.PostMessage(P2P2Manager.P2P2_INIT);manager.PostTimerMessage(new TimerMessage(manager.P2P2_LOOP_TIMER,P2P2Manager.P2P2_FASTLOOP));manager.PostTimerMessage(new TimerMessage(manager.P2P2_LOOP_TIMER_SLOW,P2P2Manager.P2P2_SLOWLOOP));manager.P2P2_LOOP_TIMER=num(request,"delay",300000);
        var states=new JSONArray();for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;String kind=(String)op.get("op");Object result="OK";try{switch(kind){
        case "ipv6":result=P2P2Functions.isIPv6((String)op.get("value"));break;case "local":result=P2P2Functions.isIPLocal((String)op.get("value"));break;case "invalid":result=P2P2Functions.isInvalidPeer((String)op.get("value"));break;case "addInvalid":P2P2Functions.addInvalidPeer((String)op.get("value"));break;case "clearInvalid":P2P2Functions.clearInvalidPeers();break;
        case "connect":P2P2Functions.connect((String)op.get("host"),(int)num(op,"port",9001));break;case "check":result=P2P2Functions.checkConnect((String)op.get("host"),(int)num(op,"port",9001));break;case "disconnect":P2P2Functions.disconnect((String)op.get("uid"));break;case "all":result=infos(P2P2Functions.getAllConnections());break;case "connected":result=infos(P2P2Functions.getAllConnectedConnections());break;case "info":result=info(P2P2Functions.getNIOCLientInfo((String)op.get("uid")));break;case "available":result=P2P2Functions.isNetAvailable();break;
        case "locals":result=strings(P2P2Functions.getLocalAddresses());break;case "localAdd":P2P2Functions.getLocalAddresses().add((String)op.get("value"));result=locals==P2P2Functions.getLocalAddresses();break;
        case "independent":P2PFunctions.mInvalidPeers.add("old-only");P2PFunctions.getLocalAddresses().add("old-only");result=!P2P2Functions.isInvalidPeer("old-only")&&!P2P2Functions.getLocalAddresses().contains("old-only");break;
        case "random":result=manager.getRandomPeerFromList();break;case "convert":manager.convertOldP2P();break;case "choose":manager.checkWhichConnect();break;case "shutdown":manager.shutdown();break;case "process":manager.process(flag(op,"nullMessage")?null:new Message((String)op.get("type")));break;
        default:throw new IllegalArgumentException(kind);
        }}catch(Exception error){result="ERROR";}
        var row=new JSONObject();row.put("result",result);row.put("events",events);row.put("posted",posted);row.put("timers",manager.timers);var pending=new JSONArray();for(var message:(LinkedList<Message>)get(manager,"mMessages"))pending.add(message.getMessageType());row.put("pending",pending);row.put("running",manager.isRunning());var peers=new JSONArray();peers.addAll(db2.getAllKnownPeers());row.put("peers",peers);row.put("data",db2.getAllData());row.put("old",old.getAllData());row.put("invalid",strings(P2P2Functions.mInvalidPeers));states.add(new JSONParser().parse(row.toJSONString()));
        }return states;
    }
    public static void main(String[] args)throws Exception{
        URL.setURLStreamHandlerFactory(protocol->protocol.equals("https")?new URLStreamHandler(){protected URLConnection openConnection(URL url)throws IOException{events.add("available");if(offline)throw new IOException("Offline fixture");return new URLConnection(url){public void connect(){}public InputStream getInputStream(){return new ByteArrayInputStream(new byte[0]);}};}}:null);
        var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run2((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace(System.err);System.out.println("\"ERROR\"");}
    }
}
