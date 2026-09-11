import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.system.network.p2p.*;
import org.minima.system.network.p2p.params.P2PParams;
import org.minima.system.network.p2p.testingutils.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;

public class OriginalP2PHelpersOracle {
    static int integer(JSONObject q,String key) { return ((Number)q.get(key)).intValue(); }
    static JSONObject address(InetSocketAddress value,boolean randomPort) {
        var result=new JSONObject(); result.put("host",value.getAddress().getHostAddress());
        result.put("resolved",!value.isUnresolved()); result.put("port",randomPort?-1:value.getPort());
        result.put("portValid",randomPort?(value.getPort()>=7000&&value.getPort()<10000):(value.getPort()>=0&&value.getPort()<=65535));
        return result;
    }
    static JSONArray addresses(Collection<InetSocketAddress> values) {
        var result=new JSONArray(); for(var value:values) result.add(address(value,false)); return result;
    }
    static JSONObject peers(Map<String,InetSocketAddress> values,boolean randomPort) {
        var result=new JSONObject(); for(var key:values.keySet()) result.put(key,address(values.get(key),randomPort)); return result;
    }
    static JSONObject run(JSONObject q) {
        var result=new JSONObject();
        try {
            String op=q.getString("op"); int count=integer(q,"count"),port=integer(q,"port");
            if(op.equals("list")||op.equals("hostname")) {
                var values=op.equals("list")?QuickInetLists.generateInetSockAddrList((String)q.get("prefix"),count,port):
                    QuickInetLists.generateInetSockAddrWithHostnameList((String)q.get("hostname"),(String)q.get("prefix"),count,port);
                result.put("addresses",addresses(values)); var alias=values;
                var retained=values.isEmpty()?null:values.get(0); values.clear();
                result.put("aliasSize",alias.size()); result.put("retained",retained==null?null:address(retained,false));
            } else if(op.equals("map")) {
                var values=QuickInetLists.generateConnectionUIDMap((String)q.get("uid"),(String)q.get("prefix"),count,port);
                result.put("peers",peers(values,port<=0)); var alias=values; values.clear(); result.put("aliasSize",alias.size());
            } else {
                GeneralParams.MINIMA_PORT=integer(q,"minimaPort");
                P2PParams.TGT_NUM_LINKS=17; P2PParams.TGT_NUM_NONE_P2P_LINKS=29; P2PParams.LOOP_DELAY=12345;
                String mode=q.getString("mode"); int other=integer(q,"other");
                P2PState state=switch(mode) {
                    case "no"->QuickState.stateNoConnections(q.getBoolean("hasAddress"));
                    case "out"->QuickState.stateOutLinksOnly(count);
                    case "in"->QuickState.stateInLinksOnly(count,port);
                    case "known"->QuickState.stateInLinkKnownPeerOnly();
                    case "both"->QuickState.stateInAndOutLinks(count,other,port);
                    case "none"->QuickState.stateNoneP2PLink(count,port);
                    case "full"->QuickState.stateFullLinks(count,other,port);
                    default->throw new IllegalArgumentException("unknown mode");
                };
                result.put("own",address(state.getMyMinimaAddress(),false)); result.put("hostSet",state.isHostSet());
                result.put("in",peers(state.getInLinks(),!mode.equals("known")&&port<=0)); result.put("out",peers(state.getOutLinks(),mode.equals("out")||port<=0));
                result.put("none",peers(state.getNoneP2PLinks(),port<=0)); result.put("notAccepting",peers(state.getNotAcceptingConnP2PLinks(),port<=0));
                result.put("all",peers(state.getAllLinks(),false)); result.put("known",addresses(state.getKnownPeers()));
                result.put("accepting",state.isAcceptingInLinks()); result.put("maxP2P",state.getMaxNumP2PConnections());
                result.put("maxNone",state.getMaxNumNoneP2PConnections()); result.put("delay",state.getLoopDelay());
                result.put("discovery",state.isDoingDiscoveryConnection()); result.put("startup",state.isStartupComplete());
                result.put("noConnect",state.isNoConnect()); result.put("secret",state.getIpReqSecret().to0xString());
                var copy=state.getKnownPeersCopy(); var known=state.getKnownPeers();
                result.put("sameKnownPeer",mode.equals("known")&&state.getInLinks().get("knownPeer")==copy.get(0));
                known.clear(); result.put("copyAfterClear",copy.size()); result.put("knownAfterClear",state.getKnownPeers().size());
            }
        } catch(Exception error) { result.put("error",error.getClass().getName()); }
        return result;
    }
    public static void main(String[] args)throws Exception {
        var input=new BufferedReader(new InputStreamReader(System.in)); String line;
        while((line=input.readLine())!=null) System.out.println(run((JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line)));
    }
}
