import java.io.*;
import java.net.*;
import java.nio.channels.*;
import java.util.concurrent.*;
import org.minima.system.Main;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.*;
import org.minima.utils.messages.Message;
import org.minima.utils.json.*;

/** The actual private connectAttempt runnable; no Main/node startup. */
public class NIOConnectorErrorsOracle extends NIOServerErrorsOracle {
    static String mode;static volatile Thread connector;static CountDownLatch logged=new CountDownLatch(1);
    static class Manager extends NIOManager {
        Manager(){super(null);}
        public void PostMessage(Message message){record("post:"+message.getMessageType());}
    }
    public static void main(String[] args)throws Exception{
        mode=args[0];kind=args[1];var output=System.out;var manager=(Manager)U.allocateInstance(Manager.class);
        var pending=new ConcurrentHashMap<String,NIOClient>();set(manager,"mConnectingClients",pending);
        var network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(network,"mNIOManager",manager);var main=(Main)U.allocateInstance(Main.class);set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);
        var server=new NIOServer(0,manager);server.shutdown();server.run();set(manager,"mNIOServer",server);events=new JSONArray();
        ServerSocket listener=null;int port=-1;if(!mode.equals("invalid")){listener=new ServerSocket(0,1,InetAddress.getByName("127.0.0.1"));port=listener.getLocalPort();}
        var client=new NIOClient("127.0.0.1",port);set(client,"mUID","fixture");pending.put(client.getUID(),client);
        Thread.setDefaultUncaughtExceptionHandler((t,e)->record("uncaught:"+t.getName()+":"+e));
        System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String full){String text=full.substring(full.indexOf(" : ")+3);if(text.startsWith("     "))return;connector=Thread.currentThread();record("log:"+text);logged.countDown();prints++;
                if(mode.equals("catch")){if(prints==1)fail(failure("runtime","success logger"));else fail(failure(kind,"catch logger"));}
                else if(prints==1)fail(failure(kind,"logger"));
            }
        });
        var method=NIOManager.class.getDeclaredMethod("connectAttempt",NIOClient.class);method.setAccessible(true);method.invoke(manager,client);await(logged);connector.join(5000);require(!connector.isAlive(),"connector timeout");
        record("pending:"+pending.size());record("registers:"+((java.util.ArrayList<?>)get(server,"mRegisterChannels")).size());
        if(listener!=null){listener.close();for(Object ch:(java.util.ArrayList<?>)get(server,"mRegisterChannels"))((SocketChannel)ch).close();}((Selector)get(server,"mSelector")).close();Thread.setDefaultUncaughtExceptionHandler(null);System.setOut(output);var result=new JSONObject();result.put("events",events);output.println(result.toJSONString());
    }
}
