import java.io.*;
import java.util.*;
import org.minima.utils.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

// Unmodified pinned handler + logger. Only logger/platform inputs are controlled.
public class MinimaUncaughtOracle extends MinimaLoggerOracle {
    static int strings;
    static String text(){
        trace.add("toString");strings++;
        if(input.containsKey("failStringAt")&&strings==((Number)input.get("failStringAt")).intValue())throw new RuntimeException("fixture toString");
        var values=(JSONArray)input.get("texts");
        return values==null?(String)input.get("text"):(String)values.get(Math.min(strings-1,values.size()-1));
    }
    static StackTraceElement[] frames(){
        trace.add("stack");if(flag("failStack"))throw new RuntimeException("fixture stack");if(flag("nullFrames"))return null;
        var values=(JSONArray)input.get("frames");if(values==null)return new StackTraceElement[0];
        var result=new StackTraceElement[values.size()];for(int i=0;i<values.size();i++)result[i]=values.get(i)==null?null:new StackTraceElement("Fixture",(String)values.get(i),"Fixture.java",12);return result;
    }
    static class Ordinary extends Exception {public String toString(){return text();}public StackTraceElement[] getStackTrace(){return frames();}}
    static class FatalMemory extends OutOfMemoryError {public String toString(){return text();}public StackTraceElement[] getStackTrace(){return frames();}}
    static class FatalStore extends org.h2.mvstore.MVStoreException {FatalStore(){super(1,"fixture");}public String toString(){return text();}public StackTraceElement[] getStackTrace(){return frames();}}
    static class FatalTimeout extends java.util.concurrent.TimeoutException {public String toString(){return text();}public StackTraceElement[] getStackTrace(){return frames();}}
    static class Serious extends java.lang.Error {public String toString(){return text();}public StackTraceElement[] getStackTrace(){return frames();}}
    static Throwable error(){return flag("nullError")?null:switch(input.getString("kind","exception")){case "oom"->new FatalMemory();case "store"->new FatalStore();case "timeout"->new FatalTimeout();case "error"->new Serious();default->new Ordinary();};}
    static class Handler implements Thread.UncaughtExceptionHandler {
        String id;Handler(String value){id=value;}
        public void uncaughtException(Thread thread,Throwable error){trace.add("delegate:"+id+":"+thread.getName());if(flag("failDelegate"))throw new RuntimeException("fixture delegate");}
    }
    static void setup()throws Exception{
        owner=(Owner)U.allocateInstance(Owner.class);mainPresent(false);dates=prints=posts=0;nested=false;
        var dateField=field(MinimaLogger.class,"DATEFORMAT");U.putObject(U.staticFieldBase(dateField),U.staticFieldOffset(dateField),new Clock());
    }
    static JSONObject program(JSONObject request)throws Exception{
        setup();Thread.setDefaultUncaughtExceptionHandler(null);var handlers=new ArrayList<MinimaUncaughtException>();var result=new JSONArray();var previous=System.out;
        System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String value){var row=new JSONObject();row.put("print",value);trace.add(row);prints++;if(input.containsKey("failPrintAt")&&prints==((Number)input.get("failPrintAt")).intValue())throw new RuntimeException("fixture print");}
        });
        try{for(Object raw:(JSONArray)request.get("ops")){
            input=(JSONObject)raw;trace=new JSONArray();strings=prints=posts=dates=0;nested=false;mainPresent(flag("main"));var row=new JSONObject();
            try{switch(input.getString("op")){
                case "default":Thread.setDefaultUncaughtExceptionHandler(input.get("name")==null?null:new Handler((String)input.get("name")));break;
                case "bind":Thread.setDefaultUncaughtExceptionHandler(handlers.get(((Number)input.get("id")).intValue()));break;
                case "new":handlers.add(new MinimaUncaughtException());break;
                case "call":handlers.get(((Number)input.get("id")).intValue()).uncaughtException(flag("nullThread")?null:new Thread(input.getString("thread","worker")),error());break;
                default:throw new IllegalArgumentException("op");
            }}catch(Throwable e){row.put("exception",e.toString());}
            row.put("trace",trace);result.add(row);
        }}finally{System.setOut(previous);Thread.setDefaultUncaughtExceptionHandler(null);mainPresent(false);}
        var output=new JSONObject();output.put("rows",result);return output;
    }
    public static void main(String[] args)throws Exception{
        if(args.length>0){
            setup();input=new JSONObject();input.put("kind",args[0]);input.put("text","fatal-fixture");trace=new JSONArray();strings=0;
            Runtime.getRuntime().addShutdownHook(new Thread(()->System.out.println("SHUTDOWN-HOOK")));
            Thread.setDefaultUncaughtExceptionHandler((t,e)->System.out.println("DELEGATED"));
            var handler=new MinimaUncaughtException();
            if(args.length>1){Thread.setDefaultUncaughtExceptionHandler(handler);Thread thread=new Thread(()->{switch(args[0]){case "oom":throw new OutOfMemoryError("native-carrier");case "store":throw new org.h2.mvstore.MVStoreException(1,"native-carrier");case "timeout":sneaky(new java.util.concurrent.TimeoutException("native-carrier"));break;}} ,"fatal-worker");thread.start();thread.join();}
            else handler.uncaughtException(new Thread("fatal-worker"),error());
            System.out.println("RETURNED");System.exit(7);return;
        }
        var reader=new BufferedReader(new InputStreamReader(System.in));String line;while((line=reader.readLine())!=null)System.out.println(program((JSONObject)new JSONParser().parse(line)).toJSONString());
    }
    @SuppressWarnings("unchecked") static <T extends Throwable> void sneaky(Throwable error)throws T{throw (T)error;}
}
