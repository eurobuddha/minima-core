import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.txpowtree.CoinDB;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.system.commands.*;
import org.minima.system.commands.backup.*;
import org.minima.utils.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class RestoreOracle {
    static JSONArray events;static JSONObject request;
    static class ManualMain extends Main {
        ManualMain(){super();}
        public void restoreReady(boolean shutdown){
            var event=new JSONArray();event.add("ready");event.add(shutdown);events.add(event);
            if(Boolean.TRUE.equals(request.get("readyFail")))throw new RuntimeException("fixture ready failure");
            if(Boolean.TRUE.equals(request.get("realReady"))){super.restoreReady(shutdown);return;}
            try{NIOClientOracle.set(this,"mRestoring",true);}catch(Exception error){throw new RuntimeException(error);}
        }
        public void restoreReadyForSync(){
            events.add("readyForSync");
            if(Boolean.TRUE.equals(request.get("syncFail")))throw new RuntimeException("fixture sync failure");
            if(Boolean.TRUE.equals(request.get("realSync")))super.restoreReadyForSync();
        }
        public void setHasShutDown(){events.add("shutdown");super.setHasShutDown();}
        public void stopMessageProcessor(){events.add("stop");super.stopMessageProcessor();}
    }
    static class ResetChild extends Command {
        final String child;
        ResetChild(String name){super(name,"fixture child");child=name;}
        public Command getFunction(){return new ResetChild(child);}
        public ArrayList<String> getValidParams(){return child.equals("restore")?new restore().getValidParams():new archive().getValidParams();}
        public JSONObject runCommand()throws Exception{
            var event=new JSONObject();event.put("child",child);event.put("text",getCompleteCommand());event.put("params",getParams());event.put("caller",getMiniDAPPID());events.add(event);
            if(child.equals("restore")&&Boolean.TRUE.equals(request.get("realRestore"))){
                var command=new restore();command.getParams().putAll(getParams());command.setCompleteCommand(getCompleteCommand());command.setMiniDAPPID(getMiniDAPPID());return command.runCommand();
            }
            if(request.containsKey(child+"Throw"))throw new RuntimeException(request.getString(child+"Throw"));
            if(request.containsKey(child+"Reply"))return (JSONObject)request.get(child+"Reply");
            var result=getJSONReply();result.put("response","fixture "+child);return result;
        }
    }
    static class ResetChildrenScope implements AutoCloseable {
        final Command[] saved=CommandRunner.ALL_COMMANDS.clone();
        ResetChildrenScope(boolean active){if(active)for(int i=0;i<saved.length;i++)if(saved[i].getName().equals("restore")||saved[i].getName().equals("archive"))CommandRunner.ALL_COMMANDS[i]=new ResetChild(saved[i].getName());}
        public void close(){for(int i=0;i<saved.length;i++)CommandRunner.ALL_COMMANDS[i]=saved[i];}
    }
    static JSONObject run(JSONObject input)throws Exception {
        request=input;events=new JSONArray();var out=new JSONObject();String folder=(String)input.get("folder"),op=(String)input.get("op");GeneralParams.DATA_FOLDER=folder;GeneralParams.BASE_FILE_FOLDER=folder;GeneralParams.USE_SQL_COINDB=false;GeneralParams.DB_IGNORE_LOCKS=false;
        var main=(ManualMain)NIOClientOracle.U.allocateInstance(ManualMain.class);NIOClientOracle.set(main,"mLock",new Object());NIOClientOracle.set(main,"mMessages",new LinkedList<>());NIOClientOracle.set(main,"mRunning",true);NIOClientOracle.set(main,"mInitKeysCreated",!Boolean.FALSE.equals(input.get("keys")));NIOClientOracle.field(Main.class,"mMainInstance").set(null,main);Main.setMinimaListener(message->events.add(message.getMessageType()));
        MinimaDB.createDB();var db=MinimaDB.getDB();db.loadAllDB();
        if("seed".equals(op)){
            boolean source="source".equals(input.get("role"));String marker=source?"source":"target";db.getUserDB().setString("fixture_marker",marker);var uninstalled=new JSONArray();uninstalled.add("fixture-dapp");db.getUserDB().setJSONArray("minidapps_uninstalled",uninstalled);db.getP2PDB().setString("fixture_marker",marker);
            var sql=db.getTxPoWDB().getSQLDB();for(int i=0;i<(source?10:2);i++)sql.addTxPoW(ArchiveCacheOracle.txpow(source?i:i+20,0),source?i%3!=0:true);sql.executeGenericSQL("UPDATE txpow SET timemilli=id*1000");
            String query=source?"INSERT INTO keys(size,depth,uses,maxuses,modifier,privatekey,publickey) VALUES(2,2,5,1000,'0x01','0xABCD','0x01'),(2,2,100,1000,'0x02','0x1234','0x02')":"INSERT INTO keys(size,depth,uses,maxuses,modifier,privatekey,publickey) VALUES(2,2,999,1000,'0x03','0x5678','0x03')";
            var result=db.getWallet().executeGenericSQL(query);if(!Boolean.TRUE.equals(result.get("status")))throw new RuntimeException("fixture wallet insert failed");
            if(source){var command=new backup();command.getParams().put("file","source.bak");out.put("backup",command.runCommand());}
            db.saveAllDB(false);out.put("seeded",true);Main.setMinimaListener(null);return out;
        }
        var oldWallet=db.getWallet();var oldTxPoWDB=db.getTxPoWDB();var oldArchive=db.getArchive();boolean resetCommand="reset".equals(input.get("name"));
        try(var children=new ResetChildrenScope(resetCommand)){
            Command command=resetCommand?new reset():new restore();
            if("meta".equals(op)){out.put("name",command.getName());out.put("help",command.getHelp());out.put("fullHelp",command.getFullHelp());var params=new JSONArray();params.addAll(command.getValidParams());out.put("params",params);command.getParams().put("file","fixture");out.put("fresh",command.getFunction().getParams());}
            else if("runner".equals(op))out.put("result",CommandRunner.getRunner().runMultiCommand(input.containsKey("caller")?input.getString("caller"):"0x00",(String)input.get("command")));
            else{command.getParams().putAll((JSONObject)input.get("params"));out.put("result",command.runCommand());}
        }catch(Throwable error){out.put("exception",error.toString());}
        out.put("events",events);out.put("restoring",main.isRestoring());out.put("shutdown",main.isShuttingDown());out.put("published",Main.getInstance()==main);out.put("running",main.isRunning());out.put("allowSave",NIOClientOracle.get(db,"mAllowSaveState"));
        out.put("user",db.getUserDB().getAllData());out.put("p2p",db.getP2PDB().getAllData());out.put("cascade",MiniData.getMiniDataVersion(db.getCascade()).to0xString());out.put("tree",MiniData.getMiniDataVersion(db.getTxPoWTree()).to0xString());
        String cached=db.getWallet().getBaseSeed().getSeed();out.put("cachedSeedHash",cached==null?"null":new MiniData(Crypto.getInstance().hashData(new MiniString(cached).getData())).to0xString());
        out.put("walletOpen",db.getWallet().isOpen());out.put("historyOpen",db.getTxPoWDB().getSQLDB().isOpen());out.put("archiveOpen",db.getArchive().isOpen());out.put("archiveFile",db.getArchive().getSQLFile().exists());
        out.put("keys",db.getWallet().executeGenericSQL("SELECT publickey,uses FROM keys ORDER BY publickey"));out.put("seed",db.getWallet().executeGenericSQL("SELECT RAWTOHEX(HASH('SHA-256',seed)) AS seedhash FROM seed"));out.put("history",db.getTxPoWDB().getSQLDB().executeGenericSQL("SELECT txpowid,isrelevant FROM txpow ORDER BY txpowid"));
        if(resetCommand){out.put("processorCreated",main.getTxPoWProcessor()!=null);out.put("walletReplaced",oldWallet!=db.getWallet());}
        if(main.getTxPoWProcessor()!=null){main.getTxPoWProcessor().stopMessageProcessor();main.getTxPoWProcessor().waitToShutDown();}
        oldWallet.hardCloseDB();oldTxPoWDB.hardCloseSQLDB();oldArchive.hardCloseDB();
        db.getWallet().hardCloseDB();db.getTxPoWDB().hardCloseSQLDB();db.getArchive().hardCloseDB();CoinDB.getTxPoWTreeCoinDB().hardCloseDB();Main.setMinimaListener(null);return out;
    }
    public static void main(String[] ignored)throws Exception {
        Locale.setDefault(Locale.ENGLISH);org.h2.Driver.load();var input=new BufferedReader(new InputStreamReader(System.in));String line;var stdout=System.out;
        while((line=input.readLine())!=null){JSONObject result;System.setOut(System.err);try{result=run((JSONObject)new JSONParser().parse(line));}catch(Throwable error){result=new JSONObject();result.put("fixtureError",error.toString());}finally{System.setOut(stdout);}System.out.println(result.toJSONString());}
    }
}
