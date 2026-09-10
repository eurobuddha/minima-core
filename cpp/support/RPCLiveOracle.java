import java.io.*;
import java.net.ServerSocket;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.system.commands.CommandRunner;
import org.minima.utils.RPCClient;
import org.minima.utils.json.*;
public class RPCLiveOracle extends NIOClientOracle {
    static int bound(MainNetworkOracle fixture)throws Exception{
        // HTTPServer publishes its startup log after assigning mServerSocket.
        // Acquire the same PrintStream monitor before reflecting that field.
        synchronized(System.out){
            Object server=get(fixture.network,"mRPCServer");if(server==null)return 0;
            ServerSocket socket=(ServerSocket)get(server,"mServerSocket");return socket==null||!socket.isBound()?0:socket.getLocalPort();
        }
    }
    static JSONObject state(MainNetworkOracle fixture)throws Exception{
        JSONObject out=new JSONObject();out.put("bound",bound(fixture));out.put("enabled",GeneralParams.RPC_ENABLED);out.put("authenticate",GeneralParams.RPC_AUTHENTICATE);out.put("password",GeneralParams.RPC_PASSWORD);out.put("users",MinimaDB.getDB().getUserDB().getRPCUsers());return out;
    }
    public static void main(String[] args)throws Exception{
        GeneralParams.resetDefaults();GeneralParams.RPC_PORT=0;GeneralParams.P2P_ENABLED=false;GeneralParams.P2P2_ENABLED=false;
        Main main=(Main)U.allocateInstance(Main.class);set(main,"mLock",new Object());set(main,"mMessages",new LinkedList<org.minima.utils.messages.Message>());field(Main.class,"mMainInstance").set(null,main);
        MinimaDB.createDB();MainNetworkOracle fixture=new MainNetworkOracle(new JSONObject(),main);
        try{
            BufferedReader input=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
            while((line=input.readLine())!=null){
                JSONObject request=(JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line),out=new JSONObject();String op=request.getString("op");long before=System.nanoTime();
                switch(op){
                    case "command":out.put("result",CommandRunner.getRunner().runMultiCommand(request.getString("text")));break;
                    case "configure":GeneralParams.RPC_CRLF=request.getBoolean("crlf");break;
                    case "replaceDB":MinimaDB.createDB();break;
                    case "wait":{long end=System.nanoTime()+5000000000L;while(bound(fixture)==0){if(System.nanoTime()>end)throw new IllegalStateException("RPC listener did not bind");Thread.sleep(2);}break;}
                    case "client":out.put("result",RPCClient.sendGETBasicAuth("http://127.0.0.1:"+bound(fixture)+"/"+request.getString("path"),request.getString("user"),request.getString("password")));break;
                    case "state":break;
                    default:throw new IllegalArgumentException(op);
                }
                out.put("elapsed",(System.nanoTime()-before)/1000000L);out.put("state",state(fixture));System.out.println(out);
            }
        }finally{fixture.network.stopRPC();field(Main.class,"mMainInstance").set(null,null);}
    }
}
