import java.io.*;
import java.net.*;
import org.minima.system.Main;
import org.minima.system.commands.send.send;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.kissvm.Contract;
import org.minima.system.network.p2p.P2PFunctions;
import org.minima.utils.json.*;
public class CoreMainsOracle extends NIOClientOracle {
    static JSONArray events=new JSONArray();static String fail,kind;
    static class Receiver extends Main {int calls;public void PostNotifyEvent(String name,JSONObject data){calls++;}}
    static void stage(String value)throws IOException{events.add(value);if(fail!=null&&value.startsWith(fail)){if(kind.equals("error"))throw new AssertionError("fixture");if(kind.equals("oom"))throw new OutOfMemoryError("fixture");throw new IOException("fixture");}}
    public static void main(String[] args)throws Exception {
        if(args.length>3){fail=args[2];kind=args[3];}
        URL.setURLStreamHandlerFactory(protocol->protocol.equals("https")?new URLStreamHandler(){protected URLConnection openConnection(URL url)throws IOException{stage("open:"+url);return new URLConnection(url){public void connect()throws IOException{stage("connect");}public InputStream getInputStream()throws IOException{stage("input");return new ByteArrayInputStream(new byte[0]){public void close()throws IOException{stage("close");}};}};}}:null);
        var receiver=(Receiver)U.allocateInstance(Receiver.class);field(Main.class,"mMainInstance").set(null,receiver);
        PrintStream original=System.out;var buffer=new ByteArrayOutputStream();System.setOut(new PrintStream(buffer,true,java.nio.charset.StandardCharsets.UTF_8){public void println(String text){if(text.startsWith("Minima @ "))text=text.substring(text.indexOf(" : ")+3);super.println(text);}});
        String error="none";
        try{String[] forwarded=args[1].equals("null")?null:args[1].equals("args")?new String[]{"unused","value"}:new String[0];switch(args[0]){case "send":send.main(forwarded);break;case "TxPoWTreeNode":TxPoWTreeNode.main(forwarded);break;case "Contract":Contract.main(forwarded);break;case "P2PFunctions":P2PFunctions.main(forwarded);break;default:throw new AssertionError("unknown main");}}
        catch(Throwable e){error=e.toString();}
        System.out.flush();System.setOut(original);var result=new JSONObject();result.put("stdout",buffer.toString(java.nio.charset.StandardCharsets.UTF_8));result.put("error",error);result.put("events",events);result.put("notifications",receiver.calls);System.out.println(result);
    }
}
