import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.commands.*;
import org.minima.system.commands.base.sphincs;
import org.minima.system.params.GeneralParams;
import org.minima.utils.MiniFile;
import org.minima.utils.json.*;

/** Actual command and runner; controlled children keep transaction tests offline. */
public class SPHINCSCommandOracle {
    static JSONObject config;static JSONArray calls;static TxnDB db;
    static JSONArray rows(){var r=new JSONArray();for(var row:db.listTxns()){var item=new JSONObject();item.put("id",row.getID());item.put("wire",MiniData.getMiniDataVersion(row.getTransaction()).to0xString());item.put("transaction",row.getTransaction().toJSON());r.add(item);}return r;}
    static class Child extends Command {
        Command original;
        Child(Command cmd){super(cmd.getName(),cmd.getHelp());original=cmd;}
        public ArrayList<String> getValidParams(){return original.getValidParams();}
        public Command getFunction(){return new Child(original.getFunction());}
        public JSONObject runCommand()throws Exception{
            var call=new JSONObject();call.put("text",getCompleteCommand());call.put("rows",rows());calls.add(call);
            if(config.containsKey("fail")&&getName().equals(config.get("fail")))return (JSONObject)config.get("reply");
            String id=getParam("id");
            if(getName().equals("txncreate")||getName().equals("txndelete")){original.getParams().putAll(getParams());return original.runCommand();}
            if(getName().equals("txnaddamount")){
                var txn=db.getTransactionRow(id).getTransaction();int n=((Long)config.get("inputs")).intValue();
                for(int i=0;i<n;i++)txn.addInput(new Coin(new MiniData("0x"+String.format("%02X",i+2)),new MiniData("0x01"),MiniNumber.ONE,new MiniData("0x00"),true));
                txn.addOutput(new Coin(new MiniData("0x00"),new MiniData("0x02"),MiniNumber.ONE,new MiniData("0x00"),true));
            }
            var ret=getJSONReply();var response=new JSONObject();response.put("child",getName());ret.put("response",response);return ret;
        }
    }
    static JSONObject run(JSONObject input)throws Exception{
        config=input;calls=new JSONArray();MinimaDB.createDB();db=new TxnDB();var field=MinimaDB.class.getDeclaredField("mTxnDB");field.setAccessible(true);field.set(MinimaDB.getDB(),db);
        GeneralParams.BASE_FILE_FOLDER=input.getString("base");
        if(input.containsKey("write")){var file=MiniFile.createBaseFile(input.getString("path"));MiniFile.writeDataToFile(file,new MiniData(input.getString("write")).getBytes());}
        var saved=CommandRunner.ALL_COMMANDS.clone();var out=new JSONObject();
        try{
            if(input.containsKey("inputs"))for(int i=0;i<saved.length;i++)if(Set.of("txncreate","txnaddamount","txnscript","txnmmr","txnpost","txndelete").contains(saved[i].getName()))CommandRunner.ALL_COMMANDS[i]=new Child(saved[i]);
            var cmd=new sphincs();cmd.getParams().putAll((JSONObject)input.get("params"));
            if(Boolean.TRUE.equals(input.get("meta"))){var m=new JSONObject();m.put("help",cmd.getHelp());m.put("fullhelp",cmd.getFullHelp());m.put("params",cmd.getValidParams());m.put("fresh",cmd.getFunction().getJSONReply());out.put("value",m);}
            else if(input.containsKey("runner"))out.put("value",CommandRunner.getRunner().runMultiCommand(input.getString("runner")));
            else out.put("value",cmd.runCommand());
        }catch(Exception e){out.put("exception",e.toString());}
        finally{for(int i=0;i<saved.length;i++)CommandRunner.ALL_COMMANDS[i]=saved[i];}
        out.put("calls",calls);out.put("rows",rows());return out;
    }
    public static void main(String[] args)throws Exception{
        var reader=new BufferedReader(new InputStreamReader(System.in));var stdout=System.out;String line;
        while((line=reader.readLine())!=null){var capture=new ByteArrayOutputStream();JSONObject out;
            try{System.setOut(new PrintStream(capture,true,"UTF-8"));out=run((JSONObject)JSONValue.parseWithException(line));}finally{System.setOut(stdout);}
            var logs=new JSONArray();for(String text:capture.toString("UTF-8").split("\n")){if(text.isEmpty())continue;int pos=text.indexOf("] : ");text=pos<0?text:text.substring(pos+4);if(!text.startsWith("     "))logs.add(text);}out.put("logs",logs);stdout.println(out.toJSONString());
        }
    }
}
