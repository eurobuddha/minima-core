import java.io.*;
import java.util.*;
import java.util.concurrent.*;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.UserDB;
import org.minima.system.Main;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class NetworkManagerOracle extends NIOClientOracle {
    static int calls;static boolean failDB,nullDB;
    static class DB extends MinimaDB {public UserDB getUserDB(){++calls;if(failDB)throw new IllegalStateException("DB failure");return nullDB?null:new UserDB();}}
    static class Traffic extends NIOTraffic {long age;public long getStartTime(){return System.currentTimeMillis()-age;}}
    static long number(JSONObject request,String key,long fallback){return request.containsKey(key)?((Number)request.get(key)).longValue():fallback;}
    static boolean flag(JSONObject request,String key){return Boolean.TRUE.equals(request.get(key));}
    static Object runNetwork(JSONObject request)throws Exception{
        calls=0;failDB=flag(request,"failDB");nullDB=flag(request,"nullDB");field(MinimaDB.class,"mMinimaDB").set(null,U.allocateInstance(DB.class));
        GeneralParams.MINIMA_HOST=(String)request.get("host");GeneralParams.IS_HOST_SET=flag(request,"hostset");GeneralParams.MINIMA_PORT=(int)number(request,"port",9001);GeneralParams.RPC_PORT=(int)number(request,"rpcport",9005);GeneralParams.RPC_ENABLED=flag(request,"rpc");GeneralParams.P2P_ENABLED=false;GeneralParams.P2P2_ENABLED=flag(request,"p2p2");
        var nio=install();var network=Main.getInstance().getNetworkManager();set(nio,"mNIOServer",new NIOServer(0,nio));set(nio,"mConnectingClients",new ConcurrentHashMap<String,NIOClient>());
        var traffic=new Traffic();traffic.age=number(request,"age",0);nio.traffic=traffic;traffic.addToTotalRead(number(request,"read",0));traffic.addToTotalWrite(number(request,"write",0));traffic.addReadBytes("RPC",number(request,"read",0));traffic.addWriteBytes("NIO",number(request,"write",0));
        for(int kind=0;kind<2;++kind)for(int i=0;i<number(request,kind==0?"connecting":"connected",0);++i){var client=new NIOClient("127.0.0.1",i);set(client,"mUID",kind+":"+i);((Map<String,NIOClient>)get(kind==0?nio:nio.getNIOServer(),kind==0?"mConnectingClients":"mClients")).put(client.getUID(),client);}
        if(flag(request,"nullNIO"))set(network,"mNIOManager",null);
        Object status;try{if(request.get("op").equals("host")){network.calculateHostIP();status=GeneralParams.MINIMA_HOST;}else status=network.getStatus(flag(request,"all"));}catch(Exception error){status="ERROR";}
        var result=new JSONObject();result.put("result",status);result.put("dbCalls",calls);return result;
    }
    public static void main(String[] args)throws Exception{var output=System.out;System.setOut(System.err);var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{output.println(JSONValue.toJSONString(runNetwork((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace(System.err);output.println("\"ERROR\"");}}
}
