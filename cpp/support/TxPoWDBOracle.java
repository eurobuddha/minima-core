import java.io.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import org.minima.database.*;
import org.minima.database.wallet.Wallet;
import org.minima.database.txpowdb.*;
import org.minima.database.txpowdb.ram.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.utils.messages.Message;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

// Only JDBC and the Main/Wallet owner boundaries are fixtures. TxPoWDB, RAM,
// both SQL subclasses, TxPoWSearcher and Wallet membership methods are pinned.
public class TxPoWDBOracle extends TxPoWSqlDBOracle {
    static ArrayList<TxPoW> fixtures;
    static ArrayList<Message> messages;
    static IdentityHashMap<RamData,Integer> ramObjects;
    static int index(TxPoW txpow){return txpow==null?-2:fixtures.indexOf(txpow);}
    static void service(String name,Object... args){
        try{event(name,args);}catch(java.sql.SQLException error){throw new IllegalStateException(error.getMessage());}
        if(name.equals(config.get("serviceFail")))throw new IllegalStateException("service failure");
    }
    static class MainReceiver extends Main {
        public void PostMessage(Message message){var txpow=(TxPoW)message.getObject("txpow");service("main",message.getMessageType(),index(txpow),txpow.getTxPoWID());messages.add(message);}
        public void PostNotifyEvent(String name,JSONObject data){}
    }
    static class TrackedWallet extends Wallet {
        public boolean isAddressRelevant(String id){service("address",id);return super.isAddressRelevant(id);}
        public boolean isKeyRelevant(String id){service("key",id);return super.isKeyRelevant(id);}
    }
    static class Owner extends MinimaDB {
        TrackedWallet wallet;
        public Wallet getWallet(){service("wallet");return flag("nullWallet")?null:wallet;}
    }
    static JSONObject pow(TxPoW txpow){if(txpow==null)return null;var result=new JSONObject();result.put("index",index(txpow));result.put("id",txpow.getTxPoWID());return result;}
    static JSONArray pows(ArrayList<TxPoW> txpows){var result=new JSONArray();for(var txpow:txpows)result.add(pow(txpow));return result;}
    static JSONArray pool(ConcurrentHashMap<String,RamData> map){
        if(map==null)return null;var result=new JSONArray();
        for(var keys=map.keys();keys.hasMoreElements();){String key=keys.nextElement();RamData data=map.get(key);if(!ramObjects.containsKey(data))ramObjects.put(data,ramObjects.size());
            var row=new JSONObject();row.put("key",key);row.put("object",ramObjects.get(data));row.put("txpow",pow(data.getTxPoW()));row.put("access",data.getLastAccess());row.put("chain",data.isOnMainChain());row.put("cascade",data.isInCascade());result.add(row);
        }return result;
    }
    static TxPoW selected(){return config.get("index")==null?null:fixtures.get(integer(config,"index",0));}
    static String selectedID(){if(config.containsKey("id"))return (String)config.get("id");var txpow=selected();return txpow==null?null:txpow.getTxPoWID();}
    @SuppressWarnings("unchecked") static JSONObject runDB(JSONObject input,String root)throws Exception {
        fixtures=new ArrayList<>();messages=new ArrayList<>();ramObjects=new IdentityHashMap<>();connectionID=preparedID=0;
        for(Object spec:(JSONArray)input.get("fixtures")){config=(JSONObject)spec;fixtures.add(fixture());}
        var db=new TxPoWDB();ConcurrentHashMap<String,RamData> held=null;var outputs=new JSONArray();var encoded=new JSONArray();for(var txpow:fixtures)encoded.add(render(txpow));
        var main=NIOClientOracle.U.allocateInstance(MainReceiver.class);var owner=(Owner)NIOClientOracle.U.allocateInstance(Owner.class);owner.wallet=new TrackedWallet();
        for(Object entry:(JSONArray)input.get("ops")){
            config=(JSONObject)entry;events=new JSONArray();calls=0;Main.STARTUP_DEBUG_LOGS=flag("debug");
            NIOClientOracle.field(Main.class,"mMainInstance").set(null,flag("nullMain")?null:main);
            NIOClientOracle.field(MinimaDB.class,"mMinimaDB").set(null,flag("nullDB")?null:owner);
            for(var pair:new String[][]{{"mAllTrackedAddress","addresses"},{"mAllKeys","keys"}}){var set=(HashSet<String>)NIOClientOracle.get(owner.wallet,pair[0]);set.clear();if(config.get(pair[1])!=null)for(Object value:(JSONArray)config.get(pair[1]))set.add((String)value);}
            var output=new JSONObject();var log=new ByteArrayOutputStream();PrintStream previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));long before=System.currentTimeMillis();
            try{
                Object result=null;String cmd=(String)config.get("cmd");
                switch(cmd){
                    case "load":db.loadSQLDB(config.containsKey("file")?(config.get("file")==null?null:new File((String)config.get("file"))):new File(root+"/db/txpow"));break;
                    case "save":db.saveDB(flag("compact"));break;
                    case "hard":db.hardCloseSQLDB();break;
                    case "add":result=db.addTxPoW(selected());break;
                    case "addSQL":db.addSQLTxPoW(selected());break;
                    case "get":result=pow(db.getTxPoW(selectedID()));break;
                    case "exists":result=db.exists(selectedID());break;
                    case "all":result=pows(db.getAllTxPoW(config.get("ids")==null?null:new ArrayList<String>((JSONArray)config.get("ids"))));break;
                    case "children":result=pows(db.getChildBlocks(selectedID()));break;
                    case "ramSize":result=db.getRamSize();break;
                    case "sqlSize":result=db.getSqlSize();break;
                    case "file":result=db.getSqlFile()==null?null:db.getSqlFile().toString();break;
                    case "cleanRAM":((RamDB)NIOClientOracle.get(db,"mRamDB")).MAX_TIME=config.get("retention")==null?3600000:((Number)config.get("retention")).longValue();db.cleanDBRAM();break;
                    case "wipeRAM":db.wipeDBRAM();break;
                    case "cleanSQL":db.cleanDBSQL();break;
                    case "clearChain":db.clearMainChainTxns();break;
                    case "chain":db.setOnMainChain(selectedID());break;
                    case "cascade":db.setInCascade(selectedID());break;
                    case "unused":result=pows(db.getAllUnusedTxns());break;
                    case "remove":db.removeMemPoolTxPoW(selectedID());break;
                    case "coin":result=db.checkMempoolCoins(new MiniData((String)config.get("coinid")));break;
                    case "hold":held=db.getCompleteMemPool();break;
                    case "mutate":selected().setNonce(new MiniNumber(((Number)config.get("nonce")).longValue()));selected().calculateTXPOWID();break;
                    default:throw new IllegalArgumentException(cmd);
                }output.put("value",result);
            }catch(Exception error){output.put("exception",error.toString());}finally{System.setOut(previous);}
            var current=db.getCompleteMemPool();output.put("pool",pool(current));output.put("held",pool(held));output.put("samePool",current==held);
            var posted=new JSONArray();for(var message:messages){var row=new JSONObject();row.put("type",message.getMessageType());row.put("txpow",pow((TxPoW)message.getObject("txpow")));posted.add(row);}output.put("posted",posted);
            output.put("before",before);output.put("after",System.currentTimeMillis());
            var logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int at=line.indexOf("] : ");if(at>=0){String body=line.substring(at+4);if(!body.startsWith("     "))logs.add(body);}}
            output.put("events",events);output.put("logs",logs);output.put("sqlFile",db.getSqlFile()==null?null:db.getSqlFile().toString());output.put("chainFile",db.getOnChainDB().getSQLFile()==null?null:db.getOnChainDB().getSQLFile().toString());outputs.add(output);
        }
        var result=new JSONObject();result.put("fixtures",encoded);result.put("ops",outputs);return result;
    }
    public static void main(String[] args)throws Exception{installDriver();var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{System.out.println(runDB((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Exception error){error.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
