import java.io.*;
import org.minima.utils.TestUtils;
import org.minima.system.network.p2p.testingutils.QuickClients;
import org.minima.utils.json.*;
public class OriginalHelpersOracle {
 static int integer(JSONObject q,String key){return ((Number)q.get(key)).intValue();}
 static JSONObject run(JSONObject q){var r=new JSONObject();try{
  if(q.getString("op").equals("clients")){
   var clients=QuickClients.generateClientInfoList((String)q.get("prefix"),integer(q,"count"),integer(q,"port"),(String)q.get("uid"),q.getBoolean("incoming"));var rows=new JSONArray();for(var client:clients)rows.add(client.toJSON());r.put("clients",rows);var alias=clients;var old=clients.isEmpty()?null:clients.get(0);clients.clear();r.put("aliasSize",alias.size());r.put("retained",old==null?null:old.toJSON());
  }else{
   String id=q.getString("id");var tx=q.getBoolean("short")?TestUtils.getTxPoW(id):TestUtils.getTxPoW(id,integer(q,"block"),integer(q,"weight"));var second=TestUtils.getTxPoW(id);r.put("id",tx.getTxPoWID());r.put("block",tx.getBlockNumber().toString());r.put("weight",tx.getWeight().toString());r.put("isBlock",tx.isBlock());r.put("isTransaction",tx.isTransaction());r.put("parent",tx.getParentID().to0xString());r.put("hasBody",tx.hasBody());r.put("distinct",tx!=second);r.put("initialTransactions",tx.getTransactions().size());tx.addTestTransaction("child");r.put("transactions",tx.getTransactions().size());r.put("otherTransactions",second.getTransactions().size());
  }
 }catch(Exception e){r.put("error",e.getClass().getName());}return r;}
 public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)System.out.println(run((JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line)));}
}
