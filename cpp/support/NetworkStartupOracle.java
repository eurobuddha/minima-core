import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.database.*;
import org.minima.database.userprefs.UserDB;
import org.minima.database.txpowtree.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.NIOManager;
import org.minima.system.network.p2p.P2PManager;
import org.minima.system.commands.CommandRunner;
import org.minima.system.commands.network.connect;
import org.minima.system.params.GeneralParams;
import org.minima.utils.messages.Message;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class NetworkStartupOracle extends NIOClientOracle {
    static JSONObject config;static JSONArray calls,posted;static int depth;static boolean postFail;
    static TxPoWTreeNode tip;static ArrayList<InetSocketAddress> peers;
    static void check(String stage){calls.add(stage);if(stage.equals(config.get("fail")))throw new IllegalStateException("Injected "+stage);}
    static class DB extends MinimaDB {
        UserDB user;TxPowTree tree;
        public void readLock(boolean lock){check(lock?"lock":"unlock");depth+=lock?1:-1;}
        public UserDB getUserDB(){return user;}
        public TxPowTree getTxPoWTree(){return tree;}
    }
    static class User extends UserDB {public String getWelcome(){check("welcome");return (String)config.get("welcome");}}
    static class Tree extends TxPowTree {public TxPoWTreeNode getTip(){check("tip");return tip;}}
    static class Peers extends P2PManager {public ArrayList<InetSocketAddress> getPeersCopy(){check("peers");return peers;}}
    static Object message(Message msg){if(msg==null)return null;var out=new JSONObject();out.put("type",msg.getMessageType());out.put("host",msg.getString("host"));out.put("port",msg.getInteger("port"));return out;}
    static class Manager extends NIOManager {Manager(){super(null);}public void PostMessage(Message msg){posted.add(message(msg));if(postFail)throw new RuntimeException("Post failure");}}
    static Object startup(JSONObject request)throws Exception{
        var main=(Main)U.allocateInstance(Main.class);var network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);
        if(request.get("kind").equals("connect")){
            posted=new JSONArray();postFail=(boolean)request.get("fail");set(network,"mNIOManager",U.allocateInstance(Manager.class));var out=new JSONObject();
            try{out.put("parsed",message(connect.createConnectMessage((String)request.get("host"))));}catch(Exception e){out.put("parsed","ERROR");}
            if(request.containsKey("command"))out.put("response",CommandRunner.getRunner().runMultiCommand((String)request.get("command")));
            else{var command=new connect();command.getParams().put("host",request.get("host"));try{out.put("response",command.runCommand());}catch(Exception e){out.put("response",e.getClass()==Exception.class?"EXCEPTION:"+e.getMessage():"ERROR");}}
            var prototype=new connect();out.put("help",prototype.getFullHelp());out.put("name",prototype.getName());out.put("shortHelp",prototype.getHelp());var params=new JSONArray();params.addAll(prototype.getValidParams());out.put("params",params);out.put("fresh",prototype.getFunction().getParams().size()==0);out.put("posted",posted);return out;
        }
        var db=(DB)U.allocateInstance(DB.class);db.user=new User();db.tree=new Tree();field(MinimaDB.class,"mMinimaDB").set(null,db);set(network,"mP2PManager",U.allocateInstance(Peers.class));
        var greeting=new Greeting();greeting.setTopBlock(new MiniNumber(123));greeting.getChain().add(new MiniData("0xFF"));greeting.getExtraData().put("retained",true);var states=new JSONArray();depth=0;
        for(Object value:(JSONArray)request.get("ops")){
            config=(JSONObject)value;calls=new JSONArray();tip=null;int count=((Number)config.get("count")).intValue();
            for(int i=0;i<count;i++){var tx=new TxPoW();tx.clearBody();tx.setTimeMilli(new MiniNumber(0));tx.setBlockNumber(new MiniNumber(i+1));tx.calculateTXPOWID();var node=new TxPoWTreeNode(tx);if(tip!=null)tip.addChildNode(node);tip=node;}
            var list=PeerMessagesOracle.list(config.get("peers"));peers=list==null?null:new ArrayList<>(list);GeneralParams.IS_HOST_SET=(boolean)config.get("hostSet");GeneralParams.MINIMA_HOST=(String)config.get("host");GeneralParams.MINIMA_PORT=((Number)config.get("port")).intValue();GeneralParams.P2P_ENABLED=(boolean)config.get("p2p");
            Object result;try{result=greeting.createGreeting()==greeting;}catch(Exception e){result="ERROR";}
            var state=new JSONObject();state.put("result",result);state.put("calls",calls);state.put("depth",depth);state.put("wire",MiniData.getMiniDataVersion(greeting).to0xString());state.put("extra",greeting.getExtraData());state.put("top",greeting.getTopBlock().toString());var chain=new JSONArray();for(var id:greeting.getChain())chain.add(id.to0xString());state.put("chain",chain);states.add(new JSONParser().parse(state.toString()));
        }
        return states;
    }
    public static void main(String[] args)throws Exception{GeneralParams.USE_SQL_COINDB=false;var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(startup((JSONObject)new JSONParser().parse(line))));}catch(Exception e){e.printStackTrace(System.err);System.out.println("\"ERROR\"");}}
}
