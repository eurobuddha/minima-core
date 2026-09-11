import java.io.*;
import java.util.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Calls every public ItemList overload on the unmodified pinned class. */
public class ItemListOracle {
    static String hex(String text){if(text==null)return null;var out=new StringBuilder();for(char c:text.toCharArray())out.append(String.format("%04X",(int)c));return out.toString();}
    static int number(JSONObject value,String key){return ((Number)value.get(key)).intValue();}
    static class LimitedList extends ArrayList {
        public boolean add(Object value){if(size()==4)throw new IllegalStateException("fixture append limit");return super.add(value);}
    }
    static class Probe {
        String text="before";int calls=0;boolean fail=false,append=false;List target;
        public String toString(){calls++;if(append){append=false;target.add("added during render");}if(fail)throw new IllegalStateException("fixture render failure");return text;}
    }
    static Object observe(Object value){
        if(value instanceof String)return hex((String)value);
        if(value instanceof Probe){var p=(Probe)value;var out=new JSONObject();out.put("probe",p.text);out.put("calls",p.calls);return out;}
        return value;
    }
    static JSONArray snapshot(List list){if(list==null)return null;var out=new JSONArray();for(Object value:list)out.add(observe(value));return out;}
    static JSONArray run(JSONObject input){
        ItemList list=new ItemList(),other=new ItemList();List alias=list.getItems();var probe=new Probe();var outputs=new JSONArray();
        for(Object raw:(JSONArray)input.get("ops")){
            var op=(JSONObject)raw;var row=new JSONObject();Object result=null;
            try{
                String kind=op.getString("op"),s=(String)op.get("s"),sep=(String)op.get("sep");
                switch(kind){
                    case "new":{int arity=number(op,"arity");list=arity==0?new ItemList():arity==1?new ItemList(s):arity==2?new ItemList(s,sep):new ItemList(s,sep,Boolean.TRUE.equals(op.get("multi")));break;}
                    case "alias":alias=list.getItems();result=alias==list.getItems();break;
                    case "setSP":list.setSP(sep);break;
                    case "add":if(op.containsKey("index"))list.add(number(op,"index"),s);else list.add(s);break;
                    case "raw":alias.add(Boolean.TRUE.equals(op.get("probe"))?probe:op.get("value"));break;
                    case "rawSet":result=observe(alias.set(number(op,"index"),op.get("value")));break;
                    case "rawClear":alias.clear();break;
                    case "rawRemove":result=observe(alias.remove(number(op,"index")));break;
                    case "get":result=hex(list.get(number(op,"index")));break;
                    case "array":result=list.getArray();break;
                    case "join":result=hex(op.containsKey("sep")?list.toString(sep):list.toString());break;
                    case "clear":list.clear();break;
                    case "reset":list.reset();break;
                    case "other":other=new ItemList(s,sep,Boolean.TRUE.equals(op.get("multi")));break;
                    case "all":if(Boolean.TRUE.equals(op.get("object")))list.addAll(Boolean.TRUE.equals(op.get("null"))?null:Boolean.TRUE.equals(op.get("self"))?list:other);else if(op.containsKey("multi"))list.addAll(s,sep,Boolean.TRUE.equals(op.get("multi")));else if(op.containsKey("sep"))list.addAll(s,sep);else list.addAll(s);break;
                    case "split":{List target=Boolean.TRUE.equals(op.get("limited"))?new LimitedList():Boolean.TRUE.equals(op.get("null"))?null:Boolean.TRUE.equals(op.get("alias"))?alias:other.getItems();try{if(op.containsKey("multi"))list.split(s,sep,target,Boolean.TRUE.equals(op.get("multi")));else list.split(s,sep,target);}finally{row.put("append",snapshot(target));}break;}
                    case "probe":probe.text=s;probe.fail=Boolean.TRUE.equals(op.get("fail"));probe.append=Boolean.TRUE.equals(op.get("append"));probe.target=list.getItems();break;
                    default:throw new IllegalArgumentException(kind);
                }
                row.put("value",result);
            }catch(Exception error){row.put("exception",error instanceof ClassCastException?error.getClass().getName():error.toString());}
            row.put("items",snapshot(list.getItems()));row.put("alias",snapshot(alias));row.put("other",snapshot(other.getItems()));row.put("size",list.size());row.put("probeCalls",probe.calls);outputs.add(row);
        }
        return outputs;
    }
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)System.out.println(run((JSONObject)new JSONParser().parse(line)));}
}
