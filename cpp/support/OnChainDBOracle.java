import java.io.*;
import org.minima.database.txpowdb.onchain.TxPoWOnChainDB;
import org.minima.objects.base.MiniNumber;
import org.minima.system.Main;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

public class OnChainDBOracle extends SqlDBOracle {
    static final long DEFAULT_RETENTION=TxPoWOnChainDB.MAX_ONCHAINSQL_MILLI;
    static class DB extends TxPoWOnChainDB {
        void create() throws Exception {createSQL();}
        void clear(){mSQLConnection=null;}
    }
    static JSONObject runChain(JSONObject input,String root)throws Exception {
        connectionID=preparedID=0;
        TxPoWOnChainDB.MAX_ONCHAINSQL_MILLI=DEFAULT_RETENTION;
        DB db=new DB();JSONArray outputs=new JSONArray();
        for(Object entry:(JSONArray)input.get("ops")) {
            config=(JSONObject)entry;events=new JSONArray();calls=0;Main.STARTUP_DEBUG_LOGS=flag("debug");
            if(config.containsKey("retention"))TxPoWOnChainDB.MAX_ONCHAINSQL_MILLI=((Number)config.get("retention")).longValue();
            JSONObject output=new JSONObject();ByteArrayOutputStream log=new ByteArrayOutputStream();PrintStream previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
            long before=System.currentTimeMillis();
            try {
                Object result=null;String command=(String)config.get("cmd");
                switch(command){
                    case "load":db.loadDB(new File(root+"/db/onchain"));break;
                    case "create":db.create();break;
                    case "wipe":db.wipeDB();break;
                    case "clear":db.clear();break;
                    case "hard":db.hardCloseDB();break;
                    case "save":db.saveDB(flag("compact"));break;
                    case "open":result=db.isOpen();break;
                    case "add":result=db.addOnChainTxPoW((String)config.get("blockid"),config.get("block")==null?null:new MiniNumber((String)config.get("block")),(String)config.get("txpowid"));break;
                    case "get":result=db.getOnChainTxPoW((String)config.get("txpowid"));break;
                    case "inblock":result=db.getInBlockTxPoW((String)config.get("blockid"));break;
                    case "inheight":result=db.getInBlockTxPoW(((Number)config.get("height")).longValue());break;
                    case "first":result=db.getFirstTxPoW();break;
                    case "last":result=db.getLastTxPoW();break;
                    case "size":result=db.getSize();break;
                    case "clean":result=config.containsKey("hard")?db.cleanDB(flag("hard")):db.cleanDB();break;
                    default:throw new IllegalArgumentException(command);
                }
                output.put("value",result);
            }catch(Exception error){output.put("exception",error.toString());}
            finally{System.setOut(previous);}
            output.put("before",before);output.put("after",System.currentTimeMillis());
            JSONArray logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int index=line.indexOf("] : ");if(index>=0){String body=line.substring(index+4);if(!body.startsWith("     "))logs.add(body);}}
            output.put("logs",logs);output.put("events",events);output.put("file",db.getSQLFile()==null?null:db.getSQLFile().toString());output.put("retention",TxPoWOnChainDB.MAX_ONCHAINSQL_MILLI);outputs.add(output);
        }
        JSONObject result=new JSONObject();result.put("ops",outputs);result.put("defaultRetention",DEFAULT_RETENTION);return result;
    }
    public static void main(String[] args)throws Exception {
        installDriver();var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null)try{System.out.println(runChain((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Exception error){System.out.println("{\"fatal\":true}");error.printStackTrace(System.err);}
    }
}
