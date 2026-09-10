import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.*;
public class TxnDBOracle {
    static TxnRow row(String id,int n){var tx=new Transaction();var witness=new Witness();for(int i=0;i<n%4;i++)tx.addInput(new Coin(new MiniData(i==0&&n%3==0?"0x03":"0x"+Integer.toHexString(n+i+1)),new MiniData("0xabcd"),new MiniNumber(i+10),Token.TOKENID_MINIMA));for(int i=0;i<n%5;i++)tx.addOutput(new Coin(Coin.COINID_OUTPUT,new MiniData("0x1234"),new MiniNumber(i+1),Token.TOKENID_MINIMA));for(int i=0;i<n%3;i++)tx.addStateVariable(new StateVariable(i,"[hello "+n+"]"));if(n%2==0)witness.addScript(new ScriptProof("RETURN TRUE"));return new TxnRow(id,tx,witness);}
    static String error(Exception e){return e instanceof IOException?"IO_ERROR":"ERROR";}
    static Object snapshot(TxnRow row){if(row==null)return null;var v=new JSONObject();v.put("id",row.getID());var tx=row.getTransaction();v.put("inputs",tx==null?-1:tx.getAllInputs().size());v.put("outputs",tx==null?-1:tx.getAllOutputs().size());v.put("state",tx==null?-1:tx.getCompleteState().size());var w=row.getWitness();v.put("scripts",w==null?-1:w.getAllScripts().size());return v;}
    static JSONArray ids(ArrayList<TxnRow> rows){var result=new JSONArray();for(var r:rows)result.add(r==null?null:r.getID());return result;}
    static Object run(JSONObject req)throws Exception {
        String kind=(String)req.get("kind");
        if(kind.equals("fixtures")){var list=new JSONArray();for(int i=0;i<30;i++){var r=row("txn-"+i+"😀",i);var db=new TxnDB();db.addCompleteTransaction(r);db.createTransaction("empty");var v=new JSONObject();v.put("row",MiniData.getMiniDataVersion(r).to0xString());v.put("db",MiniData.getMiniDataVersion(db).to0xString());list.add(v);}return list;}
        if(kind.equals("wire")){
            byte[] bytes=new MiniData((String)req.get("data")).getBytes();var in=new DataInputStream(new ByteArrayInputStream(bytes));var out=new JSONObject();boolean isRow=(boolean)req.get("row"),convert=(boolean)req.get("convert");TxnRow r=row("old",3);TxnDB db=new TxnDB();db.createTransaction("old");
            try {if(isRow){if(convert)r=TxnRow.convertMiniDataVersion(new MiniData(bytes));else r.readDataStream(in);out.put("result",r==null?null:r.toJSON((boolean)req.get("show")));out.put("wire",r==null?null:MiniData.getMiniDataVersion(r).to0xString());}else{if(convert)db=TxnDB.convertMiniDataVersion(new MiniData(bytes));else db.readDataStream(in);out.put("result",db==null?null:ids(db.listTxns()));out.put("wire",db==null?null:MiniData.getMiniDataVersion(db).to0xString());}}catch(Exception e){out.put("error",error(e));}
            if(!convert){out.put("consumed",bytes.length-in.available());out.put("partial",isRow?snapshot(r):ids(db.listTxns()));}return out;
        }
        var user=MinimaDB.getDB().getUserDB();user.getAllData().clear();var db=new TxnDB();var rows=new ArrayList<TxnRow>();var held=new ArrayList<ArrayList<TxnRow>>();var output=new JSONArray();
        for(Object op:(JSONArray)req.get("ops")){var o=(JSONObject)op;Object result="OK";try{String cmd=(String)o.get("cmd"),id=(String)o.get("id");int index=o.containsKey("index")?((Number)o.get("index")).intValue():0;
            switch(cmd){case "create":db.createTransaction(id);rows.add(db.listTxns().getLast());break;case "fixture":{var r=row(id,index);rows.add(r);db.addCompleteTransaction(r);break;}case "rename":rows.get(index).setID(id);break;case "add":db.addCompleteTransaction(rows.get(index));break;case "delete":result=db.deleteTransaction(id);break;case "get":{var r=db.getTransactionRow(id);result=r==null?-1:rows.indexOf(r);break;}case "clear":db.clearTxns();break;case "hold":held.add(db.listTxns());break;case "clearheld":held.get(index).clear();break;case "save":db.saveDB();break;case "load":db.loadDB();break;case "stored":user.saveCustomTransactions(new MiniData((String)o.get("value")));break;case "wire":result=MiniData.getMiniDataVersion(db).to0xString();break;case "json":result=rows.get(index).toJSON((boolean)o.get("show"));break;case "clearwitness":rows.get(index).clearWitness();break;default:throw new IllegalArgumentException(cmd);}
        }catch(Exception e){result=error(e);}var state=new JSONObject();state.put("result",result);state.put("list",ids(db.listTxns()));var aliases=new JSONArray();for(var h:held)aliases.add(ids(h));state.put("held",aliases);var rs=new JSONArray();for(var r:rows)rs.add(snapshot(r));state.put("rows",rs);output.add(state);}return output;
    }
    public static void main(String[] args)throws Exception {MinimaDB.createDB();var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception e){System.out.println("ERROR");}}
}
