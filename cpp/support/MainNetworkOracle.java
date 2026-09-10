import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.*;
import org.minima.system.network.p2p.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.objects.base.MiniData;
public class MainNetworkOracle extends NIOClientOracle {
    boolean statusFixture;NIOManager nio;NetworkManager network;ArrayList<NIOClient> clients=new ArrayList<>();
    static void stack(Object value)throws Exception{set(value,"mMessages",new LinkedList<Message>());set(value,"mLock",new Object());set(value,"mRunning",true);}
    MainNetworkOracle(JSONObject config,Main main)throws Exception{
        nio=(NIOManager)U.allocateInstance(NIOManager.class);stack(nio);set(nio,"mNIOServer",new NIOServer(0,nio));set(nio,"mTrafficListener",new NIOTraffic());set(nio,"mConnectingClients",new ConcurrentHashMap<String,NIOClient>());
        statusFixture=Boolean.TRUE.equals(config.get("statusFixture"));
        if(statusFixture){MessageProcessor.setTrace(false,"");GeneralParams.MINIMA_HOST=config.getString("host","fixture-host");GeneralParams.IS_HOST_SET=Boolean.TRUE.equals(config.get("hostset"));GeneralParams.MINIMA_PORT=9001;GeneralParams.RPC_PORT=9005;GeneralParams.RPC_ENABLED=false;var traffic=nio.getTrafficListener();set(traffic,"mStartTime",0L);traffic.addToTotalRead(12345);traffic.addToTotalWrite(23456);traffic.addReadBytes("fixture-read",1024);traffic.addWriteBytes("fixture-write",2048);}
        var peer=(P2PManager)U.allocateInstance(P2PManager.class);stack(peer);var state=new P2PState();set(peer,"state",state);
        var checker=(P2PPeersChecker)U.allocateInstance(P2PPeersChecker.class);stack(checker);set(checker,"verifiedPeers",new HashSet<java.net.InetSocketAddress>());set(checker,"unverifiedPeers",new HashSet<java.net.InetSocketAddress>());set(peer,"mPeersChecker",checker);
        if(config.containsKey("state")){var values=(JSONObject)config.get("state");if(values.containsKey("accept"))state.setAcceptingInLinks((Boolean)values.get("accept"));if(values.containsKey("startup"))state.setStartupComplete((Boolean)values.get("startup"));if(values.containsKey("discovery"))state.setDoingDiscoveryConnection((Boolean)values.get("discovery"));}
        network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(network,"mNIOManager",nio);set(main,"mNetwork",network);
        String kind=config.getString("peer","real");set(network,"mP2PManager",kind.equals("null")?null:kind.equals("wrong")?nio:peer);
        if(config.containsKey("clients"))for(Object raw:(JSONArray)config.get("clients")){var row=(JSONObject)raw;var channel=new Channel();var client=new NIOClient(false,"127.0.0.1",9001,channel,new Key(channel));set(client,"mUID",row.getString("uid"));if(statusFixture)set(client,"mTimeConnected",0L);client.setValidGreeting((Boolean)row.get("valid"));clients.add(client);((Map<String,NIOClient>)get(Boolean.TRUE.equals(row.get("connected"))?nio.getNIOServer():nio,Boolean.TRUE.equals(row.get("connected"))?"mClients":"mConnectingClients")).put(client.getUID(),client);}
        if(Boolean.TRUE.equals(config.get("nullNIO")))set(network,"mNIOManager",null);
    }
    JSONObject snapshot()throws Exception{
        var out=new JSONObject();var queued=new JSONArray();var queue=(LinkedList<Message>)get(nio,"mMessages");while(!queue.isEmpty()){var message=queue.removeFirst();var row=new JSONObject();row.put("type",message.getMessageType());for(String key:new String[]{"uid","host"})if(message.exists(key))row.put(key,message.getString(key));if(message.exists("port"))row.put("port",message.getInteger("port"));queued.add(row);}out.put("queued",queued);
        var rows=new JSONArray();for(var client:clients){var row=new JSONObject();row.put("uid",client.getUID());var packets=new JSONArray();for(var packet:(ArrayList<MiniData>)get(client,"mMessages"))packets.add(packet.to0xString());row.put("packets",packets);rows.add(row);}out.put("clients",rows);out.put("traffic",nio.getTrafficListener().getBreakdown());if(statusFixture){var traffic=nio.getTrafficListener();out.put("trace",nio.isTrace());out.put("filter",nio.getTraceFilter());out.put("host",GeneralParams.MINIMA_HOST);out.put("hostset",GeneralParams.IS_HOST_SET);out.put("totalRead",traffic.getTotalRead());out.put("totalWrite",traffic.getTotalWrite());out.put("start",traffic.getStartTime());}return out;
    }
}
