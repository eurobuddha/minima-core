import java.io.*;
import java.util.*;
import org.minima.objects.TxPoW;
import org.minima.objects.base.MiniData;
import org.minima.database.txpowdb.sql.TxPoWSqlDB;
import org.minima.objects.base.MiniNumber;
import org.minima.system.Main;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

public class TxPoWSqlDBOracle extends SqlDBOracle {
    static final long DEFAULT_RETENTION=TxPoWSqlDB.MAX_SQL_MILLI;
    static class DB extends TxPoWSqlDB {
        void create() throws Exception {createSQL();}
        void clear(){mSQLConnection=null;}
    }
    static TxPoW fixture() throws Exception {
        var txpow=flag("transaction")?RamOracle.transaction(integer(config,"index",0)):ArchiveCacheOracle.txpow(integer(config,"index",0),integer(config,"mode",0));
        if(config.get("state")!=null){txpow.getTransaction().addStateVariable(new org.minima.objects.StateVariable(2,(String)config.get("state")));txpow.setHeaderBodyHash();txpow.calculateTXPOWID();}
        if(flag("invalidHash"))txpow.setSuperParent(0,new MiniData(new byte[65]));return txpow;
    }
    static JSONObject render(TxPoW txpow){
        if(txpow==null)return null;JSONObject result=new JSONObject();result.put("wire",MiniData.getMiniDataVersion(txpow).to0xString());result.put("id",txpow.getTxPoWID());result.put("isblock",txpow.isBlock());result.put("istransaction",txpow.isTransaction());return result;
    }
    static JSONArray render(ArrayList<TxPoW> txpows){JSONArray result=new JSONArray();for(TxPoW txpow:txpows)result.add(render(txpow));return result;}
    static JSONObject runChain(JSONObject input,String root)throws Exception {
        connectionID=preparedID=0;
        TxPoWSqlDB.MAX_SQL_MILLI=DEFAULT_RETENTION;
        TxPoWSqlDB.LOG_ADD_BLOCKS=false;TxPoWSqlDB.LOG_ADD_TRANSACTION=false;
        DB db=new DB();JSONArray outputs=new JSONArray();
        for(Object entry:(JSONArray)input.get("ops")) {
            config=(JSONObject)entry;events=new JSONArray();calls=0;Main.STARTUP_DEBUG_LOGS=flag("debug");
            if(config.containsKey("retention"))TxPoWSqlDB.MAX_SQL_MILLI=((Number)config.get("retention")).longValue();
            JSONObject output=new JSONObject();ByteArrayOutputStream log=new ByteArrayOutputStream();PrintStream previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
            TxPoWSqlDB.LOG_ADD_BLOCKS=flag("logBlocks");TxPoWSqlDB.LOG_ADD_TRANSACTION=flag("logTransactions");
            long before=System.currentTimeMillis();
            try {
                Object result=null;String command=(String)config.get("cmd");
                switch(command){
                    case "load":db.loadDB(new File(root+"/db/txpow"));break;
                    case "create":db.create();break;
                    case "wipe":db.wipeDB();break;
                    case "clear":db.clear();break;
                    case "hard":db.hardCloseDB();break;
                    case "save":db.saveDB(flag("compact"));break;
                    case "open":result=db.isOpen();break;
                    case "make":result=MiniData.getMiniDataVersion(fixture()).to0xString();break;
                    case "add":result=db.addTxPoW(flag("nullTxPoW")?null:fixture(),flag("relevant"));break;
                    case "get":result=render(db.getTxPoW((String)config.get("txpowid")));break;
                    case "children":result=new JSONArray(db.getChildBlocks((String)config.get("parentid")));break;
                    case "relevant":result=render(config.containsKey("offset")?db.getAllRelevant(integer(config,"limit",0),integer(config,"offset",0)):db.getAllRelevant(integer(config,"limit",0)));break;
                    case "latest":result=render(db.getLatestTxPoW(integer(config,"limit",0),integer(config,"offset",0)));break;
                    case "latestSize":result=db.getLatestTxPoWSize(((Number)config.get("minTime")).longValue());break;
                    case "relevantSize":result=db.getRelevantSize();break;
                    case "size":result=db.getSize();break;
                    case "custom":result=db.customSizeQuery((String)config.get("where"));break;
                    case "exists":result=db.exists((String)config.get("txpowid"));break;
                    case "reopen":db.closeAndReopen();break;
                    case "clean":result=config.containsKey("hard")?db.cleanDB(flag("hard")):db.cleanDB();break;
                    default:throw new IllegalArgumentException(command);
                }
                output.put("value",result);
            }catch(Exception error){output.put("exception",error.toString());}
            finally{System.setOut(previous);}
            output.put("before",before);output.put("after",System.currentTimeMillis());
            JSONArray logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int index=line.indexOf("] : ");if(index>=0){String body=line.substring(index+4);if(!body.startsWith("     "))logs.add(body);}}
            output.put("logs",logs);output.put("events",events);output.put("file",db.getSQLFile()==null?null:db.getSQLFile().toString());output.put("retention",TxPoWSqlDB.MAX_SQL_MILLI);outputs.add(output);
        }
        JSONObject result=new JSONObject();result.put("ops",outputs);result.put("defaultRetention",DEFAULT_RETENTION);return result;
    }
    public static void main(String[] args)throws Exception {
        installDriver();var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null)try{System.out.println(runChain((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Exception error){System.out.println("{\"fatal\":true}");error.printStackTrace(System.err);}
    }
}
