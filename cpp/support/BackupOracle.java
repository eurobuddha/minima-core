import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.objects.TxBlock;
import org.minima.objects.base.MiniData;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.system.commands.*;
import org.minima.system.commands.backup.*;
import org.minima.utils.MiniFile;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class BackupOracle {
    static JSONObject run(JSONObject request)throws Exception {
        var out=new JSONObject();String op=(String)request.get("op");String folder=(String)request.get("folder");
        GeneralParams.DATA_FOLDER=folder;GeneralParams.BASE_FILE_FOLDER=folder;GeneralParams.USE_SQL_COINDB=false;GeneralParams.DB_IGNORE_LOCKS=false;
        if("stream".equals(op)){
            var calls=new JSONArray();var bytes=new ByteArrayOutputStream();int fail=((Number)request.get("fail")).intValue();
            var destination=new OutputStream(){public void write(int value)throws IOException{throw new AssertionError("unexpected scalar write");}
                public void write(byte[] data,int offset,int length)throws IOException{calls.add(length);if(calls.size()==fail)throw new IOException("fixture write failure");bytes.write(data,offset,length);}};
            try{var gzip=new java.util.zip.GZIPOutputStream(destination);for(Object part:(JSONArray)request.get("parts"))gzip.write(new MiniData((String)part).getBytes());gzip.finish();gzip.finish();if(Boolean.TRUE.equals(request.get("after")))gzip.write(new byte[0]);}
            catch(Throwable error){out.put("exception",error.toString());}
            out.put("calls",calls);out.put("data",new MiniData(bytes.toByteArray()).to0xString());return out;
        }
        if("decode".equals(op)){
            var command=new decryptbackup();command.getParams().put("file",request.get("file"));command.getParams().put("output",request.get("output"));command.getParams().put("password",request.get("password"));command.runCommand();
            out.put("data",new MiniData(MiniFile.readCompleteFile(new File((String)request.get("output")))).to0xString());return out;
        }
        var main=(Main)NIOClientOracle.U.allocateInstance(Main.class);NIOClientOracle.set(main,"mLock",new Object());NIOClientOracle.set(main,"mMessages",new LinkedList<>());NIOClientOracle.set(main,"mInitKeysCreated",!Boolean.FALSE.equals(request.get("keys")));NIOClientOracle.field(Main.class,"mMainInstance").set(null,main);
        MinimaDB.createDB();var db=MinimaDB.getDB();db.loadAllDB();
        try{
            if("seed".equals(op)){
                var sql=db.getTxPoWDB().getSQLDB();for(int i=0;i<10;i++)if(!sql.addTxPoW(ArchiveCacheOracle.txpow(i,0),i%3!=0))throw new RuntimeException("fixture insert failed");
                out.put("sql",sql.executeGenericSQL("UPDATE txpow SET timemilli=id*1000"));db.saveAllDB(false);out.put("seeded",true);return out;
            }
            if(request.containsKey("tip"))db.getTxPoWTree().setRoot(new TxPoWTreeNode(new TxBlock(ArchiveCacheOracle.txpow(12,0)),false));
            if(request.containsKey("payload"))db.getUserDB().getAllData().put("fixture_payload",request.get("payload"));
            if(request.containsKey("autoInitial"))db.getUserDB().setAutoBackup((Boolean)request.get("autoInitial"));
            Command command=new backup();
            if("meta".equals(op)){
                out.put("name",command.getName());out.put("help",command.getHelp());out.put("fullHelp",command.getFullHelp());var params=new JSONArray();params.addAll(command.getValidParams());out.put("params",params);
                command.getParams().put("file","fixture");out.put("freshParams",command.getFunction().getParams());var dates=new JSONArray();for(long time:new long[]{0,-1,1700000000000L})dates.add(backup.DATEFORMAT.format(new Date(time)));out.put("dates",dates);
            }else if("runner".equals(op))out.put("result",CommandRunner.getRunner().runMultiCommand(request.containsKey("caller")?(String)request.get("caller"):"0x00",(String)request.get("command")));
            else{command.getParams().putAll((JSONObject)request.get("params"));out.put("result",command.runCommand());}
        }catch(Throwable error){out.put("exception",error.toString());}
        out.put("auto",db.getUserDB().isAutoBackup());String lock=db.getRWLockInfo();out.put("lock",lock.substring(lock.indexOf('[')));db.saveSQL(false);return out;
    }
    public static void main(String[] ignored)throws Exception {
        Locale.setDefault(Locale.ENGLISH);org.h2.Driver.load();var input=new BufferedReader(new InputStreamReader(System.in));String line;var stdout=System.out;
        while((line=input.readLine())!=null){JSONObject result;System.setOut(System.err);try{result=run((JSONObject)new JSONParser().parse(line));}catch(Throwable error){result=new JSONObject();result.put("fixtureError",error.toString());}finally{System.setOut(stdout);}System.out.println(result.toJSONString());}
    }
}
