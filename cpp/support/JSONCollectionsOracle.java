import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.utils.json.*;
public class JSONCollectionsOracle {
    static JSONArray run(JSONObject input){
        boolean list=input.getString("kind").equals("array");
        JSONArray a=new JSONArray(),acopy=null,output=new JSONArray();
        JSONObject o=new JSONObject(),ocopy=null,child=new JSONObject();Iterator ai=null,oi=null;
        for(Object raw:(JSONArray)input.get("ops")){
            JSONObject op=(JSONObject)raw;String action=op.getString("op"),result="ok";
            try{
                switch(action){
                    case "add":a.add(op.get("value"));break;
                    case "remove":result=JSONValue.toJSONString(a.remove(op.getString("value")));break;
                    case "contains":result=JSONValue.toJSONString(a.contains(op.getString("value")));break;
                    case "put":o.put(op.getString("key"),op.get("value"));break;
                    case "get":result=JSONValue.toJSONString(list?a.get(((Number)op.get("index")).intValue()):o.get(op.getString("key")));break;
                    case "iter":if(list)ai=a.iterator();else oi=o.entrySet().iterator();break;
                    case "has":result=JSONValue.toJSONString(list?ai.hasNext():oi.hasNext());break;
                    case "next":
                        if(list)result=JSONValue.toJSONString(ai.next());
                        else{Map.Entry entry=(Map.Entry)oi.next();JSONArray pair=new JSONArray();pair.add(entry.getKey());pair.add(entry.getValue());result=pair.toJSONString();}break;
                    case "serial":result=list?a.toJSONString():o.toJSONString();break;
                    case "copy":if(list)acopy=new JSONArray(a);else ocopy=new JSONObject(o);break;
                    case "copySerial":result=list?acopy.toJSONString():ocopy.toJSONString();break;
                    case "replace":if(list)a=new JSONArray();else o=new JSONObject();break;
                    case "childPut":child.put(op.getString("key"),op.get("value"));break;
                    case "childStore":if(list)a.add(child);else o.put(op.getString("key"),child);break;
                    default:throw new IllegalArgumentException("Unknown operation");
                }
            }catch(Exception e){result=e.getClass().getName();}
            output.add(result);
        }
        return output;
    }
    public static void main(String[] args)throws Exception{
        BufferedReader input=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=input.readLine())!=null)System.out.println(run((JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line)));
    }
}
