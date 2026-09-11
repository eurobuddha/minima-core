import java.io.*;
import org.minima.system.Main;
import org.minima.utils.*;
import org.minima.utils.json.*;
public class UtilityMainsOracle extends NIOClientOracle {
    static class Raw extends JSONObject {String text;int renders;Raw(String value){text=value;}public String toString(){renders++;return text;}}
    static class Receiver extends Main {int notifications;Receiver()throws Exception{super();}public void PostNotifyEvent(String event,JSONObject data){notifications++;}}
    public static void main(String[] args)throws Exception{
        PrintStream output=System.out;System.setOut(new PrintStream(output){public void println(String text){if(text.startsWith("Minima @ "))text=text.substring(text.indexOf(" : ")+3);output.println(text);}});
        if(args[0].equals("debug")||args[0].equals("hang")){
            var raw=new Raw(args[1]);boolean debug=Boolean.parseBoolean(args[2]);int fail=Integer.parseInt(args[3]);int[] calls={0};var logs=new JSONArray();var receiver=(Receiver)U.allocateInstance(Receiver.class);field(Main.class,"mMainInstance").set(null,receiver);
            System.setOut(new PrintStream(OutputStream.nullOutputStream()){public void println(String text){logs.add(text.substring(text.indexOf(" : ")+3));if(++calls[0]==fail)throw new IllegalStateException("logger");}});
            var result=new JSONObject();Object value=null;String error="none";if(args[0].equals("hang"))output.println("ENTER");try{value=MiniFormat.JSONPretty(raw,debug);}catch(Exception e){error=e.toString();}result.put("output",value);result.put("error",error);result.put("logs",logs);result.put("notifications",receiver.notifications);result.put("renders",raw.renders);output.println(result);return;
        }
        try{switch(args[0]){case "Maths":Maths.main(null);break;case "BaseConverter":BaseConverter.main(null);break;case "MiniFormat":MiniFormat.main(null);break;case "POW":org.minima.kissvm.functions.number.POW.main(null);break;case "REPLACE":org.minima.kissvm.functions.string.REPLACE.main(null);break;default:throw new IllegalArgumentException();}}
        catch(Exception e){System.out.println("EXCEPTION:"+e);}
    }
}
