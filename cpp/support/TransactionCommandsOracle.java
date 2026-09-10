import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.keys.*;
import org.minima.objects.mmr.*;
import org.minima.system.commands.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class TransactionCommandsOracle {
    static Object run(JSONObject request)throws Exception {
        if("fixtures".equals(request.get("kind")))return TxnDBOracle.run(request);
        var imported=org.minima.system.brains.TxPoWSearcher.class.getDeclaredField("mImportedTokens");imported.setAccessible(true);((ArrayList<?>)imported.get(null)).clear();
        var db=new TxnDB();var field=MinimaDB.class.getDeclaredField("mTxnDB");field.setAccessible(true);field.set(MinimaDB.getDB(),db);var results=new JSONArray();
        for(Object value:(JSONArray)request.get("ops")){
            var op=(JSONObject)value;Object result="OK";
            try {
                if(op.containsKey("fixture")){
                    var row=TxnDBOracle.row((String)op.get("id"),((Number)op.get("fixture")).intValue());row.getTransaction().mHaveCheckedMonotonic=true;row.getTransaction().mIsMonotonic=true;
                    row.getWitness().addCoinProof(new CoinProof(new Coin(new MiniData("0x01"),new MiniData("0xab"),MiniNumber.ONE,Token.TOKENID_MINIMA),new MMRProof()));
                    var signature=new Signature();signature.addSignatureProof(new SignatureProof(new MiniData("0x11"),new MiniData("0x22"),new MMRProof()));row.getWitness().addSignature(signature);db.addCompleteTransaction(row);
                } else if(op.containsKey("line"))result=CommandRunner.getRunner().runMultiCommand((String)op.getOrDefault("caller","0x00"),(String)op.get("line"));
                else {
                    var command=CommandRunner.getRunner().getCommandOnly((String)op.get("name"));command.getParams().putAll((JSONObject)op.get("params"));
                    if(Boolean.TRUE.equals(op.get("meta"))){var meta=new JSONObject();meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("valid",command.getValidParams());meta.put("fresh",command.getFunction().getJSONReply());result=meta;}
                    else result=command.runCommand();
                }
            } catch(CommandException error){result="COMMAND_ERROR:"+error.getMessage();}catch(Exception error){result="ERROR";}
            var snapshot=new JSONObject();snapshot.put("result",result);snapshot.put("wire",MiniData.getMiniDataVersion(db).to0xString());var flags=new JSONArray();for(var row:db.listTxns())flags.add(row.getTransaction().isCheckedMonotonic());snapshot.put("checked",flags);results.add(snapshot);
        }
        return results;
    }
    public static void main(String[] args)throws Exception{MinimaDB.createDB();Locale.setDefault(Locale.ENGLISH);var input=new BufferedReader(new InputStreamReader(System.in));var output=System.out;String line;while((line=input.readLine())!=null)try{System.setOut(System.err);Object result=run((JSONObject)new JSONParser().parse(line));output.println(JSONValue.toJSONString(result));}catch(Exception error){output.println("\"ERROR\"");}finally{System.setOut(output);}}
}
