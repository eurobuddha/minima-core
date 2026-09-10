import java.io.*;
import java.util.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.*;
public class MessageOracle {
    static class Stack extends MessageStack {public Message pop(){return getNextMessage();}}
    static String repr(Message m) {if(m==null)return "null";if(m instanceof TimerMessage t){String text=t.toString();return "Timer:"+t.getTimer()+" Delay:"+(t.getTimer()-100)+" milli Msg:"+text.substring(text.indexOf(" Msg:")+5);}return m.toString();}
    static String str(JSONObject o,String k){return (String)o.get(k);}
    static long num(JSONObject o,String k){return ((Number)o.get(k)).longValue();}
    static Object value(JSONObject o){String type=str(o,"type");Object v=o.get("value");return switch(type){case "int" -> ((Number)v).intValue();case "float" -> Float.intBitsToFloat(((Number)v).intValue());case "bool","string","null" -> v;default -> throw new IllegalArgumentException(type);};}
    static JSONArray run(JSONObject req)throws Exception {
        var messages=new ArrayList<Message>();var stack=new Stack();var output=new JSONArray();
        for(Object obj:(JSONArray)req.get("ops")){var o=(JSONObject)obj;Object result="OK";try{String cmd=str(o,"cmd");Message m=o.containsKey("id")?messages.get((int)num(o,"id")):null;String key=str(o,"key");
            switch(cmd){
                case "new": messages.add(o.containsKey("value")?new Message(str(o,"value")):new Message());break;
                case "timer": {TimerMessage t=o.containsKey("id")?new TimerMessage(num(o,"delay"),m):new TimerMessage(num(o,"delay"),str(o,"value"));var f=TimerMessage.class.getDeclaredField("mTimer");f.setAccessible(true);f.setLong(t,1000+num(o,"delay"));messages.add(t);break;}
                case "put": {Object v=value(o);switch(str(o,"type")){case "int":m.addInteger(key,(int)v);break;case "float":m.addFloat(key,(float)v);break;case "bool":m.addBoolean(key,(boolean)v);break;case "string":m.addString(key,(String)v);break;default:m.addObject(key,v);}break;}
                case "putraw":m.getAllContents().put(key,value(o));break;
                case "remove":result=String.valueOf(m.getAllContents().remove(key));break;
                case "clearcontents":m.getAllContents().clear();break;
                case "settype":m.setMessageType(str(o,"value"));break;
                case "is":result=m.isMessageType(str(o,"value"));break;
                case "exists":result=m.exists(key);break;
                case "get":result=switch(str(o,"type")){case "int" -> ""+m.getInteger(key);case "float" -> ""+m.getFloat(key);case "bool" -> ""+m.getBoolean(key);case "default" -> ""+m.getBoolean(key,(boolean)o.get("value"));case "string" -> m.getString(key);default -> String.valueOf(m.getObject(key));};break;
                case "post":stack.PostMessage(m);break;
                case "pop":{Message popped=stack.pop();result=popped==null?-1:messages.indexOf(popped);break;}
                case "clear":stack.clear();break;
                case "except":{var list=new ArrayList<String>();for(Object v:(JSONArray)o.get("value"))list.add((String)v);stack.clearExcept(list);break;}
                case "contains":stack.clearExceptString(str(o,"value"));break;
                default:throw new IllegalArgumentException(cmd);
            }
        }catch(Exception e){result="ERROR";}
        var state=new JSONObject();state.put("result",result);state.put("size",stack.getSize());state.put("next",stack.isNextMessage());var list=new JSONArray();for(Message m:messages)list.add(repr(m));state.put("messages",list);output.add(state);
        }return output;
    }
    public static void main(String[] args)throws Exception {var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{System.out.println(run((JSONObject)new JSONParser().parse(line)));}catch(Exception e){System.out.println("ERROR");}}
}
