import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.brains.TxPoWMiner;
import org.minima.system.params.GeneralParams;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
// Executes the actual pinned miner bodies; no Main constructor or worker starts.
public class TxPoWMinerOracle extends NIOClientOracle {
    static ArrayList<TxPoW> txs;static JSONArray events;static ArrayList<Message> posted;static int calls,failAt;
    static long num(JSONObject v,String key,long fallback){return v.containsKey(key)?((Number)v.get(key)).longValue():fallback;}
    static boolean flag(JSONObject v,String key){return Boolean.TRUE.equals(v.get(key));}
    static class Owner extends Main {
        Owner()throws Exception{super();}
        public void PostMessage(Message m){events.add(describe(m));posted.add(m);if(++calls==failAt)throw new RuntimeException("fixture post failure");}
        public void PostNotifyEvent(String name,JSONObject data){}
    }
    static class Miner extends TxPoWMiner {
        ArrayList<TimerMessage> timers;
        void process(Message m)throws Exception{super.processMessage(m);}
        Message next(){return getNextMessage();}
        public void PostTimerMessage(TimerMessage t){timers.add(t);}
    }
    static Object state(TxPoW t){if(t==null)return null;var out=new JSONObject();out.put("wire",MiniData.getMiniDataVersion(t).to0xString());out.put("id",t.getTxPoWID());out.put("nonce",t.getNonce().toString());out.put("size",t.getSizeinBytes());out.put("transaction",t.isTransaction());return out;}
    static JSONObject describe(Message m){var out=new JSONObject();out.put("type",m.getMessageType());for(String key:new String[]{"starting","automine"})if(m.exists(key))out.put(key,m.getBoolean(key));var t=(TxPoW)m.getObject("txpow");out.put("tx",t==null?-1:txs.indexOf(t));out.put("state",state(t));return out;}
    static Object run(JSONObject input)throws Exception {
        txs=new ArrayList<>();for(Object raw:(JSONArray)input.get("txs")){var spec=(JSONObject)raw;var t=ArchiveManagerOracle.txpow((int)num(spec,"seed",1),0);
            for(String key:new String[]{"inputs","burn"})if(spec.containsKey(key))for(Object index:(JSONArray)spec.get(key))(key.equals("inputs")?t.getTransaction():t.getBurnTransaction()).addInput(ObjectOracle.coin(((Number)index).intValue()));
            if(spec.containsKey("difficulty"))t.setTxDifficulty(new MiniData((String)spec.get("difficulty")));t.setHeaderBodyHash();t.calculateTXPOWID();txs.add(t);
        }
        MinimaDB.createDB();var owner=(Owner)U.allocateInstance(Owner.class);owner.AUTOMINE_TIMER=num(input,"delay",50000);field(Main.class,"mMainInstance").set(null,owner);
        var miner=(Miner)U.allocateInstance(Miner.class);set(miner,"mLock",new Object());set(miner,"mMessages",new LinkedList<Message>());set(miner,"mRunning",true);set(miner,"mMiningCoins",new ArrayList<String>());miner.timers=new ArrayList<>();posted=new ArrayList<>();var outputs=new JSONArray();
        for(Object raw:(JSONArray)input.get("ops")){var op=(JSONObject)raw;events=new JSONArray();calls=0;failAt=(int)num(op,"failAt",0);int priorTimers=miner.timers.size();GeneralParams.MINING_LOGS=flag(op,"logs");GeneralParams.TXBLOCK_NODE=flag(op,"blockNode");
            var out=new JSONObject();out.put("before",System.currentTimeMillis());var log=new ByteArrayOutputStream();var prior=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
            try{Object result=null;int index=(int)num(op,"tx",0);TxPoW t=index<0?null:txs.get(index);switch((String)op.get("cmd")){
                case "queue":miner.mineTxPoWAsync(t);break;case "next":miner.process(miner.next());break;case "sync":result=miner.MineMaxTxPoW(flag(op,"maxima"),t,num(op,"limit",100000),!op.containsKey("post")||flag(op,"post"));break;
                case "process":{var msg=new Message(TxPoWMiner.TXPOWMINER_MINETXPOW).addObject("txpow",t);if(op.containsKey("automine"))msg.addBoolean("automine",flag(op,"automine"));miner.process(msg);break;}
                case "stop":miner.stopMessageProcessor();break;case "pulse":miner.process(new Message(TxPoWMiner.TXPOWMINER_MINEPULSE));break;case "block":miner.process(new Message(TxPoWMiner.TXPOWMINER_TXBLOCKMINER));break;case "unknown":miner.process(new Message("unknown"));break;
                case "clearBody":t.clearBody();break;case "difficulty":t.setTxDifficulty(new MiniData((String)op.get("value")));break;case "input":t.getTransaction().addInput(ObjectOracle.coin((int)num(op,"coin",0)));break;case "check":result=miner.checkForMiningCoin((String)op.get("id"));break;default:throw new IllegalArgumentException((String)op.get("cmd"));
            }out.put("value",result);}catch(Exception e){out.put("exception",e.toString());}finally{System.setOut(prior);}
            var logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int i=line.indexOf("] : ");if(i>=0){String text=line.substring(i+4);if(!text.startsWith("     "))logs.add(text);}}out.put("logs",logs);out.put("events",events);
            var timers=new JSONArray();for(int i=priorTimers;i<miner.timers.size();i++){var row=new JSONObject();var timer=miner.timers.get(i);row.put("type",timer.getMessageType());row.put("deadline",timer.getTimer());timers.add(row);}out.put("timers",timers);
            var pending=new JSONArray();for(var msg:(LinkedList<Message>)get(miner,"mMessages"))pending.add(describe(msg));var retained=new JSONArray();for(var msg:posted)retained.add(describe(msg));var states=new JSONArray();for(var tx:txs)states.add(state(tx));var coins=new JSONArray();for(int i=0;i<6;i++)coins.add(miner.checkForMiningCoin(ObjectOracle.coin(i).getCoinID().to0xString()));
            out.put("pending",pending);out.put("retained",retained);out.put("states",states);out.put("coins",coins);out.put("running",miner.isRunning());out.put("after",System.currentTimeMillis());outputs.add(out);
        }
        var out=new JSONObject();out.put("ops",outputs);return out;
    }
    public static void main(String[] args)throws Exception{var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
