import java.io.*;
import java.util.*;
import org.minima.system.commands.*;
import org.minima.utils.json.*;
public class HelpCommandOracle {
    static class Probe extends Command {
        final String full;
        Probe(String name,String shortHelp,String fullHelp){super(name,shortHelp);full=fullHelp;}
        public String getFullHelp(){return full;}
        public Command getFunction(){return new Probe(getName(),getHelp(),full);}
        public JSONObject runCommand(){throw new AssertionError("help must not execute target command");}
    }
    static JSONObject run(JSONObject q){
        var out=new JSONObject();var cmd=new help();var original=CommandRunner.ALL_COMMANDS[0];
        try{
            if(q.containsKey("probe")){var p=(JSONObject)q.get("probe");CommandRunner.ALL_COMMANDS[0]=new Probe((String)p.get("name"),(String)p.get("help"),(String)p.get("fullhelp"));}
            cmd.getParams().putAll((JSONObject)q.get("params"));String op=q.getString("op");
            if(op.equals("registry")){var names=new TreeSet<String>();for(var c:CommandRunner.ALL_COMMANDS)names.add(c.getName());out.put("names",new JSONArray(names));}
            else if(op.equals("meta")){out.put("name",cmd.getName());out.put("help",cmd.getHelp());out.put("fullhelp",cmd.getFullHelp());out.put("params",cmd.getValidParams());var fresh=cmd.getFunction();out.put("fresh",fresh.getJSONReply());out.put("distinct",fresh!=cmd&&fresh.getParams()!=cmd.getParams());}
            else if(op.equals("pad")){String text=null;if(q.get("units")!=null){var units=(JSONArray)q.get("units");var chars=new char[units.size()];for(int i=0;i<chars.length;i++)chars[i]=(char)((Number)units.get(i)).intValue();text=new String(chars);}var value=cmd.getStrOfLength(((Number)q.get("length")).intValue(),text);var units=new JSONArray();for(int i=0;i<value.length();i++)units.add((int)value.charAt(i));out.put("units",units);}
            else if(op.equals("runner"))out.put("value",CommandRunner.getRunner().runMultiCommand(q.getString("caller"),q.getString("text")));
            else{var reply=cmd.runCommand();if(op.equals("alias"))cmd.getParams().put("later","changed");out.put("value",reply);out.put("serialized",reply.toJSONString());}
        }catch(Throwable e){out.put("exception",e.getClass().getName());if(!(e instanceof NullPointerException)&&!(e instanceof ClassCastException))out.put("message",e.getMessage());}
        finally{CommandRunner.ALL_COMMANDS[0]=original;}
        return out;
    }
    public static void main(String[] args)throws Exception{var reader=new BufferedReader(new InputStreamReader(System.in));String line;while((line=reader.readLine())!=null)System.out.println(run((JSONObject)JSONValue.parseWithException(line)).toJSONString());}
}
