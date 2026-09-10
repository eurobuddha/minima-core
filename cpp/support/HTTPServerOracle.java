import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import org.minima.system.Main;
import org.minima.system.network.rpc.HTTPServer;
import org.minima.system.network.rpc.CMDHandler;
import org.minima.system.params.GeneralParams;
import org.minima.database.MinimaDB;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class HTTPServerOracle extends NIOClientOracle {
    static final Object lock=new Object();static JSONArray logs=new JSONArray();static int factories,active,completed,maximum;static boolean finished,rpc;static CountDownLatch ready=new CountDownLatch(1),done=new CountDownLatch(1);
    static class LogMain extends Main {public void PostNotifyEvent(String name,JSONObject data){if(name.equals("MINIMALOG")){String message=(String)data.get("message");synchronized(lock){logs.add(message.substring(message.indexOf(" : ")+3));}ready.countDown();}}}
    static class Listener extends HTTPServer {
        Listener(int port,boolean auto){super(port,auto);}
        public void run(){try{super.run();}finally{synchronized(lock){finished=true;}done.countDown();}}
        public Runnable getSocketHandler(Socket socket){synchronized(lock){++factories;}return ()->{synchronized(lock){++active;maximum=Math.max(maximum,active);lock.notifyAll();}try{if(rpc)new CMDHandler(socket).run();else{socket.setSoTimeout(5000);byte[] bytes=socket.getInputStream().readNBytes(5);for(int a=0,b=bytes.length-1;a<b;a++,b--){byte value=bytes[a];bytes[a]=bytes[b];bytes[b]=value;}socket.getOutputStream().write(bytes);}}catch(Exception error){}finally{try{socket.close();}catch(Exception error){}synchronized(lock){--active;++completed;lock.notifyAll();}}};}
    }
    public static void main(String[] args)throws Exception{
        // Reuse SearcherOracle's separate protocol stream; worker logs must not
        // occupy or be parsed as replies on the interactive control channel.
        var output=System.out;System.setOut(System.err);
        field(Main.class,"mMainInstance").set(null,U.allocateInstance(LogMain.class));Listener server=null;var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null)try{var request=(JSONObject)new JSONParser().parse(line);String kind=(String)request.get("op");switch(kind){
            case "create":{rpc=Boolean.TRUE.equals(request.get("rpc"));if(rpc){MinimaDB.createDB();if(request.containsKey("users"))MinimaDB.getDB().getUserDB().getAllData().put("rpcusers_allusers",request.get("users"));GeneralParams.RPC_AUTHENTICATE=(boolean)request.get("authenticate");GeneralParams.RPC_CRLF=(boolean)request.get("crlf");GeneralParams.RPC_PASSWORD="test-password";GeneralParams.RPC_AUTHSTYLE=(String)request.get("style");}boolean auto=Boolean.TRUE.equals(request.get("auto"));server=new Listener(((Number)request.get("port")).intValue(),auto);if(auto&&!ready.await(5,TimeUnit.SECONDS))throw new IllegalStateException("Listener startup timed out");break;}
            case "start":server.start();if(!ready.await(5,TimeUnit.SECONDS))throw new IllegalStateException("Listener startup timed out");break;
            case "status":{int target=((Number)request.get("completed")).intValue();int targetActive=request.containsKey("active")?((Number)request.get("active")).intValue():0;long end=System.currentTimeMillis()+5000;synchronized(lock){while(completed<target||active<targetActive){long left=end-System.currentTimeMillis();if(left<=0)throw new IllegalStateException("Handlers did not finish");lock.wait(left);}}break;}
            case "shutdown":server.shutdown();if(!done.await(5,TimeUnit.SECONDS))throw new IllegalStateException("Listener did not stop");break;
            default:throw new IllegalArgumentException(kind);
        }
        var row=new JSONObject();row.put("port",server.getPort());var socket=(ServerSocket)get(server,"mServerSocket");if(socket==null&&ready.getCount()==0)done.await(5,TimeUnit.SECONDS);row.put("bound",socket==null?0:socket.getLocalPort());synchronized(lock){row.put("logs",logs);row.put("factories",factories);row.put("completed",completed);row.put("concurrent",maximum>=2);row.put("finished",finished);}output.println(row.toJSONString());
        }catch(Exception error){error.printStackTrace(System.err);output.println("\"ERROR\"");}
        if(server!=null)server.shutdown();
    }
}
