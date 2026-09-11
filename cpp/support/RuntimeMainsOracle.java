import java.io.*;
import java.net.*;
import org.minima.system.Main;
import org.minima.system.brains.TxPoWMiner;
import org.minima.objects.keys.TreeKey;
import org.minima.database.cascade.Cascade;
import org.minima.system.commands.send.send;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.kissvm.Contract;
import org.minima.system.network.p2p.P2PFunctions;
import org.minima.utils.json.*;
public class RuntimeMainsOracle extends NIOClientOracle {
    static JSONArray events=new JSONArray();static String fail,kind;
    static class Receiver extends Main {int calls;public void PostNotifyEvent(String name,JSONObject data){calls++;}}
    static void stage(String value)throws IOException{events.add(value);if(fail!=null&&value.startsWith(fail)){if(kind.equals("error"))throw new AssertionError("fixture");if(kind.equals("oom"))throw new OutOfMemoryError("fixture");throw new IOException("fixture");}}
    public static void main(String[] args)throws Exception {
        var receiver=(Receiver)U.allocateInstance(Receiver.class);field(Main.class,"mMainInstance").set(null,receiver);
        PrintStream original=System.out;var buffer=new ByteArrayOutputStream();System.setOut(new PrintStream(buffer,true,java.nio.charset.StandardCharsets.UTF_8){public void println(String text){if(text.startsWith("Minima @ "))text=text.substring(text.indexOf(" : ")+3);super.println(text);}});
        String error="none";
        try{String[] forwarded=args[1].equals("null")?null:args[1].equals("args")?new String[]{"unused","value"}:new String[0];switch(args[0]){case "TxPoWMiner":TxPoWMiner.main(forwarded);break;case "TreeKey":TreeKey.main(forwarded);break;case "Cascade":Cascade.main(forwarded);break;default:throw new AssertionError("unknown main");}}
        catch(Throwable e){error=e.toString();}
        System.out.flush();System.setOut(original);var result=new JSONObject();result.put("stdout",buffer.toString(java.nio.charset.StandardCharsets.UTF_8));result.put("error",error);result.put("notifications",receiver.calls);System.out.println(result);
    }
}
