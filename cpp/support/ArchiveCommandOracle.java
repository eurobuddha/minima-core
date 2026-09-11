import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.archive.*;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.MMRProof;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.system.commands.*;
import org.minima.system.commands.backup.archive;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class ArchiveCommandOracle {
    static JSONObject request;static JSONArray events;
    static JSONObject state()throws Exception{var out=new JSONObject();for(String key:new String[]{"H2_TEMPARCHIVE","STATIC_TEMPARCHIVE","STATIC_RAW"}){var value=NIOClientOracle.field(archive.class,key).get(null);out.put(key.equals("H2_TEMPARCHIVE")?"h2":key.equals("STATIC_TEMPARCHIVE")?"temp":"raw",key.equals("H2_TEMPARCHIVE")?value:value!=null);}return out;}
    static void clear()throws Exception{var temp=(ArchiveManager)NIOClientOracle.field(archive.class,"STATIC_TEMPARCHIVE").get(null);if(temp!=null)temp.hardCloseDB();var raw=(RawArchiveInput)NIOClientOracle.field(archive.class,"STATIC_RAW").get(null);if(raw!=null)try{raw.stop();}catch(Exception ignored){}NIOClientOracle.field(archive.class,"STATIC_TEMPARCHIVE").set(null,null);NIOClientOracle.field(archive.class,"STATIC_RAW").set(null,null);NIOClientOracle.field(archive.class,"H2_TEMPARCHIVE").set(null,true);}
    static class Probe extends archive {
        public Command getFunction(){return new Probe();}
        public JSONObject runCommand()throws Exception{
            if(getParam("action","").equals("resync")&&Boolean.TRUE.equals(request.get("recordSync"))){var event=new JSONObject();event.put("command",getCompleteCommand());event.put("params",getParams());event.put("static",state());events.add(event);var out=getJSONReply();out.put("status",false);out.put("error","fixture sync declined");return out;}return super.runCommand();
        }
    }
    static JSONObject run(JSONObject input)throws Exception{
        clear();request=input;events=new JSONArray();String folder=input.getString("folder"),op=(String)input.get("op");GeneralParams.DATA_FOLDER=folder;GeneralParams.BASE_FILE_FOLDER=folder;GeneralParams.USE_SQL_COINDB=false;GeneralParams.DB_IGNORE_LOCKS=false;
        var main=(Main)NIOClientOracle.U.allocateInstance(Main.class);NIOClientOracle.set(main,"mLock",new Object());NIOClientOracle.set(main,"mMessages",new LinkedList<>());NIOClientOracle.set(main,"mInitKeysCreated",!Boolean.FALSE.equals(input.get("keys")));NIOClientOracle.field(Main.class,"mMainInstance").set(null,main);MinimaDB.createDB();var db=MinimaDB.getDB();db.loadAllDB();var saved=CommandRunner.ALL_COMMANDS.clone();for(int i=0;i<saved.length;i++)if(saved[i].getName().equals("archive"))CommandRunner.ALL_COMMANDS[i]=new Probe();
        var out=new JSONObject();try{
            if("seedTree".equals(op)){var wires=new JSONArray();var node=db.getTxPoWTree().getTip();while(node!=null){var block=node.getTxBlock();db.getArchive().saveBlock(block);wires.add(MiniData.getMiniDataVersion(block).to0xString());node=node.getParent();}db.getArchive().executeGenericSQL("UPDATE syncblock SET timemilli=block*1000");out.put("blocks",wires);db.saveAllDB();out.put("seeded",true);return out;}
            if("seed".equals(op)){var wires=new JSONArray();MiniData parent=MiniData.ZERO_TXPOWID;
                for(Object item:(JSONArray)input.get("heights")){int i=((Long)item).intValue();var pow=ArchiveCacheOracle.txpow(i,0);if(!input.containsKey("badParent")){pow.setSuperParent(0,parent);pow.calculateTXPOWID();}parent=pow.getTxPoWIDData();var block=new TxBlock(pow);
                    if(input.containsKey("coins")){var coin=new Coin(new MiniData(i%2!=0?"0x01":"0x02"),new MiniData("0x01"),new MiniNumber(i),new MiniData("0x00"),true);var states=new ArrayList<StateVariable>();states.add(new StateVariable(0,"0xAABB"));coin.setState(states);block.getOutputCoins().add(coin);block.getInputCoinProofs().add(new CoinProof(coin,new MMRProof()));}
                    db.getArchive().saveBlock(block);wires.add(MiniData.getMiniDataVersion(block).to0xString());
                }db.getArchive().executeGenericSQL("UPDATE syncblock SET timemilli=block*1000");out.put("blocks",wires);db.saveAllDB();out.put("seeded",true);return out;
            }
            if(input.containsKey("treeBlock")){var block=TxBlock.ReadFromStream(new DataInputStream(new ByteArrayInputStream(new MiniData(input.getString("treeBlock")).getBytes())));var node=new TxPoWTreeNode(new TxBlock(block.getTxPoW()),false);node.getTxBlock().getOutputCoins().addAll(block.getOutputCoins());node.getTxBlock().getInputCoinProofs().addAll(block.getInputCoinProofs());db.getTxPoWTree().setRoot(node);}
            if("encodeRequest".equals(op)){out.put("wire",MiniData.getMiniDataVersion(org.minima.system.network.minima.NIOManager.createNIOMessage(org.minima.system.network.minima.NIOMessage.MSG_ARCHIVE_REQ,new MiniNumber(input.getString("start")))).to0xString());db.saveSQL(false);return out;}
            if("encodeReply".equals(op)){var ibd=new IBD();out.put("ibd",MiniData.getMiniDataVersion(ibd).to0xString());out.put("wire",MiniData.getMiniDataVersion(org.minima.system.network.minima.NIOManager.createNIOMessage(new MiniByte(((Long)input.get("type")).intValue()),ibd)).to0xString());db.saveSQL(false);return out;}
            if("request".equals(op)){var nio=NIOClientOracle.install();if(input.containsKey("noMain"))NIOClientOracle.field(Main.class,"mMainInstance").set(null,null);else if(input.containsKey("noNetwork"))NIOClientOracle.set(Main.getInstance(),"mNetwork",null);else if(input.containsKey("noNIO"))NIOClientOracle.set(Main.getInstance().getNetworkManager(),"mNIOManager",null);var start=input.get("start")==null?null:new MiniNumber(input.getString("start"));String host=(String)input.get("host");int port=((Long)input.get("port")).intValue();var ibd=input.containsKey("attempts")?archive.sendArchiveReq(host,port,start,((Long)input.get("attempts")).intValue()):archive.sendArchiveReq(host,port,start);out.put("ibd",ibd==null?null:MiniData.getMiniDataVersion(ibd).to0xString());out.put("traffic",nio.traffic.getBreakdown());out.put("read",nio.traffic.getTotalRead());out.put("write",nio.traffic.getTotalWrite());db.saveSQL(false);return out;}
            var command=new archive();
            if("meta".equals(op)){out.put("name",command.getName());out.put("help",command.getHelp());out.put("fullHelp",command.getFullHelp());var params=new JSONArray();params.addAll(command.getValidParams());out.put("params",params);command.getParams().put("file","fixture");out.put("fresh",command.getFunction().getParams());}
            else if("notify".equals(op)){org.minima.utils.messages.MessageListener listener=message->{var event=new JSONObject();event.put("type",message.getMessageType());event.put("notify",message.getObject("notify"));events.add(event);if(Boolean.TRUE.equals(input.get("listenerThrow")))throw new RuntimeException("fixture listener failure");};archive.NotifyListener(input.containsKey("nullListener")?null:listener,(String)input.get("message"));out.put("notified",true);}
            else if("runner".equals(op))out.put("result",CommandRunner.getRunner().runMultiCommand(input.containsKey("caller")?input.getString("caller"):"0x00",input.getString("command")));
            else{command.getParams().putAll((JSONObject)input.get("params"));out.put("result",command.runCommand());}
        }catch(Throwable error){out.put("exception",error.toString());}finally{for(int i=0;i<saved.length;i++)CommandRunner.ALL_COMMANDS[i]=saved[i];}
        out.put("events",events);out.put("static",state());out.put("archiveSize",db.getArchive().getSize());out.put("shutdown",main.isShuttingDown());out.put("restoring",main.isRestoring());db.saveSQL(false);return out;
    }
    public static void main(String[] ignored)throws Exception{Locale.setDefault(Locale.ENGLISH);org.h2.Driver.load();var input=new BufferedReader(new InputStreamReader(System.in));String line;var stdout=System.out;while((line=input.readLine())!=null){JSONObject result;System.setOut(System.err);try{result=run((JSONObject)new JSONParser().parse(line));}catch(Throwable error){result=new JSONObject();result.put("fixtureError",error.toString());}finally{System.setOut(stdout);}stdout.println(result.toJSONString());}clear();}
}
