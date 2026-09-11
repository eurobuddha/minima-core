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
        clear();request=input;events=new JSONArray();String folder=input.getString("folder"),op=(String)input.get("op");GeneralParams.DATA_FOLDER=folder;GeneralParams.BASE_FILE_FOLDER=folder;GeneralParams.USE_SQL_COINDB=false;GeneralParams.DB_IGNORE_LOCKS=false;GeneralParams.IS_MEGAMMR=Boolean.TRUE.equals(input.get("megaEnabled"));
        var main=(Main)NIOClientOracle.U.allocateInstance(Main.class);NIOClientOracle.set(main,"mLock",new Object());NIOClientOracle.set(main,"mMessages",new LinkedList<>());NIOClientOracle.set(main,"mInitKeysCreated",!Boolean.FALSE.equals(input.get("keys")));NIOClientOracle.field(Main.class,"mMainInstance").set(null,main);MinimaDB.createDB();var db=MinimaDB.getDB();db.loadAllDB();var saved=CommandRunner.ALL_COMMANDS.clone();for(int i=0;i<saved.length;i++)if(saved[i].getName().equals("archive"))CommandRunner.ALL_COMMANDS[i]=new Probe();
        var out=new JSONObject();try{
            if("mega".equals(op)){GeneralParams.P2P_ENABLED=false;var result=megaFixture(input,db);db.safeReleaseReadWriteLock();db.saveSQL(false);return result;}
            if("retimeGenesis".equals(op)){var tree=db.getTxPoWTree();var node=tree.getTip();if(node==null||node!=tree.getRoot()||!node.getBlockNumber().isEqual(MiniNumber.ONE)||db.getCascade().getTip()!=null)throw new RuntimeException("retime fixture requires genesis only");var pow=node.getTxPoW();var body=MiniData.getMiniDataVersion(pow.getTxBody());var mmr=MiniData.getMiniDataVersion(node.getMMR());out.put("oldID",pow.getTxPoWID());pow.setTimeMilli(new MiniNumber(input.getString("time")));pow.calculateTXPOWID();tree.recalculateTree();if(!body.isEqual(MiniData.getMiniDataVersion(pow.getTxBody()))||!mmr.isEqual(MiniData.getMiniDataVersion(node.getMMR()))||!pow.isBlock())throw new RuntimeException("retime fixture changed body/MMR or invalid block proof");out.put("id",pow.getTxPoWID());out.put("tree",MiniData.getMiniDataVersion(tree).to0xString());out.put("block",MiniData.getMiniDataVersion(node.getTxBlock()).to0xString());out.put("bodyUnchanged",true);out.put("mmrUnchanged",true);db.saveAllDB();return out;}
            if("seedTree".equals(op)){var wires=new JSONArray();var node=db.getTxPoWTree().getTip();while(node!=null){var block=node.getTxBlock();db.getArchive().saveBlock(block);wires.add(MiniData.getMiniDataVersion(block).to0xString());node=node.getParent();}db.getArchive().executeGenericSQL("UPDATE syncblock SET timemilli=block*1000");out.put("blocks",wires);db.saveAllDB();out.put("seeded",true);return out;}
            if("seed".equals(op)){var wires=new JSONArray();MiniData parent=MiniData.ZERO_TXPOWID;
                for(Object item:(JSONArray)input.get("heights")){int i=((Long)item).intValue();var pow=ArchiveCacheOracle.txpow(i,0);if(!input.containsKey("badParent")){pow.setSuperParent(0,parent);pow.calculateTXPOWID();}parent=pow.getTxPoWIDData();var block=new TxBlock(pow);
                    if(input.containsKey("coins")){var coin=new Coin(new MiniData(i%2!=0?"0x01":"0x02"),new MiniData("0x01"),new MiniNumber(i),new MiniData("0x00"),true);var states=new ArrayList<StateVariable>();states.add(new StateVariable(0,"0xAABB"));coin.setState(states);block.getOutputCoins().add(coin);block.getInputCoinProofs().add(new CoinProof(coin,new MMRProof()));}
                    db.getArchive().saveBlock(block);wires.add(MiniData.getMiniDataVersion(block).to0xString());
                }db.getArchive().executeGenericSQL("UPDATE syncblock SET timemilli=block*1000");out.put("blocks",wires);db.saveAllDB();out.put("seeded",true);return out;
            }
            if(input.containsKey("treeBlock")){var block=TxBlock.ReadFromStream(new DataInputStream(new ByteArrayInputStream(new MiniData(input.getString("treeBlock")).getBytes())));var node=new TxPoWTreeNode(new TxBlock(block.getTxPoW()),false);node.getTxBlock().getOutputCoins().addAll(block.getOutputCoins());node.getTxBlock().getInputCoinProofs().addAll(block.getInputCoinProofs());db.getTxPoWTree().setRoot(node);}
            if("encodeRequest".equals(op)){out.put("wire",MiniData.getMiniDataVersion(org.minima.system.network.minima.NIOManager.createNIOMessage(org.minima.system.network.minima.NIOMessage.MSG_ARCHIVE_REQ,new MiniNumber(input.getString("start")))).to0xString());db.saveSQL(false);return out;}
            if("encodeReply".equals(op)){var ibd=new IBD();if(input.containsKey("blocks"))for(Object wire:(JSONArray)input.get("blocks")){ibd.getTxBlocks().add(TxBlock.ReadFromStream(new DataInputStream(new ByteArrayInputStream(new MiniData((String)wire).getBytes()))));}out.put("ibd",MiniData.getMiniDataVersion(ibd).to0xString());out.put("wire",MiniData.getMiniDataVersion(org.minima.system.network.minima.NIOManager.createNIOMessage(new MiniByte(((Long)input.get("type")).intValue()),ibd)).to0xString());db.saveSQL(false);return out;}
            if("request".equals(op)){var nio=NIOClientOracle.install();if(input.containsKey("noMain"))NIOClientOracle.field(Main.class,"mMainInstance").set(null,null);else if(input.containsKey("noNetwork"))NIOClientOracle.set(Main.getInstance(),"mNetwork",null);else if(input.containsKey("noNIO"))NIOClientOracle.set(Main.getInstance().getNetworkManager(),"mNIOManager",null);var start=input.get("start")==null?null:new MiniNumber(input.getString("start"));String host=(String)input.get("host");int port=((Long)input.get("port")).intValue();org.minima.utils.Streamable ibd=input.containsKey("megaRequest")?org.minima.system.commands.backup.mmrsync.megammrsync.sendMegaMMRSyncReq(host,port,input.containsKey("nullData")?null:new org.minima.system.commands.backup.mmrsync.MegaMMRSyncData(megaList(input,"addresses"),megaList(input,"publickeys"))):input.containsKey("attempts")?archive.sendArchiveReq(host,port,start,((Long)input.get("attempts")).intValue()):archive.sendArchiveReq(host,port,start);out.put("ibd",ibd==null?null:MiniData.getMiniDataVersion(ibd).to0xString());out.put("traffic",nio.traffic.getBreakdown());out.put("read",nio.traffic.getTotalRead());out.put("write",nio.traffic.getTotalWrite());db.saveSQL(false);return out;}
            var command=new archive();
            if("meta".equals(op)){out.put("name",command.getName());out.put("help",command.getHelp());out.put("fullHelp",command.getFullHelp());var params=new JSONArray();params.addAll(command.getValidParams());out.put("params",params);command.getParams().put("file","fixture");out.put("fresh",command.getFunction().getParams());}
            else if("notify".equals(op)){org.minima.utils.messages.MessageListener listener=message->{var event=new JSONObject();event.put("type",message.getMessageType());event.put("notify",message.getObject("notify"));events.add(event);if(Boolean.TRUE.equals(input.get("listenerThrow")))throw new RuntimeException("fixture listener failure");};archive.NotifyListener(input.containsKey("nullListener")?null:listener,(String)input.get("message"));out.put("notified",true);}
            else if("runner".equals(op))out.put("result",CommandRunner.getRunner().runMultiCommand(input.containsKey("caller")?input.getString("caller"):"0x00",input.getString("command")));
            else{command.getParams().putAll((JSONObject)input.get("params"));out.put("result",command.runCommand());}
        }catch(Throwable error){out.put("exception",error.toString());}finally{for(int i=0;i<saved.length;i++)CommandRunner.ALL_COMMANDS[i]=saved[i];}
        out.put("events",events);out.put("static",state());out.put("archiveSize",db.getArchive().getSize());out.put("shutdown",main.isShuttingDown());out.put("restoring",main.isRestoring());db.saveSQL(false);return out;
    }
    static Coin megaCoin(JSONObject c,int index){
        var coin=new Coin(new MiniData(c.getString("id")),new MiniData(c.getString("address")),new MiniNumber(c.getString("amount")),MiniData.ZERO_TXPOWID,true);
        coin.setMMREntryNumber(new org.minima.objects.mmr.MMREntryNumber(index));coin.setBlockCreated(MiniNumber.ONE);coin.setSpent(Boolean.TRUE.equals(c.get("spent")));
        var states=new ArrayList<org.minima.objects.StateVariable>();if(c.containsKey("state"))states.add(new org.minima.objects.StateVariable(0,c.getString("state")));coin.setState(states);return coin;
    }
    static ArrayList<MiniData> megaList(JSONObject input,String key){if(input.get(key)==null)return null;var list=new ArrayList<MiniData>();for(Object value:(JSONArray)input.get(key))list.add(value==null?null:new MiniData((String)value));return list;}
    static JSONObject megaFixture(JSONObject input,MinimaDB db)throws Exception{
        if(input.getString("task").equals("saved")){
            var out=new JSONObject();
            var backup=new org.minima.system.commands.backup.mmrsync.MegaMMRBackup();org.minima.utils.MiniFile.loadObjectSlow(new File(input.getString("path")),backup);
            out.put("backupIBD",MiniData.getMiniDataVersion(backup.getIBD()).to0xString());
            var details=org.minima.system.commands.backup.mmrsync.megammrsync.getMyDetails();
            out.put("request",MiniData.getMiniDataVersion(org.minima.system.network.minima.NIOManager.createNIOMessage(org.minima.system.network.minima.NIOMessage.MSG_MEGAMMRSYNC_REQ,details)).to0xString());
            var current=org.minima.system.commands.backup.mmrsync.megammrsync.getCurrentMegaMMRIBD(details);
            out.put("currentIBD",MiniData.getMiniDataVersion(current.getIBD()).to0xString());
            var proofs=new JSONArray();for(var proof:current.getAllCoinProofs())proofs.add(MiniData.getMiniDataVersion(proof).to0xString());out.put("proofs",proofs);
            current.setPeersList(input.getString("peers"));
            out.put("wire",MiniData.getMiniDataVersion(org.minima.system.network.minima.NIOManager.createNIOMessage(org.minima.system.network.minima.NIOMessage.MSG_MEGAMMRSYNC_RESP,current)).to0xString());
            out.put("weight",org.minima.system.commands.backup.mmrsync.megammr.checkMegaMMR(new File(input.getString("path"))).toString());
            return out;
        }
        var out=new JSONObject();var mega=db.getMegaMMR();mega.clear();int index=0;
        for(Object value:(JSONArray)input.get("coins")){var coin=megaCoin((JSONObject)value,index++);mega.getMMR().addEntry(org.minima.objects.mmr.MMRData.CreateMMRDataLeafNode(coin,coin.getAmount()));mega.getAllCoins().put(coin.getCoinID().to0xString(),coin);}
        mega.getMMR().setBlockTime(new MiniNumber(input.getString("time")));
        TxPoWTreeNode first=null,last=null;int height=0;
        for(Object values:(JSONArray)input.get("tree")){var node=new TxPoWTreeNode(ArchiveCacheOracle.txpow(++height,0));if(first==null)first=node;else last.addChildNode(node);last=node;int at=0;for(Object value:(JSONArray)values)node.addCoinToNode(megaCoin((JSONObject)value,at++));}
        if(first!=null)db.getTxPoWTree().setRoot(first);
        var ibd=new IBD();int cascade=((Long)input.get("cascade")).intValue();if(cascade>=0){var casc=new org.minima.database.cascade.Cascade();for(int i=1;i<=cascade;i++)casc.addToTip(ArchiveCacheOracle.txpow(i,0));casc.cascadeChain();ibd.setCascade(casc);}
        for(Object number:(JSONArray)input.get("blocks"))ibd.getTxBlocks().add(new TxBlock(ArchiveCacheOracle.txpow(((Long)number).intValue(),0)));
        var backup=new org.minima.system.commands.backup.mmrsync.MegaMMRBackup(mega,ibd);
        if(Boolean.TRUE.equals(input.get("badCoin"))&&!mega.getAllCoins().isEmpty())mega.getAllCoins().values().iterator().next().resetCoinID(new MiniData("0xFFFF"));
        if(Boolean.TRUE.equals(input.get("walletRows"))){var wallet=db.getWallet();wallet.addScript("RETURN TRUE",false,false,"0x00",true);wallet.addScript("RETURN FALSE",true,true,"0x11",false);wallet.addScript("RETURN SIGNEDBY(0x22)",true,false,"0x22",true);wallet.executeGenericSQL("INSERT INTO keys(size,depth,uses,maxuses,modifier,privatekey,publickey) VALUES (2,1,0,2,'0x00','0x00','0x11'),(2,1,0,2,'0x00','0x00','0x22'),(2,1,0,2,'0x00','0x00','0x00')");}
        String task=input.getString("task");try{
            if(task.equals("check")){out.put("weight",org.minima.system.commands.backup.mmrsync.megammr.checkMegaMMR(backup).toString());if(Boolean.TRUE.equals(input.get("twice")))out.put("second",org.minima.system.commands.backup.mmrsync.megammr.checkMegaMMR(backup).toString());}
            else if(task.equals("encode")){var value=new org.minima.system.commands.backup.mmrsync.MegaMMRIBD(ibd,new ArrayList<CoinProof>());value.setPeersList(input.containsKey("peers")?input.getString("peers"):"");out.put("ibd",MiniData.getMiniDataVersion(value).to0xString());out.put("wire",MiniData.getMiniDataVersion(org.minima.system.network.minima.NIOManager.createNIOMessage(new MiniByte(((Long)input.get("type")).intValue()),value)).to0xString());}
            else if(task.equals("write")){File file=new File(input.getString("path"));org.minima.utils.MiniFile.writeObjectToFile(file,backup);out.put("wire",MiniData.getMiniDataVersion(backup).to0xString());}
            else if(task.equals("search")){var coins=org.minima.system.commands.backup.mmrsync.megammrsync.searchMegaCoins(megaList(input,"addresses"),megaList(input,"publickeys"));var rows=new JSONArray();for(var coin:coins)rows.add(coin.toJSON());out.put("coins",rows);if(!coins.isEmpty())coins.get(0).resetCoinID(new MiniData("0xEEEE"));}
            else if(task.equals("peers")){new org.minima.system.commands.backup.mmrsync.megammrsync().updateP2PDB((String)input.get("peers"));out.put("saved",true);}
            else if(task.equals("details")){out.put("wire",MiniData.getMiniDataVersion(org.minima.system.commands.backup.mmrsync.megammrsync.getMyDetails()).to0xString());}
            else if(task.equals("current")){out.put("wire",MiniData.getMiniDataVersion(org.minima.system.commands.backup.mmrsync.megammrsync.getCurrentMegaMMRIBD(new org.minima.system.commands.backup.mmrsync.MegaMMRSyncData(megaList(input,"addresses"),megaList(input,"publickeys")))).to0xString());}
            else {Command command=input.getString("name").equals("megammr")?new org.minima.system.commands.backup.mmrsync.megammr():new org.minima.system.commands.backup.mmrsync.megammrsync();if(task.equals("meta")){out.put("name",command.getName());out.put("help",command.getHelp());out.put("fullHelp",command.getFullHelp());var params=new JSONArray();params.addAll(command.getValidParams());out.put("params",params);command.getParams().put("file","held");out.put("fresh",command.getFunction().getParams());}else if(task.equals("runner"))out.put("result",CommandRunner.getRunner().runMultiCommand(input.getString("command")));else{command.getParams().putAll((JSONObject)input.get("params"));out.put("result",command.runCommand());}}
        }catch(Throwable error){out.put("exception",error.toString());}
        out.put("time",mega.getMMR().getBlockTime().toString());out.put("finalized",mega.getMMR().isFinalized());out.put("mega",MiniData.getMiniDataVersion(mega).to0xString());return out;
    }
    public static void main(String[] ignored)throws Exception{Locale.setDefault(Locale.ENGLISH);org.h2.Driver.load();var input=new BufferedReader(new InputStreamReader(System.in));String line;var stdout=System.out;while((line=input.readLine())!=null){JSONObject result;System.setOut(System.err);try{result=run((JSONObject)new JSONParser().parse(line));}catch(Throwable error){result=new JSONObject();result.put("fixtureError",error.toString());}finally{System.setOut(stdout);}stdout.println(result.toJSONString());}clear();}
}
