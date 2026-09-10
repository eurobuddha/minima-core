import java.io.*;
import java.net.*;
import java.nio.channels.*;
import java.util.concurrent.*;
import org.minima.system.Main;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.*;
import org.minima.objects.base.MiniData;
import org.minima.utils.messages.Message;
import org.minima.utils.json.*;

/** Actual pinned server/client; controlled application receiver and selection key. */
public class NIOServerErrorsOracle extends NIOServerOracle {
    static String stage,kind,logKind;static int failLog,prints;static JSONArray events=new JSONArray();
    static NIOServer server;static CountDownLatch ready=new CountDownLatch(1),connected=new CountDownLatch(1);
    static NIOClient client;
    static synchronized void record(String text){events.add(text);}
    static Throwable failure(String type,String text){return switch(type){case "error"->new LinkageError(text);case "oom"->new OutOfMemoryError(text);case "io"->new IOException(text);default->new IllegalStateException(text);};}
    @SuppressWarnings("unchecked") static <T extends Throwable> void fail(Throwable error)throws T{throw (T)error;}
    static class Receiver extends Collector {
        public void PostMessage(Message message){
            String type=message.getMessageType();record("post:"+type+(message.exists("reconnect")?":"+message.getBoolean("reconnect"):""));
            if(type.equals("NIO_NEWCONNECT")){client=(NIOClient)message.getObject("client");connected.countDown();}
            boolean target=(stage.equals("startup")&&type.equals("NIO_SERVERSTARTED"))||((stage.equals("accept")||stage.equals("register"))&&type.equals("NIO_NEWCONNECT"))||(stage.equals("read")&&type.equals("NIO_NEWMSG"))||(stage.equals("disconnect")&&type.equals("NIO_DISCONNECTED"));
            if(target){server.shutdown();fail(failure(kind,"fixture"));}
            if(type.equals("NIO_SERVERSTARTED"))ready.countDown();
        }
    }
    static class Owner extends Main {
        Owner()throws Exception{super();}
        public void PostNotifyEvent(String event,JSONObject data){String full=data.getString("message"),text=full.substring(full.indexOf(" : ")+3);if(!text.startsWith("     "))record("notify:"+text);}
    }
    static class FailingKey extends Key {
        FailingKey(Channel c){super(c);}
        public SelectionKey interestOps(int value){record("interest");fail(failure(kind,"fixture"));return this;}
        public void cancel(){record("cancel");fail(failure(kind,"fixture"));}
    }
    static void await(CountDownLatch latch)throws Exception{require(latch.await(5,TimeUnit.SECONDS),"fixture timeout");}
    public static void main(String[] args)throws Exception{
        stage=args[0];kind=args[1];failLog=Integer.parseInt(args[2]);logKind=args[3];NIOServer.mTraceON=Boolean.parseBoolean(args[4]);
        var manager=(Receiver)U.allocateInstance(Receiver.class);manager.traffic=new NIOTraffic();var network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(network,"mNIOManager",manager);var owner=(Owner)U.allocateInstance(Owner.class);set(owner,"mNetwork",network);field(Main.class,"mMainInstance").set(null,owner);
        var output=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String full){String text=full.substring(full.indexOf(" : ")+3);if(text.startsWith("     "))return;record("log:"+text);if(++prints==failLog){server.shutdown();fail(failure(logKind,"logger"));}}
        });
        server=new NIOServer(0,manager);
        if(stage.equals("send")||stage.equals("all")||stage.equals("cancel")){
            var channel=new Channel();var key=new FailingKey(channel);client=new NIOClient(true,"127.0.0.1",1,channel,key);client.setValidGreeting(true);
            ((ConcurrentHashMap<String,NIOClient>)get(server,"mClients")).put(client.getUID(),client);
            try{if(stage.equals("cancel"))client.disconnect();else if(stage.equals("all"))server.sendMessageAll(new MiniData("0x01"));else server.sendMessage(client.getUID(),new MiniData("0x01"));record("returned");}catch(Throwable e){record("escaped:"+e);}
            record("queued:"+((java.util.ArrayList<?>)get(client,"mMessages")).size());record("closed:"+!channel.isOpen());channel.close();
        }else{
            var worker=new Thread(server,"nio-fixture");worker.setDaemon(true);worker.setUncaughtExceptionHandler((t,e)->record("uncaught:"+t.getName()+":"+e));worker.start();Socket peer=null;ServerSocket listener=null;SocketChannel outgoing=null;
            if(!stage.equals("startup")){
                await(ready);var selector=(Selector)get(server,"mSelector");int port=0;for(var key:selector.keys())if(key.channel() instanceof ServerSocketChannel ch)port=((InetSocketAddress)ch.getLocalAddress()).getPort();
                if(stage.equals("register")){listener=new ServerSocket(0,1,InetAddress.getByName("127.0.0.1"));outgoing=SocketChannel.open(new InetSocketAddress("127.0.0.1",listener.getLocalPort()));peer=listener.accept();server.regsiterNewSocket(outgoing);}
                else peer=connect(port,"127.0.0.1");
                // A trace logger may fail before NEWCONNECT is posted.
                if(!stage.equals("accept")&&!stage.equals("register")){await(connected);if(stage.equals("read"))peer.getOutputStream().write(new byte[]{0,0,0,1,42});else server.disconnect(client.getUID());}
            }
            worker.join(5000);require(!worker.isAlive(),"worker timeout");record("running:"+server.isRunning());
            record("registers:"+((java.util.ArrayList<?>)get(server,"mRegisterChannels")).size());record("disconnects:"+((java.util.ArrayList<?>)get(server,"mDisconnectChannels")).size());
            if(peer!=null)peer.close();if(outgoing!=null)outgoing.close();if(listener!=null)listener.close();
            var selector=(Selector)get(server,"mSelector");if(selector!=null){for(var key:selector.keys())key.channel().close();selector.close();}
        }
        record("clients:"+server.getNetClientSize());System.setOut(output);var result=new JSONObject();result.put("events",events);output.println(result.toJSONString());
    }
}
