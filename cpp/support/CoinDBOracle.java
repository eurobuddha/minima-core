import java.io.*;
import java.util.*;
import org.minima.database.txpowtree.CoinDB;
import org.minima.objects.Coin;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

public class CoinDBOracle extends SqlDBOracle {
    static Coin fixtureCoin(){var c=ObjectOracle.coin(integer(config,"index",0));if(flag("invalidHash"))c.resetCoinID(new MiniData(new byte[65]));return c;}
    static JSONObject renderCoin(Coin coin)throws Exception {
        if(coin==null)return null;JSONObject result=new JSONObject();result.put("wire",new MiniData(org.minima.utils.BaseConverter.decode16(ObjectOracle.wire(coin))).to0xString());result.put("json",coin.toJSON());return result;
    }
    static int id(ArrayList<CoinDB> identities,CoinDB db){if(db==null)return -1;int i=identities.indexOf(db);if(i<0){i=identities.size();identities.add(db);}return i;}
    static JSONObject runCoins(JSONObject input,String root)throws Exception {
        connectionID=preparedID=0;JSONArray outputs=new JSONArray();var identities=new ArrayList<CoinDB>();CoinDB held=null;
        for(Object entry:(JSONArray)input.get("ops")){
            config=(JSONObject)entry;events=new JSONArray();calls=0;Main.STARTUP_DEBUG_LOGS=flag("debug");JSONObject output=new JSONObject();
            File f=config.containsKey("file")?(config.get("file")==null?null:new File(root+"/files/"+(String)config.get("file"))):new File(root+"/files/db/coin");
            File old=f==null?null:new File(f.getAbsolutePath()+".mv.db");id(identities,CoinDB.getTxPoWTreeCoinDB());
            var log=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
            try {
                if(f!=null&&config.get("seed")!=null){String mode=(String)config.get("seed");old.getParentFile().mkdirs();if(mode.equals("file")){try(var stream=new FileOutputStream(old)){stream.write("fixture".getBytes());}}else{old.mkdirs();if(mode.equals("nonempty")){try(var stream=new FileOutputStream(new File(old,"child"))){stream.write("fixture".getBytes());}}}}
                Object result=null;String cmd=(String)config.get("cmd");CoinDB db=flag("held")?held:CoinDB.getTxPoWTreeCoinDB();
                switch(cmd){
                    case "create":CoinDB.createCoinDB(f);break;
                    case "hold":held=db;break;
                    case "instance":result=id(identities,db);break;
                    case "make":result=renderCoin(fixtureCoin());break;
                    case "load":db.loadDB(f);break;
                    case "encrypted":db.loadEncryptedSQLDB(f,(String)config.get("password"));break;
                    case "hard":db.hardCloseDB();break;
                    case "save":db.saveDB(flag("compact"));break;
                    case "open":result=db.isOpen();break;
                    case "reopen":db.closeAndReopen();break;
                    case "size":result=db.getSize();break;
                    case "clear":db.clearOldCoins(((Number)config.get("height")).longValue());break;
                    case "get":{var coins=new JSONArray();for(var coin:db.getAllCoins(config.get("tree")==null?null:new MiniData((String)config.get("tree"))))coins.add(renderCoin(coin));result=coins;break;}
                    case "insert":result=db.insertCoin(config.get("tree")==null?null:new MiniData((String)config.get("tree")),flag("nullCoin")?null:fixtureCoin(),config.get("block")==null?null:new MiniNumber((String)config.get("block")));break;
                    default:throw new IllegalArgumentException(cmd);
                }
                output.put("value",result);
            }catch(Exception error){output.put("exception",error.toString());}
            finally{System.setOut(previous);}
            var current=CoinDB.getTxPoWTreeCoinDB();output.put("instance",id(identities,current));output.put("held",id(identities,held));output.put("file",current==null||current.getSQLFile()==null?null:current.getSQLFile().toString());output.put("heldFile",held==null||held.getSQLFile()==null?null:held.getSQLFile().toString());output.put("oldExists",old!=null&&old.exists());
            JSONArray logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int index=line.indexOf("] : ");if(index>=0){String body=line.substring(index+4);if(!body.startsWith("     "))logs.add(body);}}
            output.put("logs",logs);output.put("events",events);outputs.add(output);
        }
        JSONObject result=new JSONObject();result.put("ops",outputs);return result;
    }
    public static void main(String[] args)throws Exception {
        installDriver();var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null)try{System.out.println(runCoins((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Exception error){System.out.println("{\"fatal\":true}");error.printStackTrace(System.err);}
    }
}
