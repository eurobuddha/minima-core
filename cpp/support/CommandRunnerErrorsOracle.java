import java.io.*;
import java.util.*;
import org.minima.system.commands.*;
import org.minima.utils.MinimaLogger;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Actual runner with a controlled command body; no wallet/network actions. */
public class CommandRunnerErrorsOracle {
    static JSONArray events,logs;
    static int prints,failAt;
    static String loggerKind;
    static class Probe extends Command {
        Probe(String name){super(name,"error fixture");}
        public ArrayList<String> getValidParams(){return new ArrayList<>(Arrays.asList("kind","emit"));}
        public Command getFunction(){return new Probe(getName());}
        public JSONObject runCommand()throws Exception{
            String kind=getParam("kind","success");events.add(getName()+":"+kind);
            if(getBooleanParam("emit",false))MinimaLogger.log("command log");
            switch(kind){
                case "command":throw new CommandException("command failure");
                case "runtime":throw new IllegalStateException("runtime failure");
                case "io":throw new IOException("io failure");
                case "sql":throw new java.sql.SQLException("sql failure");
                case "sqlruntime":throw new IllegalStateException("sql runtime failure");
                case "format":throw new NumberFormatException("format failure");
                case "arithmetic":throw new ArithmeticException("arithmetic failure");
                case "error":throw new LinkageError("error failure");
                case "oom":throw new OutOfMemoryError("memory failure");
                case "null":return null;
                default:var result=getJSONReply();result.put("caller",getMiniDAPPID());
                    if(kind.equals("missingstatus"))result.remove("status");
                    if(kind.equals("nullstatus"))result.put("status",null);
                    if(kind.equals("wrongstatus"))result.put("status","true");
                    if(kind.equals("false"))result.put("status",false);
                    return result;
            }
        }
    }
    static void fail(){switch(loggerKind){case "error":throw new LinkageError("logger failure");case "oom":throw new OutOfMemoryError("logger failure");default:throw new IllegalStateException("logger failure");}}
    static JSONObject run(JSONObject request){
        events=new JSONArray();logs=new JSONArray();prints=0;failAt=((Number)request.get("failAt")).intValue();loggerKind=request.getString("loggerKind");
        var old=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String full){int at=full.indexOf("] : ");if(at<0)return;String body=full.substring(at+4);if(body.startsWith("     "))return;logs.add(body);if(++prints==failAt)fail();}
        });Object result;
        try{result=CommandRunner.getRunner().runMultiCommand(request.getString("caller"),request.getString("text"));}
        catch(Throwable error){result="escaped:"+error.toString();}
        finally{System.setOut(old);}
        var out=new JSONObject();out.put("result",result);out.put("events",events);out.put("logs",logs);return out;
    }
    public static void main(String[] args)throws Exception{
        CommandRunner.ALL_COMMANDS[0]=new Probe("fixture");CommandRunner.ALL_COMMANDS[1]=new Probe("send");
        var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null)System.out.println(run((JSONObject)new JSONParser().parse(line)).toJSONString());
    }
}
