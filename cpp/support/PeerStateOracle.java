import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.objects.base.MiniData;
import org.minima.system.network.p2p.*;
import org.minima.system.network.p2p.messages.*;
import org.minima.system.network.p2p.params.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class PeerStateOracle extends PeerMessagesOracle {
    static Map<String,InetSocketAddress> map(P2PState state,String name){switch(name){
case "InLinks":return state.getInLinks();
case "OutLinks":return state.getOutLinks();
case "NotAcceptingConnP2PLinks":return state.getNotAcceptingConnP2PLinks();
case "NoneP2PLinks":return state.getNoneP2PLinks();
case "AllLinks":return state.getAllLinks();
default:throw new IllegalArgumentException(name);}}
    static void setMap(P2PState state,String name,Map<String,InetSocketAddress> values){switch(name){
case "InLinks":state.setInLinks(values);return;
case "OutLinks":state.setOutLinks(values);return;
case "NotAcceptingConnP2PLinks":state.setNotAcceptingConnP2PLinks(values);return;
case "NoneP2PLinks":state.setNoneP2PLinks(values);return;
case "AllLinks":state.setAllLinks(values);return;
default:throw new IllegalArgumentException(name);}}
    static Object mapSnapshot(Map<String,InetSocketAddress> map){if(map==null)return null;var out=new JSONArray();for(var entry:map.entrySet()){var item=new JSONObject();item.put("key",entry.getKey());item.put("address",snapshot(entry.getValue()));out.add(item);}return out;}
    static JSONObject stateSnapshot(P2PState state){var out=new JSONObject();
out.put("AcceptingInLinks",state.isAcceptingInLinks());
out.put("MaxNumNoneP2PConnections",state.getMaxNumNoneP2PConnections());
out.put("MaxNumP2PConnections",state.getMaxNumP2PConnections());
out.put("DoingDiscoveryConnection",state.isDoingDiscoveryConnection());
out.put("StartupComplete",state.isStartupComplete());
out.put("LoopDelay",state.getLoopDelay());
out.put("NoConnect",state.isNoConnect());
out.put("HostSet",state.isHostSet());
out.put("InLinks",mapSnapshot(state.getInLinks()));
out.put("OutLinks",mapSnapshot(state.getOutLinks()));
out.put("NotAcceptingConnP2PLinks",mapSnapshot(state.getNotAcceptingConnP2PLinks()));
out.put("NoneP2PLinks",mapSnapshot(state.getNoneP2PLinks()));
out.put("AllLinks",mapSnapshot(state.getAllLinks()));
out.put("address",snapshot(state.getMyMinimaAddress()));out.put("secret",state.getIpReqSecret()==null?null:state.getIpReqSecret().to0xString());out.put("known",snapshots(state.getKnownPeersCopy()));try{out.put("json",state.toJson());}catch(Exception e){out.put("json","ERROR");}return out;}
    static JSONObject greetingSnapshot(P2PGreeting greeting){var out=new JSONObject();
out.put("MyMinimaPort",greeting.getMyMinimaPort());
out.put("AcceptingInLinks",greeting.isAcceptingInLinks());
out.put("NumNoneP2PConnections",greeting.getNumNoneP2PConnections());
out.put("MaxNumNoneP2PConnections",greeting.getMaxNumNoneP2PConnections());
out.put("KnownPeers",snapshots(greeting.getKnownPeers()));
out.put("InLinks",snapshots(greeting.getInLinks()));
out.put("OutLinks",snapshots(greeting.getOutLinks()));
try{out.put("json",greeting.toJson());}catch(Exception e){out.put("json","ERROR");}return out;}
    static JSONObject parameterSnapshot(){var out=new JSONObject();out.put("VERSION",P2PParams.VERSION);out.put("LOG_LEVEL",P2PParams.LOG_LEVEL==null?null:P2PParams.LOG_LEVEL.toString());
out.put("PEERS_LIST_SIZE",P2PParams.PEERS_LIST_SIZE);
out.put("TGT_NUM_LINKS",P2PParams.TGT_NUM_LINKS);
out.put("TGT_NUM_NONE_P2P_LINKS",P2PParams.TGT_NUM_NONE_P2P_LINKS);
out.put("MIN_NUM_CONNECTIONS",P2PParams.MIN_NUM_CONNECTIONS);
out.put("LOOP_DELAY",P2PParams.LOOP_DELAY);
out.put("HASH_RATE_UPDATE_DELAY",P2PParams.HASH_RATE_UPDATE_DELAY);
out.put("LOOP_DELAY_VARIABILITY",P2PParams.LOOP_DELAY_VARIABILITY);
out.put("NODE_NOT_ACCEPTING_CHECK_DELAY",P2PParams.NODE_NOT_ACCEPTING_CHECK_DELAY);
out.put("SAVE_DATA_DELAY",P2PParams.SAVE_DATA_DELAY);
out.put("WALK_LINKS_EXPIRE_TIME",P2PParams.WALK_LINKS_EXPIRE_TIME);
out.put("AUTH_KEY_EXPIRY",P2PParams.AUTH_KEY_EXPIRY);
out.put("METRICS_DELAY",P2PParams.METRICS_DELAY);
out.put("nodes",snapshots(P2PParams.DEFAULT_NODE_LIST));return out;}
    static final String[] testIntegers={"TGT_NUM_LINKS","TGT_NUM_NONE_P2P_LINKS","MIN_NUM_CONNECTIONS","LOOP_DELAY","HASH_RATE_UPDATE_DELAY","LOOP_DELAY_VARIABILITY","NODE_NOT_ACCEPTING_CHECK_DELAY","SAVE_DATA_DELAY","WALK_LINKS_EXPIRE_TIME","AUTH_KEY_EXPIRY","METRICS_DELAY"};
    static JSONObject testSettingsSnapshot()throws Exception{var out=new JSONObject();for(String name:testIntegers)out.put(name,P2PTestParams.class.getField(name).get(null));out.put("LOG_LEVEL",P2PTestParams.LOG_LEVEL==null?null:P2PTestParams.LOG_LEVEL.toString());out.put("METRICS_URL",P2PTestParams.METRICS_URL);out.put("nodes",snapshots(P2PTestParams.DEFAULT_NODE_LIST));return out;}
    static JSONObject captureTestSettings(Object result)throws Exception{var row=new JSONObject();row.put("result",result);row.put("test",testSettingsSnapshot());try{row.put("active",parameterSnapshot());}catch(Exception error){row.put("active","ERROR");}row.put("alias",P2PTestParams.DEFAULT_NODE_LIST==P2PParams.DEFAULT_NODE_LIST);return row;}
    static Object runTestSettings(JSONObject request)throws Exception{var output=new JSONArray();output.add(captureTestSettings("INITIAL"));for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;Object result="OK";try{String name=(String)op.get("op");Object value=op.get("value");switch(name){
        case "apply":P2PTestParams.setTestParams();break;
        case "set":{String field=(String)op.get("field");if(field.equals("LOG_LEVEL"))P2PTestParams.LOG_LEVEL=value==null?null:P2PFunctions.Level.values()[((Number)value).intValue()];else if(field.equals("METRICS_URL"))P2PTestParams.METRICS_URL=(String)value;else P2PTestParams.class.getField(field).setInt(null,((Number)value).intValue());break;}
        case "replace":P2PTestParams.DEFAULT_NODE_LIST=list(value);break;
        default:{var nodes=Boolean.TRUE.equals(op.get("active"))?P2PParams.DEFAULT_NODE_LIST:P2PTestParams.DEFAULT_NODE_LIST;switch(name){case "append":nodes.add(address(value));break;case "clear":nodes.clear();break;case "element":result=snapshot(nodes.set(0,address(value)));break;default:throw new IllegalArgumentException(name);}}
    }}catch(Exception error){result="ERROR";}output.add(captureTestSettings(result));}return output;}
    static Object runState(JSONObject request)throws Exception{GeneralParams.MINIMA_PORT=9001;GeneralParams.IS_MOBILE=false;P2PParams.LOOP_DELAY=600000;P2PParams.TGT_NUM_LINKS=5;P2PParams.TGT_NUM_NONE_P2P_LINKS=100;
    String kind=(String)request.get("kind");
    if(kind.equals("testParams"))return runTestSettings(request);
    if(kind.equals("state")){var state=new P2PState();var external=new HashMap<String,Map<String,InetSocketAddress>>();List<InetSocketAddress> copy=null;var out=new JSONArray();out.add(stateSnapshot(state));for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;String name=(String)op.get("op"),field=(String)op.get("field");Object value=op.get("value"),result="OK";try{switch(name){
        case "set":switch(field){
case "AcceptingInLinks":state.setAcceptingInLinks((Boolean)value);break;
case "MaxNumNoneP2PConnections":state.setMaxNumNoneP2PConnections(((Number)value).intValue());break;
case "MaxNumP2PConnections":state.setMaxNumP2PConnections(((Number)value).intValue());break;
case "DoingDiscoveryConnection":state.setDoingDiscoveryConnection((Boolean)value);break;
case "StartupComplete":state.setStartupComplete((Boolean)value);break;
case "LoopDelay":state.setLoopDelay(((Number)value).longValue());break;
case "NoConnect":state.setNoConnect((Boolean)value);break;
case "HostSet":state.setHostSet((Boolean)value);break;
default:throw new IllegalArgumentException(field);}break;
        case "address":state.setMyMinimaAddress(address(value));break;
        case "host":state.setMyMinimaAddress((String)value);break;
        case "secret":state.setIpReqSecret(value==null?null:new MiniData((String)value));break;
        case "port":GeneralParams.MINIMA_PORT=((Number)value).intValue();break;
        case "mobile":GeneralParams.IS_MOBILE=(Boolean)value;break;
        case "loopParam":P2PParams.LOOP_DELAY=((Number)value).intValue();break;
        case "loopReset":state.setLoopDelayToParamValue();break;
        case "put":result=snapshot(map(state,field).put((String)op.get("key"),address(value)));break;
        case "remove":result=snapshot(map(state,field).remove((String)op.get("key")));break;
        case "clear":map(state,field).clear();break;
        case "replace":{Map<String,InetSocketAddress> m=value==null?null:new HashMap<>();if(value!=null)for(Object e:(JSONArray)value){var entry=(JSONObject)e;m.put((String)entry.get("key"),address(entry.get("value")));}external.put(field,m);setMap(state,field,m);break;}
        case "external":result=snapshot(external.get(field).put((String)op.get("key"),address(value)));break;
        case "alias":setMap(state,field,map(state,(String)value));break;
        case "knownAdd":result=state.getKnownPeers().add(address(value));break;
        case "knownRemove":result=state.getKnownPeers().remove(address(value));break;
        case "knownContains":result=state.getKnownPeers().contains(address(value));break;
        case "knownClear":state.getKnownPeers().clear();break;
        case "knownCopy":copy=state.getKnownPeersCopy();break;
        case "copyAppend":copy.add(address(value));break;
        case "greeting":result=greetingSnapshot(new P2PGreeting(state));break;
        default:throw new IllegalArgumentException(name);
        }}catch(Exception e){result="ERROR";}var snap=new JSONObject();snap.put("result",result);snap.put("state",stateSnapshot(state));snap.put("copy",snapshots(copy));out.add(snap);}return out;}
    if(kind.equals("greeting")){var known=list(request.get("known"));var in=list(request.get("in"));var outLinks=list(request.get("out"));String ctor=(String)request.get("ctor");P2PGreeting greeting=ctor.equals("short")?new P2PGreeting(9121,known):ctor.equals("full")?new P2PGreeting(9121,true,outLinks,in,5,20,known):new P2PGreeting();var out=new JSONArray();out.add(greetingSnapshot(greeting));for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;String name=(String)op.get("op"),field=(String)op.get("field");Object value=op.get("value"),result="OK";try{switch(name){
        case "set":switch(field){
case "MyMinimaPort":greeting.setMyMinimaPort(((Number)value).intValue());break;
case "AcceptingInLinks":greeting.setAcceptingInLinks((Boolean)value);break;
case "NumNoneP2PConnections":greeting.setNumNoneP2PConnections(((Number)value).intValue());break;
case "MaxNumNoneP2PConnections":greeting.setMaxNumNoneP2PConnections(((Number)value).intValue());break;
case "KnownPeers":greeting.setKnownPeers(list(value));break;
case "InLinks":greeting.setInLinks(list(value));break;
case "OutLinks":greeting.setOutLinks(list(value));break;
default:throw new IllegalArgumentException(field);}break;
        case "append":switch(field){
case "KnownPeers":greeting.getKnownPeers().add(address(value));break;
case "InLinks":greeting.getInLinks().add(address(value));break;
case "OutLinks":greeting.getOutLinks().add(address(value));break;
default:throw new IllegalArgumentException(field);}break;
        case "external":switch(field){case "KnownPeers":known.add(address(value));break;case "InLinks":in.add(address(value));break;case "OutLinks":outLinks.add(address(value));break;default:throw new IllegalArgumentException(field);}break;
        case "read":greeting=P2PGreeting.fromJSON((JSONObject)value);break;
        case "equal":result=greeting.equals(P2PGreeting.fromJSON((JSONObject)value));break;
        case "self":result=greeting.equals(greeting);break;
        case "null":result=greeting.equals(null);break;
        case "roundtrip":result=greeting.equals(P2PGreeting.fromJSON((JSONObject)greeting.toJson().get("greeting")));break;
        default:throw new IllegalArgumentException(name);
        }}catch(Exception e){result="ERROR";}var snap=new JSONObject();snap.put("result",result);snap.put("greeting",greetingSnapshot(greeting));out.add(snap);}return out;}
    if(kind.equals("params")){var out=parameterSnapshot();var operations=new JSONArray();try{P2PParams.DEFAULT_NODE_LIST.add(address(request.get("value")));operations.add("OK");}catch(Exception e){operations.add("ERROR");}try{P2PParams.DEFAULT_NODE_LIST.clear();operations.add("OK");}catch(Exception e){operations.add("ERROR");}try{P2PParams.DEFAULT_NODE_LIST.set(0,address(request.get("value")));operations.add("OK");}catch(Exception e){operations.add("ERROR");}out.put("operations",operations);return out;}
    if(kind.equals("db")){P2PParams.VERSION="1.1";var db=new P2PDB();var external=list(request.get("peers"));List<InetSocketAddress> extracted=null;var out=new JSONArray();for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;String name=(String)op.get("op");Object value=op.get("value"),result="OK";try{switch(name){
    case "set":db.setPeersList(list(value));break;
    case "externalSet":db.setPeersList(external);break;
    case "externalAppend":external.add(address(value));break;
    case "get":extracted=db.getPeersList();result=snapshots(extracted);break;
    case "append":extracted.add(address(value));break;
    case "jsonPut":db.getAllData().put((String)op.get("key"),value);break;
    case "read":db.readDataStream(new DataInputStream(new ByteArrayInputStream(new MiniData((String)value).getBytes())));break;
    case "roundtrip":{var bytes=new ByteArrayOutputStream();db.writeDataStream(new DataOutputStream(bytes));db.readDataStream(new DataInputStream(new ByteArrayInputStream(bytes.toByteArray())));break;}
    case "version":db.setVersion();break;
    case "versionParam":P2PParams.VERSION=(String)value;break;
    default:throw new IllegalArgumentException(name);
    }}catch(Exception e){result="ERROR";}var snap=new JSONObject();snap.put("result",result);snap.put("data",db.getAllData());try{snap.put("peers",snapshots(db.getPeersList()));}catch(Exception e){snap.put("peers","ERROR");}try{snap.put("version",db.getVersion());}catch(Exception e){snap.put("version","ERROR");}try{var bytes=new ByteArrayOutputStream();db.writeDataStream(new DataOutputStream(bytes));snap.put("wire",new MiniData(bytes.toByteArray()).to0xString());}catch(Exception e){snap.put("wire","ERROR");}snap.put("extracted",snapshots(extracted));
    // Freeze JSON references before the next operation mutates the live DB.
    out.add(new JSONParser().parse(snap.toJSONString()));}return out;}
    throw new IllegalArgumentException(kind);}
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(runState((JSONObject)new JSONParser().parse(line))));}catch(Exception e){System.out.println("\"ERROR\"");}}
}
