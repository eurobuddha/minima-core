import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.system.Main;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.*;
import org.minima.system.network.p2p.*;
import org.minima.system.network.p2p.params.P2PParams;
import org.minima.system.network.p2p.messages.P2PGreeting;
import org.minima.system.network.p2p.messages.P2PWalkLinks;
import org.minima.system.params.GeneralParams;
import org.minima.objects.base.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class P2PFunctionsOracle extends NIOClientOracle {
    static JSONArray events,posted;static String failure;
    static boolean flag(JSONObject o,String key){return Boolean.TRUE.equals(o.get(key));}
    static long num(JSONObject o,String key,long fallback){return o.containsKey(key)?((Number)o.get(key)).longValue():fallback;}
    static class Manager extends NIOManager {
        Manager(){super(null);}
        public void PostMessage(Message message){var row=new JSONObject();row.put("type",message.getMessageType());for(String key:new String[]{"uid","host","port"})if(message.exists(key))row.put(key,message.getObject(key));posted.add(row);}
    }
    static class MainFixture extends Main {
        public void PostNotifyEvent(String name,JSONObject data){if(name.equals("MINIMALOG")){String message=(String)data.get("message");events.add("log:"+message.substring(message.indexOf(" : ")+3));}}
    }
    static Object info(NIOClientInfo value){if(value==null)return null;var out=new JSONObject();out.put("uid",value.getUID());out.put("host",value.getHost());out.put("port",value.getPort());out.put("incoming",value.isIncoming());out.put("connected",value.isConnected());return out;}
    static JSONArray infos(List<NIOClientInfo> values){var rows=new JSONArray();for(var value:values)rows.add(info(value));return rows;}
    static JSONArray strings(Set<String> values){var list=new ArrayList<String>(values);list.sort(Comparator.nullsFirst(Comparator.naturalOrder()));var out=new JSONArray();out.addAll(list);return out;}
    static InetSocketAddress address(Object raw){if(raw==null)return null;var row=(JSONObject)raw;return flag(row,"unresolved")?InetSocketAddress.createUnresolved((String)row.get("host"),(int)num(row,"port",0)):new InetSocketAddress((String)row.get("host"),(int)num(row,"port",0));}
    static Object snapshot(InetSocketAddress value){if(value==null)return null;var out=new JSONObject();out.put("host",value.getHostString());out.put("address",value.isUnresolved()?null:value.getAddress().getHostAddress());out.put("port",value.getPort());out.put("text",value.toString());out.put("hash",value.hashCode());return out;}
    static HashMap<String,InetSocketAddress> map(Object raw){if(raw==null)return null;var out=new HashMap<String,InetSocketAddress>();for(Object item:(JSONArray)raw){var row=(JSONObject)item;out.put((String)row.get("uid"),address(row.get("address")));}return out;}
    static Object peerMapSnapshot(Map<String,InetSocketAddress> map){if(map==null)return null;var rows=new JSONArray();for(var uid:map.keySet()){var row=new JSONObject();row.put("uid",uid);row.put("address",snapshot(map.get(uid)));rows.add(row);}return rows;}
    static Object swapState(P2PState state){if(state==null)return null;var row=new JSONObject();row.put("in",peerMapSnapshot(state.getInLinks()));row.put("out",peerMapSnapshot(state.getOutLinks()));row.put("not",peerMapSnapshot(state.getNotAcceptingConnP2PLinks()));row.put("none",peerMapSnapshot(state.getNoneP2PLinks()));row.put("my",snapshot(state.getMyMinimaAddress()));row.put("hostSet",state.isHostSet());row.put("discovery",state.isDoingDiscoveryConnection());row.put("secret",state.getIpReqSecret()==null?null:state.getIpReqSecret().to0xString());var known=new JSONArray();for(var value:state.getKnownPeersCopy())known.add(snapshot(value));row.put("known",known);return row;}
    static JSONArray swapMessages(List<Message> messages){var out=new JSONArray();for(var message:messages){var row=new JSONObject();row.put("type",message.getMessageType());row.put("uid",message.getString("uid"));if(message.exists("json"))row.put("json",message.getObject("json"));out.add(row);}return out;}
    static NIOClientInfo walkClient(Object raw){if(raw==null)return null;var row=(JSONObject)raw;return new NIOClientInfo((String)row.get("uid"),(String)row.get("host"),(int)num(row,"port",9001),flag(row,"incoming"));}
    static Object walkMessage(Message message){return message==null?null:swapMessages(List.of(message)).get(0);}
    static Object snapshots(List<InetSocketAddress> values){if(values==null)return null;var rows=new JSONArray();for(var value:values)rows.add(snapshot(value));return rows;}
    static Object walkSnapshot(P2PWalkLinks walk){if(walk==null)return null;var row=new JSONObject();row.put("path",snapshots(walk.getPathTaken()));row.put("secret",walk.getSecret()==null?null:walk.getSecret().to0xString());row.put("returning",walk.isReturning());row.put("client",walk.isClientWalk());row.put("slots",walk.getAvailableNoneP2PConnectionSlots());row.put("joining",walk.isJoiningWalk());row.put("in",walk.isWalkInLinks());row.put("uid",walk.getTargetUidForNextHop());return row;}
    static Object runPeer(JSONObject request)throws Exception {
        if(request.containsKey("url")){try{var connection=new URL((String)request.get("url")).openConnection();connection.connect();connection.getInputStream().close();return true;}catch(Exception error){return false;}}
        events=new JSONArray();posted=new JSONArray();failure=(String)request.get("failure");
        var manager=(Manager)U.allocateInstance(Manager.class);var main=(Main)U.allocateInstance(MainFixture.class);var network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);set(network,"mNIOManager",manager);set(manager,"mNIOServer",new NIOServer(0,manager));set(manager,"mTrafficListener",new NIOTraffic());set(manager,"mConnectingClients",new java.util.concurrent.ConcurrentHashMap<String,NIOClient>());
        var clients=new ArrayList<NIOClient>();if(request.containsKey("clients"))for(Object raw:(JSONArray)request.get("clients")){var row=(JSONObject)raw;boolean connected=flag(row,"connected");var channel=new Channel();var client=new NIOClient(flag(row,"incoming"),(String)row.get("host"),(int)num(row,"port",9001),channel,new Key(channel));set(client,"mUID",row.get("uid"));set(client,"mTimeConnected",0L);client.setValidGreeting(flag(row,"valid"));clients.add(client);((Map<String,NIOClient>)get(connected?manager.getNIOServer():manager,connected?"mClients":"mConnectingClients")).put(client.getUID(),client);}
        GeneralParams.ALLOW_ALL_IP=flag(request,"allow");GeneralParams.TXBLOCK_NODE=false;GeneralParams.NETWORKING_LOGS=false;P2PParams.LOG_LEVEL=request.containsKey("level")&&request.get("level")==null?null:P2PFunctions.Level.values()[(int)num(request,"level",0)];P2PFunctions.clearInvalidPeers();field(P2PFunctions.class,"mLocalAddresses").set(null,null);((Random)field(UtilFuncs.class,"rnd").get(null)).setSeed(num(request,"seed",0));((Random)field(WalkLinksFuncs.class,"random").get(null)).setSeed(num(request,"seed",0));
        if(flag(request,"missingManager"))set(network,"mNIOManager",null);
        if(request.containsKey("locals"))field(P2PFunctions.class,"mLocalAddresses").set(null,new HashSet<String>((Collection<String>)(Collection<?>)request.get("locals")));
        if(request.containsKey("invalid"))for(Object raw:(JSONArray)request.get("invalid"))P2PFunctions.mInvalidPeers.add((String)raw);
        var state=new P2PState();if(request.containsKey("maps")){var maps=(JSONObject)request.get("maps");for(String name:new String[]{"in","out","none","not"})if(maps.containsKey(name)){var links=map(maps.get(name));switch(name){case "in":state.setInLinks(links);break;case "out":state.setOutLinks(links);break;case "none":state.setNoneP2PLinks(links);break;case "not":state.setNotAcceptingConnP2PLinks(links);break;}}}
        if(flag(request,"swap")){GeneralParams.MINIMA_PORT=(int)num(request,"minimaPort",9001);state.setAcceptingInLinks(!request.containsKey("accept")||flag(request,"accept"));state.setHostSet(flag(request,"hostSet"));state.setDoingDiscoveryConnection(flag(request,"discovery"));state.setMaxNumP2PConnections((int)num(request,"max",5));state.setMaxNumNoneP2PConnections((int)num(request,"maxNone",50));if(request.containsKey("my"))state.setMyMinimaAddress(address(request.get("my")));if(request.containsKey("secret"))state.setIpReqSecret(request.get("secret")==null?null:new MiniData((String)request.get("secret")));if(request.containsKey("known"))for(Object value:(JSONArray)request.get("known"))state.getKnownPeers().add(address(value));if(flag(request,"nullState"))state=null;if(flag(request,"noMain"))field(Main.class,"mMainInstance").set(null,null);}
        P2PWalkLinks walk=null;if(request.containsKey("walk")&&request.get("walk")!=null){var row=(JSONObject)request.get("walk");List<InetSocketAddress> path=null;if(row.get("path")!=null){path=new ArrayList<>();for(Object value:(JSONArray)row.get("path"))path.add(address(value));}walk=new P2PWalkLinks(flag(row,"in"),flag(row,"returning"),path);walk.setClientWalk(flag(row,"client"));walk.setJoiningWalk(flag(row,"joining"));walk.setAvailableNoneP2PConnectionSlots((int)num(row,"slots",0));walk.setSecret(row.get("secret")==null?null:new MiniData((String)row.get("secret")));}
        Set<String> saved=null;var states=new JSONArray();for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;String kind=(String)op.get("op");Object result="OK";try{switch(kind){
        case "w_filter":{List<InetSocketAddress> values=null;if(op.get("list")!=null){values=new ArrayList<>();for(Object value:(JSONArray)op.get("list"))values.add(address(value));}result=snapshots(WalkLinksFuncs.removeIPsInBFromA(map(op.get("map")),values));break;}
        case "w_address":result=snapshot(WalkLinksFuncs.getAddressFromUID(state,(String)op.get("uid")));break;
        case "w_next":result=walkMessage(WalkLinksFuncs.createNextHopMsg(address(op.get("address")),walk,state));break;
        case "w_generate":result=walkMessage(WalkLinksFuncs.generateDoSwapMessageFromWalk(state,walk,(int)num(op,"target",5)));break;
        case "w_in":result=walkMessage(WalkLinksFuncs.onInLinkWalkMsg(state,walk,walkClient(op.get("info")),null));break;
        case "w_out":result=walkMessage(WalkLinksFuncs.onOutLinkWalkMsg(state,walk,walkClient(op.get("info")),(int)num(op,"target",5),null));break;
        case "w_response":result=walkMessage(WalkLinksFuncs.onWalkLinkResponseMsg(state,walk));break;
        case "w_return":result=swapMessages(WalkLinksFuncs.onReturnedWalkMsg(state,walk,(int)num(op,"target",5)));break;
        case "w_load_return":result=swapMessages(WalkLinksFuncs.onReturnedLoadBalanceWalkMsg(state,walk));break;
        case "w_load":result=swapMessages(WalkLinksFuncs.genLoadBalanceDoSwaps(state,address(op.get("address")),(int)num(op,"slots",1)));break;
        case "s_connected":{int index=(int)num(op,"client",0);result=swapMessages(SwapLinksFunctions.onConnected(state,flag(op,"incoming"),index<0?null:clients.get(index)));break;}
        case "s_balance":result=swapMessages(SwapLinksFunctions.onConnectedLoadBalanceRequest(state,null));break;
        case "s_join":result=swapMessages(SwapLinksFunctions.joinScaleOutLinks(state,(int)num(op,"target",5),null));break;
        case "s_in":result=swapMessages(SwapLinksFunctions.requestInLinks(state,(int)num(op,"target",5),null));break;
        case "s_disconnect":{var message=new Message();if(op.get("uid")!=null)message.addString("uid",(String)op.get("uid"));SwapLinksFunctions.onDisconnected(state,flag(op,"nullMessage")?null:message);break;}
        case "s_greeting":{NIOClientInfo client=null;if(op.get("info")!=null){var row=(JSONObject)op.get("info");client=new NIOClientInfo((String)row.get("uid"),(String)row.get("host"),(int)num(row,"port",9001),flag(row,"incoming"));}var greeting=op.get("greeting")==null?null:P2PGreeting.fromJSON((JSONObject)op.get("greeting"));result=SwapLinksFunctions.processGreeting(state,greeting,client,flag(op,"noconnect"));break;}
        case "s_request":result=SwapLinksFunctions.processRequestIPMsg((JSONObject)op.get("message"),(String)op.get("host"));break;
        case "s_response":SwapLinksFunctions.processResponseIPMsg(state,(JSONObject)op.get("message"));break;
        case "s_echo":{var message=new JSONObject();message.put("secret",state.getIpReqSecret().to0xString());message.put("res_ip",op.get("host"));SwapLinksFunctions.processResponseIPMsg(state,message);break;}
        case "s_wrap_alias":{var data=new JSONObject();var wrapped=SwapLinksFunctions.wrapP2PMsg(data);data.put("after",true);result=wrapped;break;}
        case "s_wrap":result=SwapLinksFunctions.wrapP2PMsg((JSONObject)op.get("message"));break;
        case "ipv6":result=P2PFunctions.isIPv6((String)op.get("value"));break;case "local":result=P2PFunctions.isIPLocal((String)op.get("value"));break;
        case "invalid":result=P2PFunctions.isInvalidPeer((String)op.get("value"));break;case "addInvalid":P2PFunctions.addInvalidPeer((String)op.get("value"));break;case "clearInvalid":P2PFunctions.clearInvalidPeers();break;
        case "connect":P2PFunctions.connect((String)op.get("host"),(int)num(op,"port",9001));break;case "check":result=P2PFunctions.checkConnect((String)op.get("host"),(int)num(op,"port",9001));break;case "disconnect":P2PFunctions.disconnect((String)op.get("uid"));break;
        case "all":result=infos(P2PFunctions.getAllConnections());break;case "connected":result=infos(P2PFunctions.getAllConnectedConnections());break;case "info":result=info(P2PFunctions.getNIOCLientInfo((String)op.get("uid")));break;
        case "send":P2PFunctions.sendP2PMessage((String)op.get("uid"),(JSONObject)op.get("message"));break;
        case "log":P2PFunctions.log_node_runner((String)op.get("value"));P2PFunctions.log_info((String)op.get("value"));P2PFunctions.log_debug((String)op.get("value"));break;
        case "available":result=P2PFunctions.isNetAvailable();break;case "locals":result=strings(P2PFunctions.getLocalAddresses());break;case "saveLocal":saved=P2PFunctions.getLocalAddresses();break;case "localAdd":P2PFunctions.getLocalAddresses().add((String)op.get("value"));result=saved==P2PFunctions.getLocalAddresses();break;case "localClear":P2PFunctions.getLocalAddresses().clear();break;
        case "find":result=info(UtilFuncs.getClientFromInetAddress(address(op.get("address")),state));break;case "search":result=info(UtilFuncs.searchLinksMapForAddress(address(op.get("address")),map(op.get("map"))));break;
        case "select":{ArrayList<InetSocketAddress> addresses=null;if(op.get("list")!=null){addresses=new ArrayList<>();for(Object item:(JSONArray)op.get("list"))addresses.add(address(item));}result=snapshot(UtilFuncs.selectRandomAddress(addresses));break;}
        case "draw":result=((Random)field(UtilFuncs.class,"rnd").get(null)).nextInt((int)num(op,"bound",1));break;
        default:throw new IllegalArgumentException(kind);
        }}catch(Exception error){result="ERROR";}
        var row=new JSONObject();row.put("result",result);row.put("events",events);row.put("posted",posted);row.put("invalid",strings(P2PFunctions.mInvalidPeers));var queues=new JSONArray();for(var client:clients){var queue=new JSONArray();for(var packet:(ArrayList<MiniData>)get(client,"mMessages"))queue.add(packet.to0xString());queues.add(queue);}row.put("queues",queues);row.put("traffic",manager.getTrafficListener().getBreakdown());if(request.containsKey("walk"))row.put("walk",walkSnapshot(walk));if(flag(request,"swap")){row.put("state",swapState(state));var greetings=new JSONArray();for(var client:clients)greetings.add(client.hasReceivedP2PGreeting());row.put("greetings",greetings);}states.add(new JSONParser().parse(row.toJSONString()));}return states;
    }
    static void stage(String value)throws IOException{events.add(value);if(failure!=null&&value.startsWith(failure))throw new IOException("Injected availability error");}
    public static void main(String[] args)throws Exception {
        URL.setURLStreamHandlerFactory(protocol->protocol.equals("https")?new URLStreamHandler(){protected URLConnection openConnection(URL url)throws IOException{stage("open:"+url);return new URLConnection(url){public void connect()throws IOException{stage("connect");}public InputStream getInputStream()throws IOException{stage("input");return new ByteArrayInputStream(new byte[0]){public void close()throws IOException{stage("close");}};}};}}:null);
        var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(runPeer((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace(System.err);System.out.println("\"ERROR\"");}
    }
}
