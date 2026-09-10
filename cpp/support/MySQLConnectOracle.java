import java.io.*;
import java.math.*;
import java.util.*;
import org.minima.utils.mysql.MySQLConnect;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.MMREntryNumber;
import org.minima.database.cascade.Cascade;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class MySQLConnectOracle extends SqlDBOracle {
    static String setting(JSONObject input,String key,String fallback){return input.containsKey(key)?(String)input.get(key):fallback;}
    static long number(JSONObject input,String key){return input.get(key)==null?0:((Number)input.get(key)).longValue();}
    static JSONObject runMySQL(JSONObject input)throws Exception{
        connectionID=preparedID=0;Locale.setDefault(Locale.forLanguageTag(input.getString("locale").replace('_','-')));
        var db=new MySQLConnect(setting(input,"host","fixture:3306"),setting(input,"database","fixture"),setting(input,"user","fixture"),setting(input,"password","synthetic"),Boolean.TRUE.equals(input.get("readOnly")));
        var pows=new ArrayList<TxPoW>();var blocks=new ArrayList<TxBlock>();var cascades=new ArrayList<Cascade>();var pw=new JSONArray();var bw=new JSONArray();var cw=new JSONArray();
        for(int i=0;i<3;i++){var pow=ArchiveManagerOracle.txpow(i,i==2?1:0);pows.add(pow);blocks.add(new TxBlock(pow));pw.add(ArchiveManagerOracle.wire(pow));bw.add(ArchiveManagerOracle.wire(blocks.get(i)));}
        for(int count:new int[]{0,3}){var spec=new JSONObject();spec.put("count",count);var cascade=ArchiveManagerOracle.cascade(spec);cascades.add(cascade);cw.add(ArchiveManagerOracle.wire(cascade));}
        var outputs=new JSONArray();
        for(Object step:(JSONArray)input.get("ops")){
            config=(JSONObject)step;events=new JSONArray();calls=0;var out=new JSONObject();var buffer=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(buffer,true,"UTF-8"));long before=System.currentTimeMillis();
            try{
                Object value=null;String cmd=config.getString("cmd");int index=integer(config,"index",0);boolean nil=flag("null");
                switch(cmd){
                    case "init":db.init();break;case "shutdown":db.shutdown();break;case "wipe":db.wipeAll();break;case "wipeCoins":db.wipeCoinsDB();break;
                    case "debug":NIOClientOracle.set(db,"DEBUG",flag("enabled"));break;case "clear":NIOClientOracle.set(db,"mConnection",null);break;
                    case "count":value=db.getCount();break;case "first":value=db.loadFirstBlock();break;case "last":value=db.loadLastBlock();break;case "maxCoin":value=db.getMaxCoinBlock();break;case "totalCoins":value=db.getTotalCoins();break;
                    case "cascadeSave":value=db.saveCascade(nil?null:cascades.get(index));break;case "cascadeLoad":value=ArchiveManagerOracle.wire(db.loadCascade());break;
                    case "blockSave":{var block=nil?null:blocks.get(index);if(block!=null&&flag("badHash"))block.getTxPoW().setSuperParent(0,new MiniData(new byte[33]));value=db.saveBlock(block);break;}
                    case "blockID":value=ArchiveManagerOracle.wire(db.loadBlockFromID((String)config.get("id")));break;case "blockNum":value=ArchiveManagerOracle.wire(db.loadBlockFromNum(number(config,"number")));break;
                    case "range":value=ArchiveManagerOracle.wires(db.loadBlockRange(config.get("start")==null?null:new MiniNumber(config.getString("start"))));break;
                    case "txpowSave":{var pow=nil?null:pows.get(index);if(pow!=null&&flag("badHash"))pow.setSuperParent(0,new MiniData(new byte[33]));value=db.saveTxPoW(pow);break;}
                    case "txpowGet":value=ArchiveManagerOracle.wire(db.getTxPoW((String)config.get("id")));break;
                    case "coin":{var coin=ObjectOracle.coin(index);if(config.containsKey("entry"))coin.setMMREntryNumber(new MMREntryNumber(new BigDecimal(new BigInteger(config.getString("entry")),integer(config,"scale",0))));if(flag("noToken"))coin.setToken(null);if(flag("nullState"))coin.setState(null);db.insertCoin(nil?null:coin,number(config,"spentAt"),(String)config.get("date"));break;}
                    case "sql":value=db.searchCoins((String)config.get("sql"),flag("hideToken"));break;default:throw new IllegalArgumentException(cmd);
                }out.put("value",value);
            }catch(Exception error){out.put("exception",error.toString());}finally{System.setOut(previous);}
            out.put("before",before);out.put("after",System.currentTimeMillis());var logs=new JSONArray();for(String line:buffer.toString("UTF-8").split("\n")){int at=line.indexOf("] : ");if(at>=0&&!line.substring(at+4).startsWith("     "))logs.add(line.substring(at+4));}
            out.put("events",events);out.put("logs",logs);outputs.add(out);
        }var result=new JSONObject();result.put("pows",pw);result.put("blocks",bw);result.put("cascades",cw);result.put("ops",outputs);return result;
    }
    public static void main(String[] args)throws Exception{installDriver("jdbc:mysql:");var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(runMySQL((JSONObject)new JSONParser().parse(line)).toJSONString());}catch(Exception error){error.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
