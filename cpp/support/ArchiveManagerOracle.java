import java.io.*;
import java.util.*;
import org.minima.database.archive.ArchiveManager;
import org.minima.database.cascade.Cascade;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.utils.Streamable;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

// Actual pinned ArchiveManager, TxBlock, Cascade and SqlDB bodies. Only JDBC,
// Main construction/notifications and test inputs are fixtures.
public class ArchiveManagerOracle extends SqlDBOracle {
    static int shutdownCalls;
    static class DB extends ArchiveManager {
        void create()throws Exception{createSQL();}
        void clear(){mSQLConnection=null;}
    }
    static class Owner extends Main {
        Owner()throws Exception{super();}
        public boolean isShuttingDown(){
            ++shutdownCalls;
            if(shutdownCalls>=integer(config,"shutdownAfter",Integer.MAX_VALUE))try{NIOClientOracle.set(this,"mShuttingdown",true);}catch(Exception error){throw new RuntimeException(error);}
            return super.isShuttingDown();
        }
        public void PostNotifyEvent(String event,JSONObject data){}
    }
    static long longValue(JSONObject object,String key,long fallback){return object.get(key)==null?fallback:((Number)object.get(key)).longValue();}
    // Exact deterministic TxPoW constructor from ArchiveCacheOracle.txpow.
    // That older test class depends on a TxPoW cache-test replacement; this
    // fixture uses the actual upstream TxPoW class instead.
    static TxPoW txpow(int i,int mode){
        var t=new TxPoW();t.setTimeMilli(new MiniNumber(i*1000));t.setNonce(new MiniNumber(i));t.setBlockNumber(new MiniNumber(i));t.getTxBody().mPRNG=new MiniData("0x12");
        for(int n=0;n<32;n++)t.setSuperParent(n,new MiniData("0x00"));t.getTxHeader().mTimeMilli=new MiniNumber(i*1000);t.setHeaderBodyHash();if((mode&1)!=0&&i%2==0)t.clearBody();t.calculateTXPOWID();return t;
    }
    static String wire(Streamable value)throws Exception{if(value==null)return null;var out=new ByteArrayOutputStream();value.writeDataStream(new DataOutputStream(out));return new MiniData(out.toByteArray()).to0xString();}
    static JSONArray wires(ArrayList<TxBlock> blocks)throws Exception{var out=new JSONArray();for(var block:blocks)out.add(wire(block));return out;}
    static Cascade cascade(JSONObject spec){
        var cascade=new Cascade();for(int i=0;i<integer(spec,"count",3);i++)cascade.addToTip(txpow(i,0));var tip=cascade.getTip();int mode=integer(spec,"mode",0);
        if(tip!=null){
            if((mode&1)!=0)tip.setLevel(3);
            if((mode&2)!=0)tip.getTxPoW().setSuperParent(1,new MiniData("0x1234"));
            if((mode&4)!=0)tip.getTxPoW().setSuperParent(0,new MiniData("0x1234"));
            if((mode&8)!=0&&tip.getParent()!=null)tip.getParent().setLevel(2);
        }return cascade;
    }
    static JSONObject runArchive(JSONObject input,String root)throws Exception{
        connectionID=preparedID=0;GeneralParams.NUMBER_DAYS_ARCHIVE=longValue(input,"days",50);GeneralParams.ARCHIVE=false;var db=new DB();long defaultRetention=db.MAX_KEEP_BLOCKS;
        var blocks=new ArrayList<TxBlock>();var cascades=new ArrayList<Cascade>();var encoded=new JSONArray();var cascEncoded=new JSONArray();var outputs=new JSONArray();
        for(Object item:(JSONArray)input.get("fixtures")){var spec=(JSONObject)item;var block=new TxBlock(txpow(integer(spec,"index",0),integer(spec,"mode",0)));if(spec.containsKey("nonce")){block.getTxPoW().setNonce(new MiniNumber(longValue(spec,"nonce",0)));block.getTxPoW().calculateTXPOWID();}blocks.add(block);encoded.add(wire(block));}
        for(Object item:(JSONArray)input.get("cascades")){var c=cascade((JSONObject)item);cascades.add(c);cascEncoded.add(wire(c));}
        var owner=(Owner)NIOClientOracle.U.allocateInstance(Owner.class);
        for(Object entry:(JSONArray)input.get("ops")){
            config=(JSONObject)entry;events=new JSONArray();calls=shutdownCalls=0;Main.STARTUP_DEBUG_LOGS=flag("debug");
            NIOClientOracle.field(Main.class,"mMainInstance").set(null,flag("nullMain")?null:owner);NIOClientOracle.set(owner,"mShuttingdown",flag("shutdown"));
            if(config.containsKey("retention"))db.MAX_KEEP_BLOCKS=longValue(config,"retention",0);
            if(config.containsKey("archive"))GeneralParams.ARCHIVE=flag("archive");
            var output=new JSONObject();var log=new ByteArrayOutputStream();PrintStream previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));long before=System.currentTimeMillis();
            try{
                Object result=null;String cmd=(String)config.get("cmd");MiniNumber start=config.get("start")==null?null:new MiniNumber((String)config.get("start")),end=config.get("end")==null?null:new MiniNumber((String)config.get("end"));
                switch(cmd){
                    case "load":db.loadDB(new File(root+"/db/archive"));break;
                    case "create":db.create();break;
                    case "clear":db.clear();break;
                    case "hard":db.hardCloseDB();break;
                    case "hack":db.hackShut();break;
                    case "save":db.saveDB(flag("compact"));break;
                    case "open":result=db.isOpen();break;
                    case "size":result=db.getSize();break;
                    case "add":{var block=config.get("index")==null?null:blocks.get(integer(config,"index",0));if(block!=null&&flag("badHash"))block.getTxPoW().setSuperParent(0,new MiniData(new byte[33]));result=db.saveBlock(block);break;}
                    case "get":result=wire(db.loadBlock((String)config.get("id")));break;
                    case "exists":result=db.exists((String)config.get("id")).toString();break;
                    case "number":result=wire(db.loadBlockFromNumber(start));break;
                    case "first":result=wire(db.loadFirstBlock());break;
                    case "last":result=wire(db.loadLastBlock());break;
                    case "sync":result=wires(db.loadSyncBlockRange(start));break;
                    case "range":result=wires(config.containsKey("descending")?db.loadBlockRange(start,end,flag("descending")):db.loadBlockRange(start,end));break;
                    case "cascade":result=wire(db.loadCascade());break;
                    case "checkCascade":{var cascade=config.get("index")==null?null:cascades.get(integer(config,"index",0));if(cascade!=null&&cascade.getTip()!=null&&flag("badHash"))cascade.getTip().getTxPoW().setSuperParent(0,new MiniData(new byte[33]));db.checkCascadeRequired(cascade);break;}
                    case "clean":result=db.checkForCleanDB();break;
                    case "reopen":db.closeAndReopen();break;
                    default:throw new IllegalArgumentException(cmd);
                }output.put("value",result);
            }catch(Exception error){output.put("exception",error.toString());}
            finally{System.setOut(previous);}
            output.put("before",before);output.put("after",System.currentTimeMillis());output.put("shutdownCalls",shutdownCalls);
            var logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int index=line.indexOf("] : ");if(index>=0){String body=line.substring(index+4);if(!body.startsWith("     "))logs.add(body);}}
            output.put("logs",logs);output.put("events",events);output.put("file",db.getSQLFile()==null?null:db.getSQLFile().toString());output.put("retention",db.MAX_KEEP_BLOCKS);outputs.add(output);
        }
        var result=new JSONObject();result.put("ops",outputs);result.put("fixtures",encoded);result.put("cascades",cascEncoded);result.put("defaultRetention",defaultRetention);return result;
    }
    public static void main(String[] args)throws Exception{
        installDriver();var in=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=in.readLine())!=null)try{System.out.println(runArchive((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Exception error){System.out.println("{\"fatal\":true}");error.printStackTrace(System.err);}
    }
}
