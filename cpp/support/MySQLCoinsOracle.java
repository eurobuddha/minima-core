import java.io.*;
import org.minima.database.MinimaDB;
import org.minima.objects.*;
import org.minima.objects.mmr.MMRProof;
import org.minima.system.commands.backup.mysqlcoins;
import org.minima.system.commands.CommandRunner;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
public class MySQLCoinsOracle extends SqlDBOracle {
    static JSONObject runCoins(JSONObject q)throws Exception{
        config=(JSONObject)q.get("sql");events=new JSONArray();calls=connectionID=preparedID=0;
        GeneralParams.IS_MOBILE=Boolean.TRUE.equals(q.get("mobile"));MinimaDB.createDB();
        var prefs=MinimaDB.getDB().getUserDB();prefs.getAllData().putAll((JSONObject)q.get("prefs"));
        var out=new JSONObject();var blocks=new JSONArray();
        for(int i=0;i<2;i++){
            var block=new TxBlock(ArchiveManagerOracle.txpow(i+10,0));
            block.getInputCoinProofs().add(new CoinProof(ObjectOracle.coin(i),new MMRProof()));
            block.getOutputCoins().add(ObjectOracle.coin(i+1));
            block.getOutputCoins().add(ObjectOracle.coin(i+2));
            blocks.add(ArchiveManagerOracle.wire(block));
        }
        out.put("blocks",blocks);var command=new mysqlcoins();command.getParams().putAll((JSONObject)q.get("params"));
        var buffer=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(buffer,true,"UTF-8"));out.put("before",System.currentTimeMillis());
        try{
            String op=q.getString("op");Object value;
            if(op.equals("meta")){var meta=new JSONObject();meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("valid",command.getValidParams());meta.put("fresh",command.getFunction().getJSONReply());value=meta;}
            else if(op.equals("runner"))value=CommandRunner.getRunner().runMultiCommand(q.getString("caller"),q.getString("text"));
            else if(op.equals("bootstrap"))value=null;
            else value=command.runCommand();
            out.put("value",value);
        }catch(Exception error){out.put("exception",error.toString());}finally{System.setOut(previous);}
        out.put("after",System.currentTimeMillis());var logs=new JSONArray();for(String line:buffer.toString("UTF-8").split("\n")){int at=line.indexOf("] : ");if(at>=0&&!line.substring(at+4).startsWith("     "))logs.add(line.substring(at+4));}
        out.put("events",events);out.put("logs",logs);out.put("prefs",new JSONObject(prefs.getAllData()));return out;
    }
    public static void main(String[] args)throws Exception{installDriver("jdbc:mysql:");var reader=new BufferedReader(new InputStreamReader(System.in));String line;while((line=reader.readLine())!=null)System.out.println(runCoins((JSONObject)JSONValue.parseWithException(line)).toJSONString());}
}
