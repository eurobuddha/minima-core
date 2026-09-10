import java.io.*;
import java.net.*;
import java.util.concurrent.*;
import org.minima.system.network.rpc.HTTPServer;
import org.minima.utils.json.*;

public class HTTPServerErrorsOracle extends NIOClientOracle {
    static String kind,stage;static int failLog,prints;static JSONArray events=new JSONArray();
    static CountDownLatch ready=new CountDownLatch(1),finished=new CountDownLatch(1),handled=new CountDownLatch(1);
    static synchronized void record(String value){events.add(value);}
    static Throwable failure(){return switch(kind){
        case "io"->new IOException("fixture");case "socket"->new SocketException("fixture");case "bind"->new BindException("fixture");
        case "error"->new LinkageError("fixture");case "oom"->new OutOfMemoryError("fixture");default->new IllegalStateException("fixture");
    };}
    static class Listener extends HTTPServer {
        Listener(int port){super(port,false);}
        public void run(){try{super.run();}finally{record("finished");finished.countDown();ready.countDown();}}
        public Runnable getSocketHandler(Socket socket){
            record("factory");try{socket.close();}catch(IOException e){throw new RuntimeException(e);}
            if(stage.equals("factory")||stage.equals("stopped")){if(stage.equals("stopped"))shutdown();sneaky(failure());}
            return ()->{record("handler");sneaky(failure());};
        }
    }
    @SuppressWarnings("unchecked") static <T extends Throwable> void sneaky(Throwable e)throws T{throw (T)e;}
    public static void main(String[] args)throws Exception{
        stage=args[0];kind=args[1];int port=Integer.parseInt(args[2]);failLog=Integer.parseInt(args[3]);var output=System.out;
        System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String full){String text=full.substring(full.indexOf(" : ")+3);if(text.startsWith("     "))return;record("log:"+text);prints++;if(prints==failLog)throw new IllegalArgumentException("logger");if(text.startsWith("Server started"))ready.countDown();}
        });
        Thread.setDefaultUncaughtExceptionHandler((t,e)->{record("uncaught:"+t.getName()+":"+e.toString());handled.countDown();if(kind.equals("defaultthrows"))throw new RuntimeException("default");});
        Listener listener=new Listener(port);
        if(stage.equals("direct")){try{listener.run();}catch(Throwable e){record("direct:"+e.toString());}}
        else{
            listener.start();if(!ready.await(5,TimeUnit.SECONDS))throw new AssertionError("startup timeout");
            if(!stage.equals("startup")&&!stage.equals("invalid")&&!stage.equals("collision")){
                var socket=(ServerSocket)get(listener,"mServerSocket");try(Socket client=new Socket("127.0.0.1",socket.getLocalPort())){}
            }
            boolean caught=(stage.equals("factory")||stage.equals("stopped"))&&(kind.equals("io")||kind.equals("socket")||kind.equals("bind"))&&failLog==0;
            if(caught){if(!finished.await(5,TimeUnit.SECONDS))throw new AssertionError("caught failure did not return");}
            else if(!handled.await(5,TimeUnit.SECONDS))throw new AssertionError("missing uncaught handler");
        }
        record("running:"+get(listener,"mRunning"));record("bound:"+(get(listener,"mServerSocket")!=null));listener.shutdown();if(!finished.await(5,TimeUnit.SECONDS))throw new AssertionError("listener did not stop");
        Thread.setDefaultUncaughtExceptionHandler(null);System.setOut(output);var result=new JSONObject();result.put("events",events);System.out.println(result.toJSONString());
    }
}
