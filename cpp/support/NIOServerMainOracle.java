import java.io.*;
import org.minima.system.network.minima.NIOServer;
import org.minima.utils.json.JSONObject;
public class NIOServerMainOracle {
    static PrintStream output=System.out;
    static synchronized void emit(String event,String value){var row=new JSONObject();row.put("event",event);row.put("value",value);output.println(row);}
    public static void main(String[] args)throws Exception{
        int interruptAt=Integer.parseInt(args[0]);int[] stage={0};
        System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String text){text=text.substring(text.indexOf(" : ")+3);emit("log",text);if(text.equals("Started.. waiting..")||text.equals("Started again.. waiting..")||text.equals("Stopping..")){if(++stage[0]==interruptAt)Thread.currentThread().interrupt();}}
        });
        Thread.setDefaultUncaughtExceptionHandler((thread,error)->emit("uncaught",thread.getName()+":"+error.getClass().getName()));Runtime.getRuntime().addShutdownHook(new Thread(()->emit("hook","exit")));
        try{NIOServer.main(null);emit("returned","unexpected");}catch(Exception e){emit("caught",e.getClass().getName()+":"+Thread.interrupted());System.in.read();Runtime.getRuntime().halt(7);}
    }
}
