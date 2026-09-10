import java.io.*;
import java.util.*;
import org.minima.system.Main;
import org.minima.system.network.minima.*;
import org.minima.system.commands.CommandRunner;
import org.minima.database.MinimaDB;
import org.minima.database.txpowdb.sql.TxPoWSqlDB;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.database.txpowtree.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
import org.minima.utils.messages.*;
public class MainRuntimeOracle extends SqlDBOracle {
    static JSONArray timers,listenerCalls;
    static ArrayList<JSONObject> held=new ArrayList<>();
    static class ManualMain extends Main {
        ManualMain(){super();}
        public Message take(){return getNextMessage();}
        public void dispatch(Message message)throws Exception{super.processMessage(message);}
        public void PostTimerMessage(TimerMessage timer){timers.add(new JSONArray(Arrays.asList(timer.getMessageType(),timer.getTimer())));if(flag("timerFail"))throw new RuntimeException("fixture timer failure");}
    }
    static class Listener implements MessageListener {
        public void processMessage(Message message)throws Exception{held.add((JSONObject)message.getObject("notify"));var info=new JSONObject();info.put("main",Main.getInstance()!=null);info.put("type",message.getMessageType());listenerCalls.add(info);if(flag("listenerError"))throw new AssertionError("fixture listener failure");if(flag("listenerFail"))throw new RuntimeException("fixture listener failure");}
    }
    static void install(Main main)throws Exception{NIOClientOracle.field(Main.class,"mMainInstance").set(null,main);}
    static JSONObject snapshot(ManualMain main){var out=new JSONObject();out.put("startup",main.isStartUpComplete());out.put("error",main.isStartupError());out.put("errorMessage",main.getStartupErrorMsg());out.put("shutdown",main.isShuttingDown());out.put("restoring",main.isRestoring());out.put("either",main.isShuttongDownOrRestoring());out.put("sync",main.isSyncIBD());out.put("normal",main.isNormalMineMode());out.put("automine",main.AUTOMINE_TIMER);out.put("keys",main.getAllKeysCreated());out.put("instance",Main.getInstance()==main);out.put("listener",Main.getMinimaListener()!=null);out.put("queue",main.getSize());return out;}
    static JSONObject run(JSONObject input,String root)throws Exception {
        config=input;events=new JSONArray();GeneralParams.DATA_FOLDER=root;GeneralParams.RESCUE_MEGAMMR_NODE="";GeneralParams.USE_SQL_COINDB=false;GeneralParams.P2P_ENABLED=false;GeneralParams.P2P2_ENABLED=false;GeneralParams.MYSQL_DB_DELAY=0;GeneralParams.MYSQL_STORE_ALLTXPOW=false;GeneralParams.IBDSYNC_LOGS=false;GeneralParams.DB_IGNORE_LOCKS=false;GeneralParams.IS_MEGAMMR=false;Main.STARTUP_DEBUG_LOGS=false;Main.setMinimaListener(null);held.clear();connectionID=preparedID=0;
        var main=(ManualMain)NIOClientOracle.U.allocateInstance(ManualMain.class);NIOClientOracle.set(main,"mLock",new Object());NIOClientOracle.set(main,"mMessages",new LinkedList<Message>());NIOClientOracle.set(main,"STARTUP_ERROR_MSG","");NIOClientOracle.set(main,"mNormalMineMode",true);NIOClientOracle.set(main,"mOldTip",MiniData.ZERO_TXPOWID);main.AUTOMINE_TIMER=50000;
        for(var pair:new Object[][]{{"CLEANDB_RAM_TIMER",1800000L},{"CLEANDB_SQL_TIMER",43200000L},{"SYSTEMCLEAN_TIMER",300000L},{"MAIN_AUTOBACKUP_MYSQL_TIMER",7200000L},{"AUTOBACKUP_TIMER",86400000L},{"NETRESET_TIMER",86400000L},{"INIT_KEYS_TIMER",10000L},{"CHECKER_TIMER",180000L},{"P2PNETMDS_TIMER",1800000L}})NIOClientOracle.set(main,(String)pair[0],pair[1]);
        install(main);MinimaDB.createDB();var data=new JSONObject();MainNetworkOracle network=null;var outputs=new JSONArray();
        for(Object step:(JSONArray)input.get("ops")){
            config=(JSONObject)step;events=new JSONArray();calls=0;timers=new JSONArray();listenerCalls=new JSONArray();var out=new JSONObject();var bytes=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(bytes,true,"UTF-8"));long before=System.currentTimeMillis();
            try{Object value=null;String cmd=config.getString("cmd");switch(cmd){
                case "fields":{var values=(JSONObject)config.get("values");for(Object raw:values.keySet()){String key=(String)raw;String field=switch(key){case "restoring"->"mRestoring";case "shutdown"->"mShuttingdown";case "keys"->"mInitKeysCreated";case "complete"->"STARTUP_COMPLETE";case "importing"->"MYSQL_IMPORTING_NO_ACTION";case "uptime"->"mUptimeMilli";default->throw new IllegalArgumentException(key);};NIOClientOracle.set(main,field,values.get(key));}break;}
                case "logFlags":{if(config.containsKey("values")){var values=(JSONObject)config.get("values");
                    if(values.containsKey("scripts"))GeneralParams.SCRIPTLOGS=(Boolean)values.get("scripts");
                    if(values.containsKey("mining"))GeneralParams.MINING_LOGS=(Boolean)values.get("mining");
                    if(values.containsKey("maxima"))GeneralParams.MAXIMA_LOGS=(Boolean)values.get("maxima");
                    if(values.containsKey("blocks"))GeneralParams.BLOCK_LOGS=(Boolean)values.get("blocks");
                    if(values.containsKey("networking"))GeneralParams.NETWORKING_LOGS=(Boolean)values.get("networking");
                    if(values.containsKey("ibd"))GeneralParams.IBDSYNC_LOGS=(Boolean)values.get("ibd");
                    if(values.containsKey("peerschecker"))GeneralParams.PEERSCHECKER_lOG=(Boolean)values.get("peerschecker");
                    if(values.containsKey("txpowdb"))TxPoWSqlDB.LOG_ADD_BLOCKS=(Boolean)values.get("txpowdb");
                    if(values.containsKey("txpowTransactions"))TxPoWSqlDB.LOG_ADD_TRANSACTION=(Boolean)values.get("txpowTransactions");
                }var flags=new JSONObject();
                flags.put("scripts",GeneralParams.SCRIPTLOGS);
                flags.put("mining",GeneralParams.MINING_LOGS);
                flags.put("maxima",GeneralParams.MAXIMA_LOGS);
                flags.put("blocks",GeneralParams.BLOCK_LOGS);
                flags.put("networking",GeneralParams.NETWORKING_LOGS);
                flags.put("ibd",GeneralParams.IBDSYNC_LOGS);
                flags.put("peerschecker",GeneralParams.PEERSCHECKER_lOG);
                flags.put("txpowdb",TxPoWSqlDB.LOG_ADD_BLOCKS);
                flags.put("txpowTransactions",TxPoWSqlDB.LOG_ADD_TRANSACTION);
                value=flags;break;}
                case "syncCurrent":main.setSyncIBD(flag("value"));break;
                case "diagnostics":{if(flag("reset")){MessageProcessor.setTrace(false,"");NIOClient.mTraceON=false;NIOServer.mTraceON=false;}var diagnostics=new JSONObject();diagnostics.put("trace",main.isTrace());diagnostics.put("filter",main.getTraceFilter());diagnostics.put("client",NIOClient.mTraceON);diagnostics.put("server",NIOServer.mTraceON);if(config.containsKey("match"))diagnostics.put("matches",main.checkTraceFilter(config.getString("match")));value=diagnostics;break;}
                case "normal":main.setNormalAutoMineSpeed();break;case "low":main.setLowPowAutoMineSpeed();break;
                case "sync":GeneralParams.IBDSYNC_LOGS=flag("logs");main.setSyncIBD(flag("value"));break;
                case "error":main.setStartUpError(flag("value"),(String)config.get("message"));break;case "uptime":value=main.getUptimeMilli();break;
                case "listener":Main.setMinimaListener(flag("enabled")?new Listener():null);break;
                case "notify1":Main.NotifyMainListenerOnly((String)config.get("event"));break;
                case "notify2":Main.NotifyMainListenerOnly((String)config.get("event"),flag("null")?null:data);break;
                case "notifyTo":main.PostNotifyEvent((String)config.get("event"),flag("null")?null:data,(String)config.get("to"));break;
                case "mutate":data.put(config.getString("key","value"),config.get("value"));break;
                case "clear":Main.ClearMainInstance();break;case "publish":install(main);break;case "shutdownNotice":main.NotifyMainListenerOfShutDown();break;
                case "network":network=new MainNetworkOracle(config,main);NIOClientOracle.set(main,"mNetwork",flag("nullNetwork")?null:network.network);break;
                case "networkSnapshot":value=network.snapshot();break;
                case "rescueNode":GeneralParams.RESCUE_MEGAMMR_NODE=config.getString("host");break;
                case "drain":{var queued=new JSONArray();Message message;while((message=main.take())!=null){var item=new JSONObject();item.put("type",message.getMessageType());item.put("host",message.getString("host"));queued.add(item);}value=queued;break;}
                case "run":value=CommandRunner.getRunner().runMultiCommand(config.getString("caller","0x00"),config.getString("text"));break;
                case "direct":case "meta":{var command=CommandRunner.getRunner().getCommandOnly(config.getString("name"));if(cmd.equals("direct")){command.getParams().putAll((JSONObject)config.get("params"));value=command.runCommand();}else{var meta=new JSONObject();meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullHelp",command.getFullHelp());meta.put("valid",new JSONArray(command.getValidParams()));var clone=command.getFunction();meta.put("fresh",clone!=command&&clone.getParams()!=command.getParams());value=meta;}break;}
                case "command":value=main.runSingleMinimaCMD((String)config.get("text"));break;
                case "shutdown":main.shutdown(flag("compact"));break;case "restore":main.restoreReady(flag("mds"));break;case "final":main.shutdownFinalProcs(flag("mds"));break;
                case "keysSize":value=main.getAllDefaultKeysSize();break;case "prefs":MinimaDB.getDB().getUserDB().getAllData().putAll((JSONObject)config.get("values"));break;
                case "healthChain":{var db=MinimaDB.getDB();db.resetCascadeAndTxPoWTree();var nodes=new ArrayList<TxPoWTreeNode>();var blocks=(JSONArray)config.get("blocks");
                    for(int i=0;i<blocks.size();i++){var pow=ArchiveManagerOracle.txpow(i,0);pow.setBlockNumber(new MiniNumber((String)blocks.get(i)));pow.calculateTXPOWID();var node=new TxPoWTreeNode(pow);if(i>0){int parent=config.containsKey("parents")?((Number)((JSONArray)config.get("parents")).get(i)).intValue():i-1;nodes.get(parent).addChildNode(node);}else db.getTxPoWTree().setRoot(node);nodes.add(node);}db.getTxPoWTree().recalculateTree();
                    int i=0;for(Object number:(JSONArray)config.get("cascade")){var pow=ArchiveManagerOracle.txpow(i++,0);pow.setBlockNumber(new MiniNumber((String)number));pow.calculateTXPOWID();db.getCascade().addToTip(pow);}if(flag("nullCascade"))db.setIBDCascade(null);break;}
                case "chain":{var db=MinimaDB.getDB();db.resetCascadeAndTxPoWTree();TxPoWTreeNode prev=null;for(int i=0;i<integer(config,"count",3);i++){var node=new TxPoWTreeNode(ArchiveManagerOracle.txpow(i,0));if(prev!=null)prev.addChildNode(node);else db.getTxPoWTree().setRoot(node);prev=node;}db.getTxPoWTree().recalculateTree();break;}
                case "mysqlConfig":GeneralParams.MYSQL_DB_DELAY=integer(config,"delay",0);GeneralParams.MYSQL_DB_COINS=flag("coins");org.minima.system.commands.backup.mysql.convertMySQLParams(config.getString("text"));break;
                case "message":{GeneralParams.TXBLOCK_NODE=flag("slave");GeneralParams.MYSQL_STORE_ALLTXPOW=flag("storeTxPoW");var message=new Message((String)config.get("type"));if(config.containsKey("fields")){var values=(JSONObject)config.get("fields");for(Object raw:values.keySet()){String key=(String)raw;Object item=values.get(key);if(item==null)continue;if(item instanceof Long l)item=l.intValue();message.addObject(key,item);}}if(config.containsKey("pow"))message.addObject("txpow",ArchiveManagerOracle.txpow(integer(config,"pow",0),0));main.dispatch(message);break;}
                case "info":break;default:throw new IllegalArgumentException(cmd);
            }out.put("value",value);}catch(Throwable error){out.put("exception",error.toString());}finally{System.setOut(previous);}
            long after=System.currentTimeMillis();var logs=new JSONArray();for(String line:bytes.toString("UTF-8").split("\n")){int at=line.indexOf("] : ");if(at>=0&&!line.substring(at+4).startsWith("     "))logs.add(line.substring(at+4));}
            out.put("before",before);out.put("after",after);out.put("logs",logs);out.put("timers",timers);out.put("events",events);out.put("listeners",listenerCalls);out.put("held",new JSONArray(held));out.put("state",snapshot(main));out.put("user",new JSONObject(MinimaDB.getDB().getUserDB().getAllData()));outputs.add(out);
        }Main.setMinimaListener(null);install(null);var result=new JSONObject();result.put("ops",outputs);return result;
    }
    public static void main(String[] args)throws Exception{installDriver("jdbc:");var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{System.out.println(run((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Throwable e){e.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
