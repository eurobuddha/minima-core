import java.io.*;
import java.util.*;
import org.minima.database.wallet.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class WalletOracle extends SqlDBOracle {
    static class DB extends Wallet{void create()throws Exception{createSQL();}void clear(){mSQLConnection=null;}}
    static Object render(Object value){if(value==null)return null;if(value instanceof SeedRow r)return r.toJSON();if(value instanceof KeyRow r)return r.toJSON();if(value instanceof ScriptRow r)return r.toJSON();JSONArray list=new JSONArray();for(Object r:(ArrayList<?>)value)list.add(render(r));return list;}
    static JSONObject caches(DB db)throws Exception{JSONObject out=new JSONObject();String[] fields={"mAllKeys","mAllTrackedAddress","mAllSimpleAddress"};String[] names={"keys","tracked","simple"};for(int i=0;i<3;i++){var f=Wallet.class.getDeclaredField(fields[i]);f.setAccessible(true);var list=new ArrayList<String>((HashSet<String>)f.get(db));list.sort(Comparator.nullsFirst(Comparator.naturalOrder()));out.put(names[i],new JSONArray(list));}return out;}
    static int id(ArrayList<SeedRow> seeds,SeedRow row){if(row==null)return -1;int i=seeds.indexOf(row);if(i<0){i=seeds.size();seeds.add(row);}return i;}
    static JSONObject runWallet(JSONObject input,String root)throws Exception{
        connectionID=preparedID=0;GeneralParams.TEST_PARAMS=Boolean.TRUE.equals(input.get("test"));Wallet.NUMBER_GETADDRESS_KEYS=64;DB db=new DB();JSONArray outputs=new JSONArray();SeedRow held=null;var seeds=new ArrayList<SeedRow>();
        for(Object entry:(JSONArray)input.get("ops")){
            config=(JSONObject)entry;events=new JSONArray();calls=0;Main.STARTUP_DEBUG_LOGS=flag("debug");GeneralParams.SEED_PHRASE=config.containsKey("phraseParam")?(String)config.get("phraseParam"):"fixture seed";GeneralParams.ANYSEED_PHRASE=!config.containsKey("any")||flag("any");if(config.containsKey("target"))Wallet.NUMBER_GETADDRESS_KEYS=integer(config,"target",0);
            JSONObject out=new JSONObject();var log=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
            try{Object result=null;String cmd=(String)config.get("cmd");
                switch(cmd){
                    case "load":db.loadDB(new File(root+"/wallet"));break;case "construct":GeneralParams.TEST_PARAMS=flag("test");new DB();break;case "create":db.create();break;case "clearConnection":db.clear();break;case "hard":db.hardCloseDB();break;case "save":db.saveDB(flag("compact"));break;case "open":result=db.isOpen();break;
                    case "base":result=render(db.getBaseSeed());break;case "available":result=db.isBaseSeedAvailable();break;case "hold":held=db.getBaseSeed();break;case "mutate":{var row=flag("held")?held:db.getBaseSeed();row.mPhrase=(String)config.get("phrase");row.mSeed=(String)config.get("seed");break;}
                    case "updateSeed":db.updateSeedRow((String)config.get("phrase"),(String)config.get("seed"));break;case "wipeSeed":db.wipeBaseSeedRow();break;case "reset":db.resetDB((String)config.get("phrase"));break;case "resetPrivate":result=db.resetBaseSeedPrivKeys((String)config.get("phrase"),(String)config.get("seed"));break;case "checkPrivate":result=db.checkAllPrivateKeys();break;case "checkSingle":result=db.checkSingleKey((String)config.get("privatekey"),(String)config.get("modifier"));break;
                    case "stop":db.setStopNewKeys(flag("stop"));break;case "shutdown":db.shuttingDown();break;case "defaults":result=db.getDefaultKeysNumber();break;case "init":result=config.containsKey("log")?db.initDefaultKeys(integer(config,"max",0),flag("log")):db.initDefaultKeys(integer(config,"max",0));break;case "default":result=render(db.getDefaultAddress());break;case "newKey":result=render(db.createNewKey());break;case "newSimple":result=render(db.createNewSimpleAddress(flag("default")));break;
                    case "addScript":result=render(db.addScript((String)config.get("script"),flag("simple"),flag("default"),(String)config.get("publickey"),flag("track")));break;case "remove":db.removeScript((String)config.get("address"));break;case "script":result=render(db.getScriptFromAddress((String)config.get("address")));break;case "keyRelevant":result=db.isKeyRelevant((String)config.get("publickey"));break;case "addressRelevant":result=db.isAddressRelevant((String)config.get("address"));break;case "addressSimple":result=db.isAddressSimple((String)config.get("address"));break;
                    case "keys":result=render(db.getAllKeys());break;case "key":result=render(db.getKeyFromPublic((String)config.get("publickey")));break;case "addresses":result=render(db.getAllAddresses());break;case "sign":{var sig=db.signData((String)config.get("publickey"),config.get("data")==null?null:new MiniData((String)config.get("data")));if(sig!=null)result=MiniData.getMiniDataVersion(sig).to0xString();break;}case "uses":db.updateAllKeyUses(integer(config,"uses",0));break;case "increment":db.updateIncrementAllKeyUses(integer(config,"uses",0));break;default:throw new IllegalArgumentException(cmd);
                }out.put("value",result);
            }catch(Exception error){out.put("exception",error.toString());}finally{System.setOut(previous);}
            JSONArray logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int i=line.indexOf("] : ");if(i>=0){String body=line.substring(i+4);if(!body.startsWith("     "))logs.add(body);}}
            out.put("base",render(db.getBaseSeed()));out.put("held",render(held));out.put("baseID",id(seeds,db.getBaseSeed()));out.put("heldID",id(seeds,held));out.put("caches",caches(db));out.put("target",Wallet.NUMBER_GETADDRESS_KEYS);out.put("phraseParam",GeneralParams.SEED_PHRASE);out.put("events",events);out.put("logs",logs);outputs.add(out);
        }JSONObject out=new JSONObject();out.put("ops",outputs);return out;
    }
    public static void main(String[] args)throws Exception{installDriver();var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(runWallet((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Exception error){System.out.println("{\"fatal\":true}");error.printStackTrace(System.err);}}
}
