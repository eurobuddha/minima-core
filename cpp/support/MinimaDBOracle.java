import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.txpowtree.CoinDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.objects.mmr.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.commands.CommandRunner;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
import org.minima.utils.messages.*;
// Actual pinned MinimaDB and all child implementations. JDBC and Main's
// external effects are recorded; Main's constructor/worker is never started.
public class MinimaDBOracle extends SqlDBOracle {
    static JSONArray mainEvents,pulseEvents;
    static int pulseCalls;
    static MinimaDB pulseBacking;
    static class PulseFaultDB extends MinimaDB {
        void call(String name){pulseEvents.add(name);if(++pulseCalls==integer(config,"pulseFail",-1)){if(flag("pulseError"))throw new AssertionError("fixture pulse failure");throw new RuntimeException("fixture pulse failure");}}
        public void readLock(boolean lock){call("readLock("+lock+")");pulseBacking.readLock(lock);}
        public org.minima.database.txpowtree.TxPowTree getTxPoWTree(){call("getTxPoWTree");if(flag("pulseClearDB"))try{NIOClientOracle.field(MinimaDB.class,"mMinimaDB").set(null,null);}catch(Exception e){throw new RuntimeException(e);}return flag("pulseNullTree")?null:pulseBacking.getTxPoWTree();}
    }
    static class Owner extends Main {
        Owner()throws Exception{super();}
        public boolean isShuttingDown(){return false;}
        public void PostMessage(Message message){mainEvents.add(new JSONArray(Arrays.asList("message",message.getMessageType())));}
        public void PostTimerMessage(TimerMessage timer){mainEvents.add(new JSONArray(Arrays.asList("timer",timer.getMessageType(),timer.getTimer())));}
        public void PostNotifyEvent(String type,JSONObject data){}
        public void setStartUpError(boolean value,String error){mainEvents.add(new JSONArray(Arrays.asList("startup",value,error)));}
    }
    static class Identities {
        ArrayList<Object> objects=new ArrayList<>();
        int id(Object value){if(value==null)return -1;for(int i=0;i<objects.size();i++)if(objects.get(i)==value)return i;objects.add(value);return objects.size()-1;}
        JSONArray snapshot(MinimaDB db){var out=new JSONArray();for(Object value:new Object[]{db,db.getArchive(),db.getTxPoWDB(),db.getTxPoWTree(),db.getCascade(),db.getUserDB(),db.getCustomTxnDB(),db.getWallet(),db.getTxBlockDB(),db.getMegaMMR(),db.getP2PDB(),db.getP2P2DB()})out.add(id(value));return out;}
    }
    static JSONArray pulseDescribe(ArrayList<MiniData> list){if(list==null)return null;var out=new JSONArray();for(var id:list)out.add(id.to0xString());return out;}
    static JSONObject runDB(JSONObject input,String root)throws Exception{
        config=new JSONObject();config.put("rows",new JSONArray());config.put("columns",new JSONArray());events=new JSONArray();calls=connectionID=preparedID=0;Main.STARTUP_DEBUG_LOGS=false;
        GeneralParams.DATA_FOLDER=root;GeneralParams.DB_IGNORE_LOCKS=false;GeneralParams.IS_MEGAMMR=false;GeneralParams.IS_MOBILE=true;GeneralParams.IS_JNLP=false;GeneralParams.RESCUE_MEGAMMR_NODE="";GeneralParams.TEST_PARAMS=false;GeneralParams.USE_SQL_COINDB=false;
        NIOClientOracle.field(Main.class,"mMainInstance").set(null,NIOClientOracle.U.allocateInstance(Owner.class));Thread.currentThread().setName("fixture");
        MinimaDB.createDB();var db=MinimaDB.getDB();db.writeLock(true);db.writeLock(false);CoinDB.createCoinDB(new File(root+"/bootstrap/coins"));connectionID=preparedID=0;
        var ids=new Identities();var original=ids.snapshot(db);var held=db;TxnDB txn=null;var outputs=new JSONArray();var query=new MMR();MMR heldMMR=null;
        var pulse=new org.minima.objects.Pulse();ArrayList<MiniData> heldPulseList=null;org.minima.objects.Pulse.PULSE_VERSION=MiniNumber.ONE;
        for(Object item:(JSONArray)input.get("ops")){
            config=(JSONObject)item;calls=0;events=new JSONArray();mainEvents=new JSONArray();pulseEvents=new JSONArray();pulseCalls=0;var out=new JSONObject();var log=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
            Main.STARTUP_DEBUG_LOGS=flag("debug");GeneralParams.SEED_PHRASE="fixture seed";GeneralParams.ANYSEED_PHRASE=true;
            if(config.containsKey("ignore"))GeneralParams.DB_IGNORE_LOCKS=flag("ignore");if(config.containsKey("mega"))GeneralParams.IS_MEGAMMR=flag("mega");GeneralParams.IS_MAIN_DBPASSWORD_SET=flag("encrypted");GeneralParams.MAIN_DBPASSWORD="fixture password";GeneralParams.RESCUE_MEGAMMR_NODE=config.get("rescue")==null?"":(String)config.get("rescue");
            out.put("before",System.currentTimeMillis());try{Object value=null;String targetName=(String)config.get("target");MMR target="tip".equals(targetName)?db.getTxPoWTree().getTip().getMMR():"global".equals(targetName)?db.getMegaMMR().getMMR():"held".equals(targetName)?heldMMR:query;switch((String)config.get("cmd")){
                case "read":db.readLock(flag("lock"));break;case "write":db.writeLock(flag("lock"));break;case "safe":db.safeReleaseReadWriteLock();break;
                case "create":MinimaDB.createDB();db=MinimaDB.getDB();break;case "reset":db.resetCascadeAndTxPoWTree();break;case "cascade":db.setIBDCascade(flag("null")?null:held.getCascade());break;case "mega":db.hardSetMegaMMR(flag("null")?null:held.getMegaMMR());break;case "hold":held=db;break;case "useHeld":db=held;break;
                case "path":GeneralParams.DATA_FOLDER=config.containsKey("base")?(String)config.get("base"):root;value=db.getDBFile((String)config.get("name")).toString();GeneralParams.DATA_FOLDER=root;break;
                case "add":db.addCoinNotify((String)config.get("address"));break;case "remove":value=db.removeCoinNotify((String)config.get("address"));break;case "check":value=db.checkCoinNotify((String)config.get("address"));break;
                case "load":db.loadAllDB();break;case "reload":db.loadArchiveAndTxPoWDB(flag("wallet"));break;case "restore":db.loadDBsForRestoreSync();break;case "refresh":db.refreshSQLDB();break;case "restart":db.fullDBRestartMemFree();break;
                case "allow":db.setAllowSaveState(flag("allow"));break;case "save":db.saveAllDB(flag("compact"));break;case "saveSQL":db.saveSQL(flag("compact"));break;case "saveState":db.saveState();break;case "saveUser":db.saveUserDB();break;case "saveP2P":db.saveP2PDB();break;
                case "holdTxn":txn=db.getCustomTxnDB();break;case "saveTxn":txn.saveDB();break;case "newTxn":db.getCustomTxnDB().createTransaction((String)config.get("id"));break;
                case "mmrHold":heldMMR=db.getMegaMMR().getMMR();break;case "mmrClearMega":db.getMegaMMR().clear();break;case "mmrClear":target.clearParent();break;case "mmrChild":query=new MMR(query);break;case "mmrTime":target.setBlockTime(new MiniNumber(integer(config,"time",0)));break;
                case "mmrSet":{var entry=target.setEntry(integer(config,"row",0),new MMREntryNumber(integer(config,"entry",0)),new MMRData(new MiniData("0x01"),new MiniNumber(integer(config,"amount",1))));value=entry==null?null:entry.toJSON();break;}
                case "mmrAdd":value=target.addEntry(new MMRData(new MiniData("0x01"),new MiniNumber(integer(config,"amount",1)))).toJSON();break;
                case "mmrGet":value=target.getEntry(integer(config,"row",0),new MMREntryNumber(integer(config,"entry",0)),new MiniNumber(integer(config,"back",0))).toJSON();break;
                case "mmrWire":value=MiniData.getMiniDataVersion(target).to0xString();break;
                case "truncate":{String name=(String)config.get("name");if(!Set.of("cascade.db","chaintree.db","megammr.mmr","userprefs.db").contains(name))throw new IllegalArgumentException("Unexpected fixture file");var file=db.getDBFile(name).toPath();var bytes=java.nio.file.Files.readAllBytes(file);java.nio.file.Files.write(file,Arrays.copyOf(bytes,Math.min(bytes.length,integer(config,"length",0))));break;}
                case "snapshot":{var state=new JSONObject();state.put("cascade",MiniData.getMiniDataVersion(db.getCascade()).to0xString());state.put("tree",MiniData.getMiniDataVersion(db.getTxPoWTree()).to0xString());state.put("mega",MiniData.getMiniDataVersion(db.getMegaMMR()).to0xString());value=state;break;}
                case "command":{
                    var runner=CommandRunner.getRunner();String mode=config.getString("mode","direct");
                    if(mode.equals("runner"))value=runner.runMultiCommand(config.getString("caller","0x00"),config.getString("text"));
                    else{var command=runner.getCommandOnly(config.getString("name"));if(config.containsKey("params"))command.getParams().putAll((JSONObject)config.get("params"));
                        if(mode.equals("meta")){var meta=new JSONObject();meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("valid",command.getValidParams());command.getParams().put("fixture","old");meta.put("fresh",command.getFunction().getJSONReply());value=meta;}
                        else value=command.runCommand();}break;
                }
                case "tipTime":{var pow=db.getTxPoWTree().getTip().getTxPoW();pow.setTimeMilli(new MiniNumber(config.getString("time")));pow.calculateTXPOWID();db.getTxPoWTree().recalculateTree();break;}
                case "chain":{db.resetCascadeAndTxPoWTree();TxPoWTreeNode previousNode=null;var nodes=new ArrayList<TxPoWTreeNode>();int start=integer(config,"start",1);for(int i=0;i<integer(config,"count",3);i++){if(config.containsKey("parents")){int parent=((Number)((JSONArray)config.get("parents")).get(i)).intValue();previousNode=parent<0?null:nodes.get(parent);}var pow=ArchiveManagerOracle.txpow(start+i,0);if(previousNode!=null){pow.setSuperParent(0,previousNode.getTxPoW().getTxPoWIDData());pow.calculateTXPOWID();}var node=new TxPoWTreeNode(pow);if(previousNode!=null)previousNode.addChildNode(node);else db.getTxPoWTree().setRoot(node);previousNode=node;nodes.add(node);}db.getTxPoWTree().recalculateTree();if(!flag("emptyCascade"))db.getCascade().addToTip(ArchiveManagerOracle.txpow(integer(config,"cascadeHeight",start-1),0));break;}
                case "pulseFault":pulseBacking=db;NIOClientOracle.field(MinimaDB.class,"mMinimaDB").set(null,NIOClientOracle.U.allocateInstance(PulseFaultDB.class));break;
                case "pulseRestore":NIOClientOracle.field(MinimaDB.class,"mMinimaDB").set(null,db);break;
                case "pulse":pulse=org.minima.objects.Pulse.createPulse();break;
                case "pulseNew":pulse=new org.minima.objects.Pulse();break;
                case "pulseVersion":org.minima.objects.Pulse.PULSE_VERSION=new MiniNumber(config.getString("version"));break;
                case "pulseHold":heldPulseList=config.getString("target","pulse").equals("tree")?db.getTxPoWTree().getPulseList():pulse.getBlockList();break;
                case "pulseSet":{String mode=config.getString("target","held");pulse.setBlockList(mode.equals("null")?null:mode.equals("tree")?db.getTxPoWTree().getPulseList():mode.equals("new")?new ArrayList<>():heldPulseList);break;}
                case "pulseMutate":{String name=config.getString("target","pulse");var list=name.equals("tree")?db.getTxPoWTree().getPulseList():name.equals("held")?heldPulseList:pulse.getBlockList();if(config.getString("action","add").equals("clear"))list.clear();else list.add(new MiniData(config.getString("id","0x01")));break;}
                case "pulseRecalc":db.getTxPoWTree().recalculateTree();break;
                case "pulseWire":value=ArchiveManagerOracle.wire(pulse);break;
                case "pulseDataRead":{var bytes=new ByteArrayInputStream(new MiniData(config.getString("wire")).getBytes());try{value=MiniData.ReadFromStream(new DataInputStream(bytes),integer(config,"expected",-1)).to0xString();}finally{out.put("remaining",bytes.available());}break;}
                case "pulseRead":{var bytes=new ByteArrayInputStream(new MiniData(config.getString("wire")).getBytes());try{pulse.readDataStream(new DataInputStream(bytes));}finally{out.put("remaining",bytes.available());}break;}
                case "pulseSnapshot":{var tree=db.getTxPoWTree().getPulseList();var current=pulse.getBlockList();var state=new JSONObject();state.put("pulse",pulseDescribe(current));state.put("held",pulseDescribe(heldPulseList));state.put("tree",pulseDescribe(tree));state.put("pulseTree",current==tree);state.put("pulseHeld",current==heldPulseList);state.put("heldTree",heldPulseList==tree);value=state;break;}
                case "info":break;default:throw new IllegalArgumentException((String)config.get("cmd"));
            }out.put("value",value);}catch(Throwable error){out.put("exception",error.toString());}finally{System.setOut(previous);GeneralParams.DATA_FOLDER=root;}
            if(config.getString("cmd").startsWith("pulse"))out.put("pulseCalls",pulseEvents);
            var logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int i=line.indexOf("] : ");if(i>=0){String body=line.substring(i+4);if(!body.startsWith("     "))logs.add(body);}}
            out.put("logs",logs);out.put("events",events);out.put("main",mainEvents);out.put("ids",ids.snapshot(db));out.put("global",ids.id(MinimaDB.getDB()));out.put("lock",db.getRWLockInfo());out.put("thread",MinimaDB.mCurrentWriteLockThread);out.put("state",MinimaDB.mCurrentWriteLockState);out.put("user",new JSONObject(db.getUserDB().getAllData()));out.put("sizes",new JSONArray(Arrays.asList(db.getCascadeFileSize(),db.getUserDBFileSize(),db.getTxPowTreeFileSize(),db.getP2PFileSize())));out.put("after",System.currentTimeMillis());outputs.add(out);
        }
        var out=new JSONObject();out.put("original",original);out.put("ops",outputs);return out;
    }
    public static void main(String[] args)throws Exception{installDriver();var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{System.out.println(runDB((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Exception error){System.out.println("{\"fatal\":true}");error.printStackTrace(System.err);}}
}
