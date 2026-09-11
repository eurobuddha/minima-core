import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.database.txpowdb.TxPoWDB;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.objects.*;
import org.minima.objects.base.MiniData;
import org.minima.utils.json.*;
public class TxBodyIDsOracle {
    static MiniData item(Object value){return value==null?null:new MiniData((String)value);}
    static String text(MiniData value){return value==null?null:value.to0xString();}
    static int number(JSONObject q,String key){return ((Number)q.get(key)).intValue();}
    static JSONArray snapshot(ArrayList<MiniData> list){var result=new JSONArray();for(var value:list)result.add(text(value));return result;}
    static boolean remove(ArrayList<MiniData> list,MiniData value){for(int i=0;i<list.size();i++){var old=list.get(i);if(value==null?old==null:old!=null&&value.isEqual(old)){list.remove(i);return true;}}return false;}
    static void mutate(MiniData value)throws IOException{value.readDataStream(new DataInputStream(new ByteArrayInputStream(MiniData.getMiniDataVersion(new MiniData("0x99")).getBytes())));}
    static JSONArray collection(JSONObject q){
        var owner=new TxPoW();var list=owner.getBlockTransactions();MiniData held=null;
        for(Object value:(JSONArray)q.get("initial"))list.add(item(value));
        Iterator<MiniData> iterator=null;var output=new JSONArray();
        for(Object raw:(JSONArray)q.get("ops")){
            var op=(JSONObject)raw;String action=op.getString("op");Object result="ok";
            try{
                switch(action){
                    case "iter":iterator=list.iterator();break;
                    case "has":result=iterator.hasNext();break;
                    case "next":result=text(iterator.next());break;
                    case "add":list.add(item(op.get("value")));break;
                    case "get":result=text(list.get(number(op,"index")));break;
                    case "set":result=text(list.set(number(op,"index"),item(op.get("value"))));break;
                    case "remove":result=remove(list,item(op.get("value")));break;
                    case "clear":list.clear();break;
                    case "replace":owner.getTxBody().mTxPowIDList=new ArrayList<>();list=owner.getBlockTransactions();break;
                    case "release":owner.clearBody();list=null;break;
                    case "serial":result=snapshot(list);break;
                    case "retain":held=list.get(number(op,"index"));break;
                    case "held":result=text(held);break;
                    case "addHeld":list.add(held);break;
                    case "mutate":mutate(held);break;
                    case "sameList":result=owner.hasBody()&&list==owner.getBlockTransactions();break;
                    default:throw new IllegalArgumentException("Unknown operation");
                }
            }catch(Exception e){result=e.getClass().getName();}
            output.add(result);
        }
        return output;
    }
    // Reuses the actual TxPoWDB subclass boundary already used by NIOMessageOracle.
    static class Pool extends TxPoWDB {
        TxPoW pow;ArrayList<MiniData> ids;JSONObject q;JSONArray calls=new JSONArray();int count=0;
        public boolean exists(String id){
            calls.add(id);++count;int trigger=number(q,"trigger");
            if(count==trigger)try{
                switch(q.getString("action")){
                    case "add":ids.add(new MiniData("0x99"));break;
                    case "clear":ids.clear();break;
                    case "set":ids.set(number(q,"index"),item(q.get("value")));break;
                    case "remove":remove(ids,item(q.get("value")));break;
                    case "replace":pow.getTxBody().mTxPowIDList=new ArrayList<>();pow.addBlockTxPOW(new MiniData("0xAA"));break;
                    case "clearOwner":pow.clearBody();break;
                    case "mutate":mutate(ids.get(number(q,"index")));break;
                }
            }catch(IOException e){throw new RuntimeException(e);}
            return !(count==trigger&&q.getBoolean("stop"));
        }
    }
    static JSONObject consumer(JSONObject q){
        var pow=new TxPoW();var ids=pow.getBlockTransactions();for(Object value:(JSONArray)q.get("initial"))ids.add(item(value));
        var node=new TxPoWTreeNode(pow);var pool=new Pool();pool.pow=pow;pool.ids=ids;pool.q=q;var result=new JSONObject();
        try{result.put("first",node.checkFullTxns(pool));}catch(Exception e){result.put("first",e.getClass().getName());}
        result.put("calls",pool.calls);result.put("heldList",snapshot(ids));result.put("bodyList",snapshot(pow.getBlockTransactions()));result.put("hasBody",pow.hasBody());
        if(pow.hasBody())pow.addBlockTxPOW(new MiniData("0xEE"));
        result.put("beforeSecond",snapshot(pow.getBlockTransactions()));
        var again=new JSONArray();var second=new TxPoWDB(){public boolean exists(String id){again.add(id);return false;}};
        try{result.put("second",node.checkFullTxns(second));}catch(Exception e){result.put("second",e.getClass().getName());}
        result.put("again",again);return result;
    }
    public static void main(String[] args)throws Exception{
        var reader=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=reader.readLine())!=null){var q=(JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line);System.out.println(q.getString("kind").equals("collection")?collection(q):consumer(q));}
    }
}
