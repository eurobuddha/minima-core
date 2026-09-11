import java.io.*;
import org.minima.database.MinimaDB;
import org.minima.objects.*;
import org.minima.objects.mmr.MMRProof;
import org.minima.system.commands.backup.mysql;
import org.minima.system.Main;
import org.minima.system.commands.CommandRunner;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
public class MySQLCommandOracle extends SqlDBOracle {
    static JSONObject runCoins(JSONObject q)throws Exception{
        config=(JSONObject)q.get("sql");events=new JSONArray();calls=connectionID=preparedID=0;queryBatches.clear();
        var main=(Main)NIOClientOracle.U.allocateInstance(Main.class);NIOClientOracle.set(main,"mLock",new Object());NIOClientOracle.set(main,"mMessages",new java.util.LinkedList<>());NIOClientOracle.field(Main.class,"mMainInstance").set(null,main);
        GeneralParams.DATA_FOLDER=q.getString("folder");GeneralParams.BASE_FILE_FOLDER=q.getString("folder");GeneralParams.MYSQL_STORE_ALLTXPOW=Boolean.TRUE.equals(q.get("storeAll"));
        GeneralParams.IS_MOBILE=Boolean.TRUE.equals(q.get("mobile"));MinimaDB.createDB();
        var prefs=MinimaDB.getDB().getUserDB();prefs.getAllData().putAll((JSONObject)q.get("prefs"));
        var requested=config;var setup=new JSONObject();setup.put("rows",new JSONArray());setup.put("columns",new JSONArray());config=setup;MinimaDB.getDB().getArchive().loadDB(new File(q.getString("folder"),"archive"));config=requested;events=new JSONArray();calls=0;queryBatches.clear();
        var out=new JSONObject();var blocks=new JSONArray();
        for(int i=0;i<2;i++){
            var block=new TxBlock(ArchiveManagerOracle.txpow(i+10,0));
            block.getInputCoinProofs().add(new CoinProof(ObjectOracle.coin(i),new MMRProof()));
            block.getOutputCoins().add(ObjectOracle.coin(i+1));
            block.getOutputCoins().add(ObjectOracle.coin(i+2));
            blocks.add(ArchiveManagerOracle.wire(block));
        }
        out.put("blocks",blocks);
        if(q.getString("op").equals("bootstrap")){
            var chains=new JSONObject();var pows=new JSONObject();var raws=new JSONObject();
            for(String name:new String[]{"normal","gap","bad","zero"}){
                var wires=new JSONArray();var bufferRaw=new ByteArrayOutputStream();var dos=new DataOutputStream(bufferRaw);org.minima.objects.base.MiniByte.FALSE.writeDataStream(dos);new org.minima.objects.base.MiniNumber(3).writeDataStream(dos);var parent=org.minima.objects.base.MiniData.ZERO_TXPOWID;
                for(int i=0;i<3;i++){int number=name.equals("zero")?i:i+1;if(name.equals("gap")&&i>0)++number;var pow=ArchiveManagerOracle.txpow(number,0);if(!name.equals("bad")){pow.setSuperParent(0,parent);pow.calculateTXPOWID();}parent=pow.getTxPoWIDData();var block=new TxBlock(pow);var coin=new Coin(new org.minima.objects.base.MiniData("0x01"),new org.minima.objects.base.MiniData("0x01"),new org.minima.objects.base.MiniNumber(number),new org.minima.objects.base.MiniData("0x00"));var state=new java.util.ArrayList<StateVariable>();state.add(new StateVariable(0,"0xAABB"));coin.setState(state);block.getOutputCoins().add(coin);block.getInputCoinProofs().add(new CoinProof(coin,new MMRProof()));block.writeDataStream(dos);wires.add(ArchiveManagerOracle.wire(block));if(i==0)pows.put(name,ArchiveManagerOracle.wire(pow));}
                dos.flush();chains.put(name,wires);raws.put(name,new org.minima.objects.base.MiniData(bufferRaw.toByteArray()).to0xString());
            }out.put("chains",chains);out.put("pows",pows);out.put("raws",raws);
        }
        var command=new mysql();command.getParams().putAll((JSONObject)q.get("params"));
        var buffer=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(buffer,true,"UTF-8"));out.put("before",System.currentTimeMillis());
        try{
            String op=q.getString("op");Object value;
            if(op.equals("meta")){var meta=new JSONObject();meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("valid",command.getValidParams());meta.put("fresh",command.getFunction().getJSONReply());value=meta;}
            else if(op.equals("runner"))value=CommandRunner.getRunner().runMultiCommand(q.getString("caller"),q.getString("text"));
            else if(op.equals("bootstrap"))value=null;
            else if(op.equals("configure")){GeneralParams.MYSQL_DB_DELAY=integer(q,"delay",0);GeneralParams.MYSQL_DB_COINS=Boolean.TRUE.equals(q.get("coins"));mysql.convertMySQLParams((String)q.get("text"));value=null;}
            else if(op.equals("main")){var stdoutBuffer=new ByteArrayOutputStream();var stderrBuffer=new ByteArrayOutputStream();var previousOut=System.out;var previousError=System.err;try{System.setOut(new PrintStream(stdoutBuffer,true,"UTF-8"));System.setErr(new PrintStream(stderrBuffer,true,"UTF-8"));mysql.main(new String[0]);}finally{System.setOut(previousOut);System.setErr(previousError);}var output=new JSONObject();output.put("stdout",stdoutBuffer.toString("UTF-8"));output.put("stderr",stderrBuffer.toString("UTF-8"));value=output;}
            else value=command.runCommand();
            out.put("value",value);
        }catch(Exception error){out.put("exception",error.toString());}finally{System.setOut(previous);}
        out.put("importing",main.MYSQL_IMPORTING_NO_ACTION);out.put("storeAll",GeneralParams.MYSQL_STORE_ALLTXPOW);out.put("after",System.currentTimeMillis());var logs=new JSONArray();for(String line:buffer.toString("UTF-8").split("\n")){int at=line.indexOf("] : ");if(at>=0&&!line.substring(at+4).startsWith("     "))logs.add(line.substring(at+4));}
        out.put("events",events);out.put("logs",logs);out.put("prefs",new JSONObject(prefs.getAllData()));return out;
    }
    public static void main(String[] args)throws Exception{installDriver("jdbc:");var reader=new BufferedReader(new InputStreamReader(System.in));String line;while((line=reader.readLine())!=null)System.out.println(runCoins((JSONObject)JSONValue.parseWithException(line)).toJSONString());}
}
