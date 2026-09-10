import java.io.*;
import java.text.*;
import java.util.*;
import org.minima.system.Main;
import org.minima.utils.MinimaLogger;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
// Actual pinned logger. Only DateFormat, PrintStream and Main receivers are
// controlled; Runtime heap text is normalized in the external comparator.
public class MinimaLoggerOracle extends NIOClientOracle {
    static JSONObject input;static JSONArray trace;static boolean mainPresent,nested;static int dates,prints,posts;static Owner owner;
    static final SimpleDateFormat original=MinimaLogger.DATEFORMAT;
    static boolean flag(String key){return Boolean.TRUE.equals(input.get(key));}
    static void mainPresent(boolean value){mainPresent=value;try{field(Main.class,"mMainInstance").set(null,value?owner:null);}catch(Exception e){throw new RuntimeException(e);}}
    static class Clock extends SimpleDateFormat {
        Clock(){super("dd/MM/yyyy HH:mm:ss",Locale.ENGLISH);}
        public StringBuffer format(Date date,StringBuffer buffer,FieldPosition position){trace.add("date");if(flag("failDate"))throw new RuntimeException("fixture date");return super.format(new Date(1700000000000L+1000*dates++),buffer,position);}
    }
    static class Error extends Exception {
        public String toString(){trace.add("toString");if(flag("failString"))throw new RuntimeException("fixture toString");return (String)input.get("text");}
        public StackTraceElement[] getStackTrace(){trace.add("stack");if(flag("failStack"))throw new RuntimeException("fixture stack");if(flag("nullFrames"))return null;var frames=(JSONArray)input.get("frames");var values=new StackTraceElement[frames.size()];for(int i=0;i<values.length;i++){String name=(String)frames.get(i);values[i]=name==null?null:new StackTraceElement("Fixture",name,"Fixture.java",12);}return values;}
    }
    static class Owner extends Main {
        Owner()throws Exception{super();}
        public void PostNotifyEvent(String event,JSONObject data){var row=new JSONObject();row.put("event",event);row.put("data",data);trace.add(row);posts++;if(flag("mutatePost"))data.put("after",true);if(flag("clearPost"))mainPresent(false);if(flag("nested")&&!nested){nested=true;MinimaLogger.log("nested",false);}if(flag("failPost"))throw new RuntimeException("fixture post");}
    }
    static JSONObject run(JSONObject request)throws Exception{
        input=request;var result=new JSONObject();if(input.containsKey("date")){result.put("date",original.format(new Date(Long.parseLong((String)input.get("date")))));return result;}
        trace=new JSONArray();dates=prints=posts=0;nested=false;owner=(Owner)U.allocateInstance(Owner.class);mainPresent(flag("main"));
        var dateField=field(MinimaLogger.class,"DATEFORMAT");U.putObject(U.staticFieldBase(dateField),U.staticFieldOffset(dateField),new Clock());
        var previous=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String value){var row=new JSONObject();row.put("print",value);trace.add(row);prints++;if(flag("clearPrint"))mainPresent(false);if(flag("setPrint"))mainPresent(true);if(flag("failPrint"))throw new RuntimeException("fixture print");}
        });
        try{boolean notify=flag("notify");Exception error=flag("nullError")?null:new Error();switch(input.getString("mode")){
            case "string":MinimaLogger.log((String)input.get("text"),notify);break;case "defaultString":MinimaLogger.log((String)input.get("text"));break;case "exception":MinimaLogger.log(error,notify);break;case "defaultException":MinimaLogger.log(error);break;case "uncaught":MinimaLogger.logUncaught(error,notify);break;default:throw new IllegalArgumentException(input.getString("mode"));
        }}catch(Exception error){result.put("exception",error.toString());}finally{System.setOut(previous);}
        result.put("trace",trace);result.put("prints",prints);result.put("posts",posts);result.put("main",mainPresent);return result;
    }
    public static void main(String[] args)throws Exception{var reader=new BufferedReader(new InputStreamReader(System.in));String line;while((line=reader.readLine())!=null)try{System.out.println(run((JSONObject)new JSONParser().parse(line)).toJSONString());}catch(Exception e){e.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
