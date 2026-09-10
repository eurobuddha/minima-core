import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.system.commands.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Actual commands, MinimaDB and Wallet; only JDBC calls use the recording driver. */
public class WalletCommandsOracle extends SqlDBOracle {
    static sun.misc.Unsafe unsafe()throws Exception {var f=sun.misc.Unsafe.class.getDeclaredField("theUnsafe");f.setAccessible(true);return (sun.misc.Unsafe)f.get(null);}
    static void setField(Class<?> type,Object target,String name,Object value)throws Exception {var f=type.getDeclaredField(name);f.setAccessible(true);f.set(target,value);}
    static boolean vaultEnabled,vaultShutdown,vaultStopped;
    static JSONArray vaultCalls=new JSONArray();
    static String vaultRoot;
    static void vaultCalled(String name){vaultCalls.add(name);if(name.equals(config.get("vaultFail")))throw new RuntimeException("fixture Main failure");}
    static JSONObject vaultSnapshot()throws Exception{
        var wallet=MinimaDB.getDB().getWallet();var base=wallet.getBaseSeed();var value=new JSONObject();value.put("base",base==null?null:base.toJSON());value.put("calls",vaultCalls);
        var field=wallet.getClass().getDeclaredField("STOP_NEWKEYS");field.setAccessible(true);value.put("stopNewKeys",field.getBoolean(wallet));value.put("shutdown",vaultShutdown);value.put("stopped",vaultStopped);
        var encrypted=MinimaDB.getDB().getUserDB().getEncryptedSeed();value.put("encrypted",encrypted.to0xString());
        if(!encrypted.isEqual(org.minima.objects.base.MiniData.ZERO_TXPOWID))try{String password=config.containsKey("observePassword")?config.getString("observePassword"):"fixture pass";value.put("decrypted",new org.minima.objects.base.MiniString(org.minima.utils.encrypt.PasswordCrypto.decryptPassword(password,encrypted).getBytes()).toString());}catch(Exception e){value.put("decrypted","DECRYPT_ERROR");}
        var files=new JSONArray();for(String name:new String[]{"archive.mv.db","cascade.db","chaintree.db","p2p.db"})files.add(new File(vaultRoot+"/databases/"+name).exists());value.put("files",files);return value;
    }
    static void vaultStores()throws Exception{
        var folder=MinimaDB.getDB().getBaseDBFolder();MinimaDB.getDB().getTxPoWDB().getSQLDB().loadDB(new File(folder,"txpow"));MinimaDB.getDB().getArchive().loadDB(new File(folder,"archive"));
        folder.mkdirs();for(String name:new String[]{"archive.mv.db","cascade.db","chaintree.db","p2p.db"})java.nio.file.Files.writeString(new File(folder,name).toPath(),"fixture");
    }
    static JSONArray forwarded=new JSONArray(),historyCalls=new JSONArray();
    static class RecordingHistory extends org.minima.system.commands.search.history {
        public JSONObject getTxnDetails(org.minima.objects.TxPoW txpow){
            if(config.containsKey("detailBlock")){txpow.setBlockNumber(new org.minima.objects.base.MiniNumber(config.getString("detailBlock")));var change=new JSONObject();change.put("id",txpow.getTxPoWID());change.put("block",txpow.getBlockNumber().toString());historyCalls.add(change);}
            else historyCalls.add(txpow.getTxPoWID());
            if(historyCalls.size()==integer(config,"detailThrow",0))throw new RuntimeException("fixture detail failure");
            if(config.containsKey("detailResults"))return (JSONObject)((JSONArray)config.get("detailResults")).get(historyCalls.size()-1);
            return super.getTxnDetails(txpow);
        }
    }
    static class RecordingSend extends org.minima.system.commands.send.send {
        public org.minima.system.commands.Command getFunction(){return new RecordingSend();}
        public JSONObject runCommand()throws Exception{
            var call=new JSONObject();call.put("text",getCompleteCommand());call.put("params",getParams());forwarded.add(call);
            if(Boolean.TRUE.equals(config.get("sendThrow")))throw new RuntimeException("fixture send failure");
            if(config.containsKey("sendResult"))return (JSONObject)config.get("sendResult");
            var result=getJSONReply();var response=new JSONObject();response.put("forwarded",true);result.put("response",response);return result;
        }
    }
    static class SendRecordingScope implements AutoCloseable {
        int index=-1;org.minima.system.commands.Command saved;
        SendRecordingScope(boolean enabled){if(enabled)for(int i=0;i<CommandRunner.ALL_COMMANDS.length;i++)if("send".equals(CommandRunner.ALL_COMMANDS[i].getName())){index=i;saved=CommandRunner.ALL_COMMANDS[i];CommandRunner.ALL_COMMANDS[i]=new RecordingSend();break;}}
        public void close(){if(index>=0)CommandRunner.ALL_COMMANDS[index]=saved;}
    }
    static class RecordingMysql extends Command {
        RecordingMysql(){super("mysql","fixture MySQL delegation");}
        public Command getFunction(){return new RecordingMysql();}
        public ArrayList<String> getValidParams(){return new ArrayList<>(Arrays.asList("action","txpowid"));}
        public JSONObject runCommand()throws Exception{
            var call=new JSONObject();call.put("text",getCompleteCommand());call.put("params",getParams());forwarded.add(call);
            if(Boolean.TRUE.equals(config.get("mysqlThrow")))throw new RuntimeException("fixture mysql failure");
            return (JSONObject)config.get("mysqlResult");
        }
    }
    static class MysqlRecordingScope implements AutoCloseable {
        int index=-1;Command saved;
        MysqlRecordingScope(boolean enabled){if(enabled)for(int i=0;i<CommandRunner.ALL_COMMANDS.length;i++)if("mysql".equals(CommandRunner.ALL_COMMANDS[i].getName())){index=i;saved=CommandRunner.ALL_COMMANDS[i];CommandRunner.ALL_COMMANDS[i]=new RecordingMysql();break;}}
        public void close(){if(index>=0)CommandRunner.ALL_COMMANDS[index]=saved;}
    }
    static MiningMiner miningMiner;
    static ArrayList<org.minima.utils.messages.Message> miningPosted=new ArrayList<>();
    static ArrayList<org.minima.objects.TxPoW> miningHeld=new ArrayList<>();
    static JSONArray miningCalls=new JSONArray();
    static class MiningMain extends org.minima.system.Main {
        MiningMain()throws Exception{super();}
        public boolean getAllKeysCreated(){vaultCalled("getAllKeysCreated");return Boolean.TRUE.equals(config.get("keysCreated"));}
        public int getAllDefaultKeysSize(){vaultCalled("getAllDefaultKeysSize");return super.getAllDefaultKeysSize();}
        public void restoreReady(){vaultCalled("restoreReady");}
        public void setHasShutDown(){vaultCalled("setHasShutDown");super.setHasShutDown();vaultShutdown=true;}
        public void stopMessageProcessor(){vaultCalled("stopMessageProcessor");vaultStopped=true;}
        public void PostMessage(org.minima.utils.messages.Message message){miningPosted.add(message);if(Boolean.TRUE.equals(config.get("failPost")))throw new RuntimeException("fixture post failure");}
        public void PostNotifyEvent(String name,JSONObject data){}
    }
    static class MiningMiner extends org.minima.system.brains.TxPoWMiner {
        public void mineTxPoWAsync(org.minima.objects.TxPoW pow){miningEnter("async",pow,false,0,true);super.mineTxPoWAsync(pow);}
        public boolean MineMaxTxPoW(boolean maxima,org.minima.objects.TxPoW pow,long limit,boolean post){
            miningEnter("sync",pow,maxima,limit,post);String mode=config.containsKey("mineResult")?config.getString("mineResult"):"real";
            if(mode.equals("false"))return false;if(mode.equals("throw"))throw new RuntimeException("fixture mining failure");
            return super.MineMaxTxPoW(maxima,pow,limit,post);
        }
        void processNext()throws Exception{super.processMessage(getNextMessage());}
    }
    static void miningEnter(String kind,org.minima.objects.TxPoW pow,boolean maxima,long limit,boolean post){
        pow.getTxBody().mPRNG=new org.minima.objects.base.MiniData("0x"+"12".repeat(32));
        var row=MinimaDB.getDB().getCustomTxnDB().getTransactionRow("test");var call=new JSONObject();
        call.put("kind",kind);call.put("maxima",maxima);call.put("limit",limit);call.put("post",post);
        call.put("wire",org.minima.objects.base.MiniData.getMiniDataVersion(pow).to0xString());
        call.put("sameTransaction",row!=null&&row.getTransaction()==pow.getTransaction());call.put("sameWitness",row!=null&&row.getWitness()==pow.getWitness());
        miningCalls.add(call);miningHeld.add(pow);
    }
    static void miningInit()throws Exception{
        var owner=(MiningMain)unsafe().allocateInstance(MiningMain.class);setField(org.minima.system.Main.class,null,"mMainInstance",owner);
        if(Boolean.TRUE.equals(config.get("nullMiner"))){miningMiner=null;setField(org.minima.system.Main.class,owner,"mTxPoWMiner",null);return;}
        miningMiner=(MiningMiner)unsafe().allocateInstance(MiningMiner.class);
        setField(org.minima.utils.messages.MessageStack.class,miningMiner,"mLock",new Object());
        setField(org.minima.utils.messages.MessageStack.class,miningMiner,"mMessages",new LinkedList<org.minima.utils.messages.Message>());
        setField(org.minima.utils.messages.MessageProcessor.class,miningMiner,"mRunning",true);
        setField(org.minima.system.brains.TxPoWMiner.class,miningMiner,"mMiningCoins",new ArrayList<String>());
        setField(org.minima.system.Main.class,owner,"mTxPoWMiner",miningMiner);
    }
    static JSONObject miningDescribe(org.minima.utils.messages.Message message){
        var value=new JSONObject();value.put("type",message.getMessageType());
        for(String key:new String[]{"automine","starting"})if(message.exists(key))value.put(key,message.getBoolean(key));
        if(message.exists("keyuses"))value.put("keyuses",message.getInteger("keyuses"));
        var pow=(org.minima.objects.TxPoW)message.getObject("txpow");value.put("wire",pow==null?null:org.minima.objects.base.MiniData.getMiniDataVersion(pow).to0xString());return value;
    }
    static JSONObject miningSnapshot()throws Exception{
        var value=new JSONObject();value.put("calls",miningCalls);var posted=new JSONArray();var pending=new JSONArray();var held=new JSONArray();var coins=new JSONArray();
        for(var message:miningPosted)posted.add(miningDescribe(message));
        if(miningMiner!=null){var field=org.minima.utils.messages.MessageStack.class.getDeclaredField("mMessages");field.setAccessible(true);for(var message:(LinkedList<org.minima.utils.messages.Message>)field.get(miningMiner))pending.add(miningDescribe(message));}
        for(var pow:miningHeld)held.add(org.minima.objects.base.MiniData.getMiniDataVersion(pow).to0xString());
        if(miningMiner!=null)for(var pow:miningHeld)for(var txn:new org.minima.objects.Transaction[]{pow.getTransaction(),pow.getBurnTransaction()})for(var coin:txn.getAllInputs())coins.add(miningMiner.checkForMiningCoin(coin.getCoinID().to0xString()));
        value.put("posted",posted);value.put("pending",pending);value.put("held",held);value.put("coins",coins);return value;
    }
    static void createOwner()throws Exception{
        MinimaDB.createDB();var field=MinimaDB.class.getDeclaredField("mTxnDB");field.setAccessible(true);field.set(MinimaDB.getDB(),new org.minima.database.userprefs.txndb.TxnDB());
    }
    static JSONObject unsignedFileObservation(byte[] data){
        var result=new JSONObject();
        try{
            var stream=new ByteArrayInputStream(data);var input=new DataInputStream(stream);var header=new org.minima.objects.TxHeader();header.readDataStream(input);
            if(org.minima.objects.base.MiniByte.ReadFromStream(input).isTrue()){result.put("prngLength",input.readInt());result.put("prngOffset",data.length-stream.available());}
            stream=new ByteArrayInputStream(data);var pow=org.minima.objects.TxPoW.ReadFromStream(new DataInputStream(stream));
            result.put("consumed",data.length-stream.available());result.put("json",pow.toJSON());
        }catch(Exception error){result.put("error",error.toString());}
        return result;
    }
    static String signedFile(File root){String result="";for(File file:root.listFiles())if(file.getName().startsWith("signedtransaction-"))result=file.getName();return result;}
    static JSONObject runCommands(JSONObject input,String root)throws Exception{
        if(input.containsKey("nodeState"))java.nio.file.Files.deleteIfExists(new File(root,"control-prefs").toPath());
        boolean offline=input.containsKey("offline");File offlineRoot=new File(root,"offline");GeneralParams.BASE_FILE_FOLDER=offline?offlineRoot.getPath():"";
        if(offline){if(offlineRoot.exists())try(var paths=java.nio.file.Files.walk(offlineRoot.toPath())){for(var path:paths.sorted(Comparator.reverseOrder()).toList())java.nio.file.Files.delete(path);}offlineRoot.mkdirs();}

        org.minima.system.commands.txn.txnaddamount.enableCoinLock(false);org.minima.database.txpowdb.sql.TxPoWSqlDB.MAX_RELEVANT_TXPOW=new org.minima.objects.base.MiniNumber(100);GeneralParams.NUMBER_DAYS_SQLTXPOWDB=3;
        vaultEnabled=Boolean.TRUE.equals(input.get("vault"));vaultCalls=new JSONArray();vaultShutdown=vaultStopped=false;vaultRoot=root;org.minima.database.wallet.Wallet.NUMBER_GETADDRESS_KEYS=64;
        if(vaultEnabled)for(String name:new String[]{"archive.mv.db","cascade.db","chaintree.db","p2p.db"})new File(root+"/databases/"+name).delete();
        miningMiner=null;miningPosted.clear();miningHeld.clear();miningCalls=new JSONArray();setField(org.minima.system.Main.class,null,"mMainInstance",null);GeneralParams.USE_SQL_COINDB=false;GeneralParams.IS_MEGAMMR=false;connectionID=preparedID=0;org.minima.system.params.GlobalParams.MINIMA_MMR_PROOF_HISTORY=new org.minima.objects.base.MiniNumber(256);GeneralParams.DATA_FOLDER=root;var prefs=new File(root+"/databases/userprefs.db");java.nio.file.Files.deleteIfExists(prefs.toPath());createOwner();var wallet=MinimaDB.getDB().getWallet();JSONArray outputs=new JSONArray();org.minima.objects.TxPoW held=null;org.minima.objects.TxBody body=null;
        for(Object entry:(JSONArray)input.get("ops")){
            config=(JSONObject)entry;historyCalls=new JSONArray();forwarded=new JSONArray();events=new JSONArray();calls=0;miningCalls=new JSONArray();vaultCalls=new JSONArray();JSONObject out=new JSONObject();long before=System.currentTimeMillis();
            var bytes=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(bytes,true,"UTF-8"));
            try(var recording=new SendRecordingScope(Boolean.TRUE.equals(config.get("recordSend")));var mysqlRecording=new MysqlRecordingScope(Boolean.TRUE.equals(config.get("recordMysql")))){
                String op=config.getString("op");Object result=null;
                if(op.equals("filePut")){var file=new File(offlineRoot,config.getString("file"));file.getParentFile().mkdirs();java.nio.file.Files.write(file.toPath(),new org.minima.objects.base.MiniData(config.getString("data")).getBytes());}
                else if(op.equals("signDemo")){
                    org.minima.system.commands.send.sendsign.main(Boolean.TRUE.equals(config.get("nullArgs"))?null:new String[0]);result=bytes.toString("UTF-8");
                }
                else if(op.equals("fileWrite"))org.minima.utils.MiniFile.writeDataToFile(org.minima.utils.MiniFile.createBaseFile(config.getString("file")),new org.minima.objects.base.MiniData(config.getString("data")).getBytes(),Boolean.TRUE.equals(config.get("append")));
                else if(op.equals("fileBase"))GeneralParams.BASE_FILE_FOLDER=config.getString("folder");
                else if(op.equals("fileDirectory"))new File(offlineRoot,config.getString("file")).mkdirs();
                else if(op.equals("verifyFile")){
                    var fileBytes=org.minima.utils.MiniFile.readCompleteFile(new File(offlineRoot,signedFile(offlineRoot)));var pow=org.minima.objects.TxPoW.convertMiniDataVersion(new org.minima.objects.base.MiniData(fileBytes));var value=new JSONObject();
                    for(boolean burn:new boolean[]{false,true}){var valid=new JSONArray();var txn=burn?pow.getBurnTransaction():pow.getTransaction();var witness=burn?pow.getBurnWitness():pow.getWitness();for(var signature:witness.getAllSignatures()){var key=new org.minima.objects.keys.TreeKey();key.setPublicKey(signature.getRootPublicKey());valid.add(key.verify(txn.getTransactionID(),signature));}value.put(burn?"burn":"main",valid);}result=value;
                }
                else if(op.equals("vaultMain"))miningInit();
                else if(op.equals("vaultStores"))vaultStores();
                else if(op.equals("vaultTarget"))org.minima.database.wallet.Wallet.NUMBER_GETADDRESS_KEYS=((Number)config.get("target")).intValue();
                else if(op.equals("vaultSeed"))wallet.updateSeedRow((String)config.get("phrase"),(String)config.get("seed"));
                else if(op.equals("vaultEncrypted"))MinimaDB.getDB().getUserDB().setEncryptedSeed(new org.minima.objects.base.MiniData(config.getString("data")));
                else if(op.equals("vaultDecrypt"))result=new org.minima.objects.base.MiniString(org.minima.utils.encrypt.PasswordCrypto.decryptPassword(config.getString("password"),new org.minima.objects.base.MiniData(config.getString("data"))).getBytes()).toString();
                else if(op.equals("vaultLock"))org.minima.system.commands.backup.vault.passwordLockDB((String)config.get("password"));
                else if(op.equals("vaultUnlock"))org.minima.system.commands.backup.vault.passowrdUnlockDB((String)config.get("password"));
                else if(op.equals("keysCheck"))result=org.minima.system.commands.search.keys.checkKey((String)config.get("publickey"));
                else if(op.equals("vaultCheck"))org.minima.system.commands.backup.vault.checkAllKeysCreated();
                else if(op.equals("vaultStop"))org.minima.system.commands.backup.vault.stopAllKeysCreated();
                else if(op.equals("treeIndex")){var node=new org.minima.objects.keys.TreeKeyNode(new org.minima.objects.base.MiniData("0x12"),2);int index=((Number)config.get("index")).intValue();result=(config.getString("method").equals("child")?node.getChild(index).getPublicKey():node.getWOTSKey(index).getPublicKey()).to0xString();}
                else if(op.equals("verifyStored")){var valid=new JSONArray();var row=MinimaDB.getDB().getCustomTxnDB().getTransactionRow("test");for(var signature:row.getWitness().getAllSignatures()){var key=new org.minima.objects.keys.TreeKey();key.setPublicKey(signature.getRootPublicKey());valid.add(key.verify(row.getTransaction().getTransactionID(),signature));}result=valid;}
                else if(op.equals("addNullSignature"))MinimaDB.getDB().getCustomTxnDB().getTransactionRow("test").getWitness().addSignature(null);
                else if(op.equals("signedBy"))result=MinimaDB.getDB().getCustomTxnDB().getTransactionRow("test").getWitness().isSignedBy((String)config.get("publickey"));
                else if(op.equals("selection")){
                    var coins=new ArrayList<org.minima.objects.Coin>();
                    for(Object item:(JSONArray)config.get("coins")){
                        var spec=(JSONObject)item;
                        if(spec.containsKey("alias"))coins.add(coins.get(((Number)spec.get("alias")).intValue()));
                        else coins.add(new org.minima.objects.Coin(new org.minima.objects.base.MiniData(spec.getString("id")),new org.minima.objects.base.MiniData(spec.getString("address")),new org.minima.objects.base.MiniNumber(spec.getString("amount")),org.minima.objects.Token.TOKENID_MINIMA));
                    }
                    var value=new JSONObject();var selected=new ArrayList<org.minima.objects.Coin>();
                    try{selected=Boolean.TRUE.equals(config.get("order"))?org.minima.system.commands.send.send.orderCoins(coins):org.minima.system.commands.send.send.selectCoins(coins,new org.minima.objects.base.MiniNumber(config.getString("amount")),Boolean.TRUE.equals(config.get("debug")));}
                    catch(Exception error){value.put("error",error.toString());}
                    if(Boolean.TRUE.equals(config.get("mutate"))&&!selected.isEmpty())selected.get(0).setSpent(true);
                    var sorted=new JSONArray();var returned=new JSONArray();for(var coin:coins)sorted.add(coin.toJSON());for(var coin:selected)returned.add(coin.toJSON());
                    value.put("sorted",sorted);value.put("selected",returned);result=value;
                }
                else if(op.equals("coinLock")){
                    var action=config.getString("action");
                    if(action.equals("enable"))org.minima.system.commands.txn.txnaddamount.enableCoinLock(Boolean.TRUE.equals(config.get("enabled")));
                    else if(action.equals("add"))org.minima.system.commands.txn.txnaddamount.addCoinLock(config.getString("id"));
                    else if(action.equals("clear"))org.minima.system.commands.txn.txnaddamount.clearCoinLocked();
                    else result=org.minima.system.commands.txn.txnaddamount.isCoinLocked(config.getString("id"));
                }
                else if(op.equals("verifyMined")){
                    var valid=new JSONArray();for(var pow:miningHeld)for(var signature:pow.getWitness().getAllSignatures()){
                        var key=new org.minima.objects.keys.TreeKey();key.setPublicKey(signature.getRootPublicKey());valid.add(key.verify(pow.getTransaction().getTransactionID(),signature));
                    }result=valid;
                }
                else if(op.equals("mining"))miningInit();
                else if(op.equals("mineNext"))miningMiner.processNext();
                else if(op.equals("mineState")){}
                else if(op.equals("postHeld")){var command=CommandRunner.getRunner().getCommandOnly("txnminepost");command.getParams().put("data",org.minima.objects.base.MiniData.getMiniDataVersion(miningHeld.get(miningHeld.size()-1)).to0xString());result=command.runCommand();}
                else if(op.equals("limitSize"))MinimaDB.getDB().getTxPoWTree().getTip().getTxPoW().getMagic().mCurrentMaxTxPoWSize=new org.minima.objects.base.MiniNumber(config.getString("size"));

                else if(op.equals("coinStore"))org.minima.database.txpowtree.CoinDB.createCoinDB(new File(root,"control-coins"));
                else if(op.equals("coinSQL"))GeneralParams.USE_SQL_COINDB=Boolean.TRUE.equals(config.get("enabled"));
                else if(op.equals("relevantEntries")){
                    var node=MinimaDB.getDB().getTxPoWTree().getTip().getParent(integer(config,"depth",0));var entries=node.getRelevantCoinsEntries();entries.clear();
                    for(Object value:(JSONArray)config.get("entries"))entries.add(new org.minima.objects.mmr.MMREntryNumber(new java.math.BigInteger((String)value)));if(Boolean.TRUE.equals(config.get("recalculate")))node.calculateRelevantCoins();
                }
                else if(op.equals("poolEntries")){
                    var pool=MinimaDB.getDB().getTxPoWDB().getCompleteMemPool();if(Boolean.TRUE.equals(config.get("clear")))pool.clear();
                    for(Object value:(JSONArray)config.get("entries")){
                        var row=(JSONObject)value;org.minima.objects.TxPoW pow=null;
                        if(Boolean.TRUE.equals(row.get("nullTxPoW"))){}
                        else if(row.containsKey("wire"))pow=org.minima.objects.TxPoW.convertMiniDataVersion(new org.minima.objects.base.MiniData(row.getString("wire")));
                        else pow=new org.minima.objects.TxPoW(row.getString("id"),0,1,(Boolean)row.get("block"),"0x00",(Boolean)row.get("transaction"));
                        var ram=new org.minima.database.txpowdb.ram.RamData(pow);ram.setInCascade((Boolean)row.get("cascade"));ram.setOnMainChain((Boolean)row.get("onchain"));pool.put(row.getString("key"),ram);
                    }
                    if(config.containsKey("remove"))for(Object key:(JSONArray)config.get("remove"))pool.remove((String)key);
                }
                else if(op.equals("preferencePatch"))MinimaDB.getDB().getUserDB().getAllData().putAll((JSONObject)config.get("values"));
                else if(op.equals("preferenceSave"))MinimaDB.getDB().getUserDB().saveDB(new File(root,"control-prefs"));
                else if(op.equals("preferenceLoad"))MinimaDB.getDB().getUserDB().loadDB(new File(root,"control-prefs"));
                else if(op.equals("generatedMagic"))result=org.minima.system.brains.TxPoWGenerator.generateTxPoW(new org.minima.objects.Transaction(),new org.minima.objects.Witness()).getMagic().toJSON();
                else if(op.equals("txpowStores")){
                    MinimaDB.getDB().getTxPoWDB().loadSQLDB(new File(root,"txpow"));MinimaDB.getDB().getArchive().loadDB(new File(root,"archive"));
                }
                else if(op.equals("txpowPrefs")){MinimaDB.getDB().getUserDB().setAutoLoginDetailsMySQL(Boolean.TRUE.equals(config.get("login")));GeneralParams.NUMBER_DAYS_SQLTXPOWDB=config.containsKey("days")?((Number)config.get("days")).longValue():3;}
                else if(op.equals("txpowRAM")){
                    var value=org.minima.objects.TxPoW.convertMiniDataVersion(new org.minima.objects.base.MiniData(config.getString("data")));MinimaDB.getDB().getTxPoWDB().getCompleteMemPool().put(value.getTxPoWID(),new org.minima.database.txpowdb.ram.RamData(value));result=value.getTxPoWID();
                }
                else if(op.equals("historyLoad"))MinimaDB.getDB().getTxPoWDB().getSQLDB().loadDB(new File(root,"txpow"));
                else if(op.equals("historyLimit"))org.minima.database.txpowdb.sql.TxPoWSqlDB.MAX_RELEVANT_TXPOW=new org.minima.objects.base.MiniNumber(config.getString("limit"));
                else if(op.equals("tokenImports")){
                    var field=org.minima.system.brains.TxPoWSearcher.class.getDeclaredField("mImportedTokens");field.setAccessible(true);((ArrayList<?>)field.get(null)).clear();
                    for(Object data:(JSONArray)config.get("tokens"))org.minima.system.brains.TxPoWSearcher.importToken(data==null?null:org.minima.objects.Token.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)data)));
                }
                else if(op.equals("scanDetails")){
                    org.minima.objects.TxPoW pow=null;
                    if(!Boolean.TRUE.equals(config.get("nullTxPoW"))){
                        pow=org.minima.objects.TxPoW.convertMiniDataVersion(new org.minima.objects.base.MiniData(config.getString("data")));
                        if(Boolean.TRUE.equals(config.get("bodyless")))pow.clearBody();
                        if(Boolean.TRUE.equals(config.get("nullTransaction")))pow.getTxBody().mTransaction=null;
                        if(Boolean.TRUE.equals(config.get("nullBurn")))pow.getTxBody().mBurnTransaction=null;
                    }
                    result=new org.minima.system.commands.base.scanchain().getTransactionDetails(pow);
                }
                else if(op.equals("historyDetails")){
                    org.minima.objects.TxPoW pow=null;
                    if(!Boolean.TRUE.equals(config.get("nullTxPoW"))){
                        pow=org.minima.objects.TxPoW.convertMiniDataVersion(new org.minima.objects.base.MiniData(config.getString("data")));
                        if(Boolean.TRUE.equals(config.get("bodyless")))pow.clearBody();
                        if(Boolean.TRUE.equals(config.get("nullTransaction")))pow.getTxBody().mTransaction=null;
                    }
                    result=new org.minima.system.commands.search.history().getTxnDetails(pow);
                }
                else if(op.equals("historyOverride")){
                    var command=new RecordingHistory();command.getParams().putAll((JSONObject)config.get("params"));result=command.runCommand();
                }
                else if(op.equals("numberRange")){
                    result=(Boolean.TRUE.equals(config.get("decimal"))?new org.minima.objects.base.MiniNumber(new java.math.BigDecimal(new java.math.BigInteger(config.getString("coefficient")),((Number)config.get("scale")).intValue())):new org.minima.objects.base.MiniNumber(config.getString("text"))).toString();
                }
                else if(op.equals("load"))wallet.loadDB(new File(root+"/wallet"));
                else if(op.equals("chain")){
                    MinimaDB.getDB().resetCascadeAndTxPoWTree();
                    org.minima.system.params.GlobalParams.MINIMA_MMR_PROOF_HISTORY=new org.minima.objects.base.MiniNumber(config.getString("history"));
                    org.minima.database.txpowtree.TxPoWTreeNode first=null,previousNode=null;
                    for(Object n:(JSONArray)config.get("nodes")){
                        var spec=(JSONObject)n;var node=new org.minima.database.txpowtree.TxPoWTreeNode(org.minima.objects.TxPoW.convertMiniDataVersion(new org.minima.objects.base.MiniData(spec.getString("pow"))));
                        node.getMMR().readDataStream(new DataInputStream(new ByteArrayInputStream(new org.minima.objects.base.MiniData(spec.getString("mmr")).getBytes())));
                        if(spec.containsKey("coins"))for(Object c:(JSONArray)spec.get("coins"))node.addCoinToNode(org.minima.objects.Coin.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)c)));
                        if(spec.containsKey("relevant"))for(Object c:(JSONArray)spec.get("relevant"))node.getRelevantCoins().add(org.minima.objects.Coin.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)c)));
                        if(previousNode!=null)previousNode.addChildNode(node);else first=node;previousNode=node;
                    }
                    if(first!=null)MinimaDB.getDB().getTxPoWTree().setRoot(first);
                    if(config.containsKey("mega"))GeneralParams.IS_MEGAMMR=Boolean.TRUE.equals(config.get("mega"));
                    if(config.containsKey("megaCoins"))for(Object data:(JSONArray)config.get("megaCoins")){
                        var coin=org.minima.objects.Coin.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)data));
                        MinimaDB.getDB().getMegaMMR().getAllCoins().put(coin.getCoinID().to0xString(),coin);
                    }
                }else if(op.equals("txn")){
                    var transaction=new org.minima.objects.Transaction();var witness=new org.minima.objects.Witness();
                    for(Object c:(JSONArray)config.get("coins")){
                        var spec=(JSONObject)c;var coin=org.minima.objects.Coin.convertMiniDataVersion(new org.minima.objects.base.MiniData(spec.getString("data")));
                        if(spec.containsKey("created"))coin.setBlockCreated(new org.minima.objects.base.MiniNumber(spec.getString("created")));
                        if(spec.containsKey("entry"))coin.setMMREntryNumber(new org.minima.objects.mmr.MMREntryNumber(new java.math.BigInteger(spec.getString("entry"))));
                        transaction.addInput(coin);
                    }
                    for(Object proof:(JSONArray)config.get("proofs"))witness.addCoinProof(org.minima.objects.CoinProof.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)proof)));
                    for(Object script:(JSONArray)config.get("scripts"))witness.addScript(new org.minima.objects.ScriptProof((String)script));
                    transaction.mHaveCheckedMonotonic=true;transaction.mIsMonotonic=true;
                    MinimaDB.getDB().getCustomTxnDB().addCompleteTransaction(new org.minima.database.userprefs.txndb.TxnRow(config.getString("id"),transaction,witness));
                }
                else if(op.equals("validationFixture")){
                    var mmr=org.minima.objects.mmr.MMR.ReadFromStream(ConsensusOracle.data(config.getString("mmr")));
                    var pow=ConsensusOracle.txpow(config.getString("pow"));var parent=ConsensusOracle.txpow(config.getString("block"));parent.setBlockNumber(mmr.getBlockTime());parent.calculateTXPOWID();
                    var node=new org.minima.database.txpowtree.TxPoWTreeNode(parent);setField(node.getClass(),node,"mMMR",mmr);MinimaDB.getDB().getTxPoWTree().setRoot(node);
                    var txn=pow.getTransaction();txn.mHaveCheckedMonotonic=true;txn.mIsMonotonic=true;txn.mIsValid=false;
                    MinimaDB.getDB().getCustomTxnDB().addCompleteTransaction(new org.minima.database.userprefs.txndb.TxnRow("test",txn,pow.getWitness()));
                    if(config.containsKey("hashrate"))MinimaDB.getDB().getUserDB().setHashRate(new org.minima.objects.base.MiniNumber(config.getString("hashrate")));
                    if(Boolean.TRUE.equals(config.get("proofCoins")))for(var proof:pow.getWitness().getAllCoinProofs()){node.addCoinToNode(proof.getCoin());node.getRelevantCoins().add(proof.getCoin());}
                }else if(op.equals("generate")||op.equals("held")){
                    var row=MinimaDB.getDB().getCustomTxnDB().getTransactionRow("test");
                    if(op.equals("generate"))held=org.minima.system.brains.TxPoWGenerator.generateTxPoW(row.getTransaction(),row.getWitness());
                    var value=new JSONObject();value.put("sameTransaction",row!=null&&held.getTransaction()==row.getTransaction());value.put("sameWitness",row!=null&&held.getWitness()==row.getWitness());
                    value.put("transaction",ConsensusOracle.wire(held.getTransaction()));value.put("witness",ConsensusOracle.wire(held.getWitness()));result=value;
                }else if(op.equals("body")){
                    body=new org.minima.objects.TxBody();var row=MinimaDB.getDB().getCustomTxnDB().getTransactionRow("test");body.mTransaction=row.getTransaction();body.mWitness=row.getWitness();
                    if(Boolean.TRUE.equals(config.get("alias"))){body.mBurnTransaction=body.mTransaction;body.mBurnWitness=body.mWitness;}
                    body.mPRNG=new org.minima.objects.base.MiniData("0x12");body.mTxPowIDList.add(new org.minima.objects.base.MiniData("0x99"));
                    for(Object field:(JSONArray)config.get("nulls")){switch((String)field){case "transaction":body.mTransaction=null;break;case "witness":body.mWitness=null;break;case "burnTransaction":body.mBurnTransaction=null;break;default:body.mBurnWitness=null;}}
                }else if(op.equals("bodyread")){
                    var txn=body.mTransaction;var wit=body.mWitness;var burn=body.mBurnTransaction;var bw=body.mBurnWitness;
                    var bodyBytes=new org.minima.objects.base.MiniData(config.getString("data")).getBytes();var stream=new ByteArrayInputStream(bodyBytes);
                    try{body.readDataStream(new DataInputStream(stream));}catch(Exception e){out.put("readError",e.toString());}
                    var value=new JSONObject();value.put("offset",bodyBytes.length-stream.available());value.put("prng",body.mPRNG.to0xString());value.put("difficulty",body.mTxnDifficulty.to0xString());
                    value.put("sameTransaction",txn==body.mTransaction);value.put("sameWitness",wit==body.mWitness);value.put("sameBurn",burn==body.mBurnTransaction);value.put("sameBurnWitness",bw==body.mBurnWitness);
                    value.put("ids",body.mTxPowIDList.stream().map(org.minima.objects.base.MiniData::to0xString).toList());
                    try{value.put("wire",ConsensusOracle.wire(body));}catch(Exception e){value.put("wireError",e.toString());}result=value;
                }else if(op.equals("utilities")){
                    var row=MinimaDB.getDB().getCustomTxnDB().getTransactionRow(config.getString("id"));
                    if(Boolean.TRUE.equals(config.get("single")))org.minima.system.commands.txn.txnutils.setMMRandScripts(row.getTransaction().getAllInputs().get(0),row.getWitness());
                    else org.minima.system.commands.txn.txnutils.setMMRandScripts(row.getTransaction(),row.getWitness(),!config.containsKey("exit")||Boolean.TRUE.equals(config.get("exit")));
                    result=row.toJSON();
                }else if(op.equals("miner")){
                    var main=(org.minima.system.Main)unsafe().allocateInstance(org.minima.system.Main.class);
                    org.minima.system.brains.TxPoWMiner miner=null;
                    if(!Boolean.TRUE.equals(config.get("null"))){
                        miner=(org.minima.system.brains.TxPoWMiner)unsafe().allocateInstance(org.minima.system.brains.TxPoWMiner.class);
                        var ids=new ArrayList<String>();for(Object c:(JSONArray)config.get("coins"))ids.add(org.minima.objects.Coin.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)c)).getCoinID().to0xString());
                        setField(org.minima.system.brains.TxPoWMiner.class,miner,"mMiningCoins",ids);
                    }
                    setField(org.minima.system.Main.class,main,"mTxPoWMiner",miner);setField(org.minima.system.Main.class,null,"mMainInstance",main);
                }else if(op.equals("pool")){
                    var pow=new org.minima.objects.TxPoW();pow.getTxBody().mPRNG=new org.minima.objects.base.MiniData("0x"+"12".repeat(32));pow.setTimeMilli(new org.minima.objects.base.MiniNumber(0));pow.setTxDifficulty(new org.minima.objects.base.MiniData("0x"+"FF".repeat(32)));
                    for(Object data:(JSONArray)config.get("proofs")){
                        var proof=org.minima.objects.CoinProof.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)data));pow.getTransaction().addInput(proof.getCoin());
                        if(Boolean.TRUE.equals(config.get("burnWitness")))pow.getBurnWitness().addCoinProof(proof);else pow.getWitness().addCoinProof(proof);
                    }
                    pow.setHeaderBodyHash();pow.calculateTXPOWID();var data=new org.minima.database.txpowdb.ram.RamData(pow);
                    data.setInCascade(Boolean.TRUE.equals(config.get("cascade")));data.setOnMainChain(Boolean.TRUE.equals(config.get("onChain")));
                    MinimaDB.getDB().getTxPoWDB().getCompleteMemPool().put(pow.getTxPoWID(),data);result=pow.isTransaction();
                }else if(op.equals("automaticHelper")){
                    var row=org.minima.system.commands.txn.txnauto.createTransaction((String)config.get("address"),config.get("amount")==null?null:new org.minima.objects.base.MiniNumber(config.getString("amount")));
                    var value=new JSONObject();value.put("row",row.toJSON());value.put("wire",org.minima.objects.base.MiniData.getMiniDataVersion(row).to0xString());var verified=new JSONArray();
                    for(var signature:row.getWitness().getAllSignatures()){var key=new org.minima.objects.keys.TreeKey();key.setPublicKey(signature.getRootPublicKey());verified.add(key.verify(row.getTransaction().getTransactionID(),signature));}value.put("verified",verified);result=value;
                }else if(op.equals("burn")){
                    var excluded=new ArrayList<String>();for(Object id:(JSONArray)config.get("exclude"))excluded.add((String)id);
                    var row=org.minima.system.commands.txn.txnutils.createBurnTransaction(excluded,new org.minima.objects.base.MiniData(config.getString("link")),new org.minima.objects.base.MiniNumber(config.getString("amount")),!config.containsKey("sign")||Boolean.TRUE.equals(config.get("sign")));
                    var value=new JSONObject();value.put("row",row.toJSON());value.put("wire",org.minima.objects.base.MiniData.getMiniDataVersion(row).to0xString());result=value;
                }else if(op.equals("clean"))result=org.minima.kissvm.Contract.cleanScript(config.getString("script"),Boolean.TRUE.equals(config.get("log")));
                else if(op.equals("allowSave"))MinimaDB.getDB().setAllowSaveState(Boolean.TRUE.equals(config.get("allow")));
                else if(op.equals("reloadPrefs"))MinimaDB.getDB().getUserDB().loadDB(prefs);
                else if(op.equals("restart")){createOwner();wallet=MinimaDB.getDB().getWallet();}
                else if(op.equals("locked"))wallet.getBaseSeed().mSeed="0x00";
                else if(op.equals("meta")){
                    Command command=Boolean.TRUE.equals(config.get("classOnly"))?new org.minima.system.commands.base.magic():CommandRunner.getRunner().getCommandOnly(config.getString("name"));var meta=new JSONObject();
                    meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("valid",command.getValidParams());meta.put("fresh",command.getFunction().getJSONReply());result=meta;
                }else if(op.equals("direct")){
                    Command command=Boolean.TRUE.equals(config.get("classOnly"))?new org.minima.system.commands.base.magic():CommandRunner.getRunner().getCommandOnly(config.getString("name"));command.getParams().putAll((JSONObject)config.get("params"));if("$SIGNED".equals(command.getParams().get("file")))command.getParams().put("file",signedFile(offlineRoot));result=command.runCommand();
                }else result=CommandRunner.getRunner().runMultiCommand(config.getString("caller"),config.getString("text"));
                out.put("value",Boolean.TRUE.equals(config.get("replyText"))?org.minima.utils.json.JSONValue.toJSONString(result):result);
            }catch(Exception error){out.put("exception",error.toString());}finally{System.setOut(previous);}
            JSONArray logs=new JSONArray();for(String line:bytes.toString("UTF-8").split("\n")){int at=line.indexOf("] : ");if(at>=0&&!line.substring(at+4).startsWith("     "))logs.add(line.substring(at+4));}
            boolean captureImportObservation=input.containsKey("coinImport");
            if(captureImportObservation)System.setOut(new PrintStream(bytes,true,"UTF-8"));
            try{if(input.containsKey("nodeState")){
                var savedPreferences=new File(root,"control-prefs");out.put("savedPreferences",savedPreferences.exists()?new org.minima.objects.base.MiniData(org.minima.utils.MiniFile.readCompleteFile(savedPreferences)).to0xString():null);
                try{out.put("preferenceWire",org.minima.objects.base.MiniData.getMiniDataVersion(MinimaDB.getDB().getUserDB()).to0xString());}catch(Exception error){out.put("preferenceError",error.toString());}
                try{var nodes=new JSONArray();for(var node=MinimaDB.getDB().getTxPoWTree().getTip();node!=null;node=node.getParent()){
                    var value=new JSONObject();var entries=new JSONArray();var coins=new JSONArray();for(var entryNumber:node.getRelevantCoinsEntries())entries.add(entryNumber.toString());for(var coin:node.getRelevantCoins())coins.add(org.minima.objects.base.MiniData.getMiniDataVersion(coin).to0xString());
                    value.put("entries",entries);value.put("coins",coins);value.put("mmr",org.minima.objects.base.MiniData.getMiniDataVersion(node.getMMR()).to0xString());
                    if(input.containsKey("coinImport")){var all=new JSONArray();for(var coin:node.getAllCoins())all.add(org.minima.objects.base.MiniData.getMiniDataVersion(coin).to0xString());value.put("allCoins",all);var rootData=node.getMMR().getRoot();value.put("root",rootData==null?null:org.minima.objects.base.MiniData.getMiniDataVersion(rootData).to0xString());value.put("finalized",node.getMMR().isFinalized());}
                    nodes.add(value);
                }out.put("nodeState",nodes);}catch(Exception error){out.put("nodeStateError",error.toString());}
            }}finally{if(captureImportObservation)System.setOut(previous);}
            if(captureImportObservation){logs.clear();for(String captured:bytes.toString("UTF-8").split("\n")){int at=captured.indexOf("] : ");if(at>=0&&!captured.substring(at+4).startsWith("     "))logs.add(captured.substring(at+4));}}
            if(input.containsKey("history")){out.put("before",before);out.put("after",System.currentTimeMillis());out.put("historyCalls",historyCalls);}
            try{out.put("txnwire",org.minima.objects.base.MiniData.getMiniDataVersion(MinimaDB.getDB().getCustomTxnDB()).to0xString());}catch(Exception e){out.put("txnwireError",e.toString());}
            var txnflags=new JSONArray();for(var row:MinimaDB.getDB().getCustomTxnDB().listTxns()){var txn=row.getTransaction();txnflags.add(new JSONArray(Arrays.asList(txn.mHaveCheckedMonotonic,txn.mIsMonotonic,txn.mIsValid)));}out.put("txnflags",txnflags);
            JSONArray checked=new JSONArray();for(var row:MinimaDB.getDB().getCustomTxnDB().listTxns())checked.add(row.getTransaction().isCheckedMonotonic());out.put("checked",checked);out.put("mining",miningSnapshot());
            if(vaultEnabled)out.put("vault",vaultSnapshot());
            if(input.containsKey("consolidation")||input.containsKey("txpowSearch"))out.put("forwarded",forwarded);
            if(input.containsKey("funding")){out.put("coinLocks",new JSONArray(org.minima.system.commands.txn.txnaddamount.LOCKED_COINS));out.put("coinLockEnabled",org.minima.system.commands.txn.txnaddamount.isCoinLockEnabled());}
            if(offline){
                var files=new JSONObject();var structures=new JSONObject();var directories=new ArrayList<String>();
                try(var paths=java.nio.file.Files.walk(offlineRoot.toPath())){for(var path:paths.toList())if(!path.equals(offlineRoot.toPath())){
                    String relative=offlineRoot.toPath().relativize(path).toString();
                    if(java.nio.file.Files.isDirectory(path))directories.add(relative);
                    else if(java.nio.file.Files.isRegularFile(path)){var fileData=java.nio.file.Files.readAllBytes(path);files.put(relative,new org.minima.objects.base.MiniData(fileData).to0xString());if(input.containsKey("unsigned"))structures.put(relative,unsignedFileObservation(fileData));}
                }}
                Collections.sort(directories);out.put("files",files);out.put("directories",new JSONArray(directories));if(input.containsKey("unsigned"))out.put("fileTxPoW",structures);
            }
            out.put("events",events);out.put("logs",logs);JSONArray cache=new JSONArray();
            var probes=new ArrayList<Object>((JSONArray)config.get("probe"));probes.add(org.minima.objects.Address.TRUE_ADDRESS.getAddressData().to0xString());
            for(Object a:probes){var address=(String)a;var row=new JSONObject();row.put("address",address);row.put("tracked",wallet.isAddressRelevant(address));row.put("simple",wallet.isAddressSimple(address));row.put("notify",MinimaDB.getDB().checkCoinNotify(address));cache.add(row);}out.put("cache",cache);
            out.put("userEnabled",MinimaDB.getDB().getUserDB().isSlaveNode());out.put("userHost",MinimaDB.getDB().getUserDB().getSlaveNodeHost());
            out.put("prefs",prefs.isFile()?new org.minima.objects.base.MiniData(java.nio.file.Files.readAllBytes(prefs.toPath())).to0xString():null);outputs.add(out);
        }
        JSONObject result=new JSONObject();result.put("ops",outputs);return result;
    }
    public static void main(String[] args)throws Exception{
        installDriver();GeneralParams.SEED_PHRASE="fixture seed";GeneralParams.ANYSEED_PHRASE=true;
        var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null){
            var request=(JSONObject)new JSONParser().parse(line);
            if("offlineFixtures".equals(request.get("kind"))){
                var fixtures=new JSONArray();var coin=new org.minima.objects.Coin(new org.minima.objects.base.MiniData("0x65"),new org.minima.objects.base.MiniData("0x01"),org.minima.objects.base.MiniNumber.ONE,org.minima.objects.Token.TOKENID_MINIMA);
                for(int main=0;main<3;main++)for(int burn=0;burn<3;burn++){
                    var pow=new org.minima.objects.TxPoW();pow.setTimeMilli(org.minima.objects.base.MiniNumber.ZERO);pow.getTxBody().mPRNG=new org.minima.objects.base.MiniData("0x"+"12".repeat(32));
                    for(int i=0;i<main;i++)pow.getTransaction().addInput(coin);for(int i=0;i<burn;i++)pow.getBurnTransaction().addInput(coin);
                    pow.calculateTXPOWID();var item=new JSONObject();item.put("main",main);item.put("burn",burn);item.put("wire",org.minima.objects.base.MiniData.getMiniDataVersion(pow).to0xString());item.put("json",pow.toJSON());fixtures.add(item);
                }
                System.out.println(fixtures.toJSONString());


            }else if("txpowSearchFixtures".equals(request.get("kind"))){
                var result=new JSONArray();for(Object wire:(JSONArray)request.get("wires")){
                    var value=org.minima.objects.TxPoW.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)wire));var row=new JSONObject();
                    row.put("id",value.getTxPoWID());row.put("wire",wire);row.put("block",org.minima.objects.base.MiniData.getMiniDataVersion(new org.minima.objects.TxBlock(value)).to0xString());row.put("json",value.toJSON().toJSONString());result.add(row);
                }System.out.println(result.toJSONString());
            }else if("historyFixtures".equals(request.get("kind"))){
                var result=new JSONArray();int index=0;
                for(Object value:(JSONArray)request.get("cases")){
                    var spec=(JSONObject)value;var pow=new org.minima.objects.TxPoW();pow.setTimeMilli(org.minima.objects.base.MiniNumber.ZERO);pow.setNonce(new org.minima.objects.base.MiniNumber(index++));pow.setBlockNumber(new org.minima.objects.base.MiniNumber(spec.getString("block","20")));pow.getTxBody().mPRNG=new org.minima.objects.base.MiniData("0x"+"12".repeat(32));
                    for(String key:new String[]{"inputs","outputs","burnInputs","burnOutputs"}){
                        if(!spec.containsKey(key))continue;
                        var transaction=key.startsWith("burn")?pow.getBurnTransaction():pow.getTransaction();
                        for(Object wire:(JSONArray)spec.get(key)){
                            var coin=org.minima.objects.Coin.convertMiniDataVersion(new org.minima.objects.base.MiniData(wire instanceof JSONObject?((JSONObject)wire).getString("wire"):(String)wire));
                            if(wire instanceof JSONObject){
                                var replacement=new org.minima.objects.Coin(coin.getCoinID(),coin.getAddress(),new org.minima.objects.base.MiniNumber(((JSONObject)wire).getString("amount")),coin.getTokenID());replacement.setToken(coin.getToken());coin=replacement;
                            }
                            if(key.endsWith("nputs"))transaction.addInput(coin);else transaction.addOutput(coin);
                        }
                    }
                    if(spec.containsKey("time"))pow.setTimeMilli(new org.minima.objects.base.MiniNumber(spec.getString("time")));
                    if(spec.containsKey("state"))pow.getTransaction().addStateVariable(new org.minima.objects.StateVariable(0,spec.getString("state")));
                    if(spec.containsKey("transactions"))for(Object id:(JSONArray)spec.get("transactions"))pow.addBlockTxPOW(new org.minima.objects.base.MiniData((String)id));
                    pow.setHeaderBodyHash();pow.calculateTXPOWID();result.add(org.minima.objects.base.MiniData.getMiniDataVersion(pow).to0xString());
                }
                System.out.println(result.toJSONString());
            }else if("balanceFixtures".equals(request.get("kind"))){
                var value=SearcherOracle.proofFixtures();var groups=new JSONArray();int groupIndex=0;
                for(Object item:(JSONArray)request.get("groups")){
                    var spec=(JSONObject)item;boolean nativeToken=Boolean.TRUE.equals(spec.get("native"));
                    var token=new org.minima.objects.Token(new org.minima.objects.base.MiniData("0xA0"),new org.minima.objects.base.MiniNumber(spec.getString("scale")),new org.minima.objects.base.MiniNumber("100"),new org.minima.objects.base.MiniString(spec.getString("name")),new org.minima.objects.base.MiniString(spec.getString("script")),new org.minima.objects.base.MiniNumber("7"));
                    var tokenid=nativeToken?org.minima.objects.Token.TOKENID_MINIMA:token.getTokenID();var group=new JSONObject();var coins=new JSONArray();var spent=new JSONArray();var missing=new JSONArray();
                    for(int i=0;i<6;i++){
                        var coin=new org.minima.objects.Coin(new org.minima.objects.base.MiniData("0x"+Integer.toHexString(128+groupIndex*16+i)),new org.minima.objects.base.MiniData("0x0"+(1+i%3)),new org.minima.objects.base.MiniNumber(""+(i+1)),tokenid);
                        coin.setBlockCreated(new org.minima.objects.base.MiniNumber(new String[]{"8","9","11","12","0","10"}[i]));
                        if(Boolean.TRUE.equals(request.get("entries")))coin.setMMREntryNumber(new org.minima.objects.mmr.MMREntryNumber(i));
                        missing.add(org.minima.objects.base.MiniData.getMiniDataVersion(coin).to0xString());
                        if(!nativeToken)coin.setToken(token);
                        coins.add(org.minima.objects.base.MiniData.getMiniDataVersion(coin).to0xString());coin.setSpent(true);spent.add(org.minima.objects.base.MiniData.getMiniDataVersion(coin).to0xString());
                    }
                    group.put("id",tokenid.to0xString());group.put("coins",coins);group.put("spent",spent);group.put("missing",missing);groups.add(group);groupIndex++;
                }
                value.put("groups",groups);value.put("mx",org.minima.objects.Address.makeMinimaAddress(new org.minima.objects.base.MiniData("0x01")));System.out.println(value.toJSONString());
            }else if("consolidationFixtures".equals(request.get("kind"))){
                var value=SearcherOracle.proofFixtures();var variants=new JSONObject();
                var token=new org.minima.objects.Token(new org.minima.objects.base.MiniData("0xA0"),new org.minima.objects.base.MiniNumber(2),new org.minima.objects.base.MiniNumber(100),new org.minima.objects.base.MiniString("consolidation token"),new org.minima.objects.base.MiniString("RETURN TRUE"));
                value.put("tokenid",token.getTokenID().to0xString());
                for(String mode:new String[]{"grouped","tied","token","missingToken","young","many"}){
                    var coins=new JSONArray();int count=mode.equals("many")?25:6;
                    for(int i=0;i<count;i++){
                        String address="0x0"+(1+i%3);int amount=mode.equals("tied")?5:count-i;
                        var coin=new org.minima.objects.Coin(new org.minima.objects.base.MiniData("0x"+Integer.toHexString(128+i)),new org.minima.objects.base.MiniData(address),new org.minima.objects.base.MiniNumber(amount),mode.equals("token")||mode.equals("missingToken")?token.getTokenID():org.minima.objects.Token.TOKENID_MINIMA);
                        if(mode.equals("token"))coin.setToken(token);
                        coin.setBlockCreated(new org.minima.objects.base.MiniNumber(mode.equals("young")?9:0));coins.add(org.minima.objects.base.MiniData.getMiniDataVersion(coin).to0xString());
                    }
                    variants.put(mode,coins);
                }
                value.put("variants",variants);System.out.println(value.toJSONString());
            }else if("fundingFixtures".equals(request.get("kind"))){
                var fixtures=new JSONArray();
                for(String script:new String[]{"RETURN TRUE","RETURN FALSE"}){
                    var token=new org.minima.objects.Token(new org.minima.objects.base.MiniData("0xA0"),new org.minima.objects.base.MiniNumber(2),new org.minima.objects.base.MiniNumber(100),new org.minima.objects.base.MiniString("teaching token"),new org.minima.objects.base.MiniString(script),org.minima.objects.base.MiniNumber.ZERO);
                    var item=SearcherOracle.proofFixtures(token);item.put("tokenid",token.getTokenID().to0xString());item.put("script",script);fixtures.add(item);
                }
                System.out.println(fixtures.toJSONString());
            }else if("keyFixtures".equals(request.get("kind"))){
                var privateKey=org.minima.utils.Crypto.getInstance().hashObjects(new org.minima.objects.base.MiniData("0xAA"),new org.minima.objects.base.MiniData("0x00"));
                var tree=new org.minima.objects.keys.TreeKey(privateKey,2,2);var value=new JSONObject();value.put("privatekey",privateKey.to0xString());value.put("publickey",tree.getPublicKey().to0xString());System.out.println(value.toJSONString());
            }else if("vaultFixtures".equals(request.get("kind"))){
                var value=new JSONObject();var privateKey=new org.minima.objects.base.MiniData("0x"+"12".repeat(32));var tree=new org.minima.objects.keys.TreeKey(privateKey,2,2);value.put("publickey",tree.getPublicKey().to0xString());
                value.put("encrypted",org.minima.utils.encrypt.PasswordCrypto.encryptPassword("fixture pass",new org.minima.objects.base.MiniData(new org.minima.objects.base.MiniString("fixture seed").getData())).to0xString());System.out.println(value.toJSONString());
            }else if("checkFixtures".equals(request.get("kind"))){
                var fixtures=new JSONArray();var stdout=System.out;System.setOut(System.err);
                for(int mode=0;mode<30;mode++)if(mode!=26)for(String script:new String[]{"RETURN TRUE","RETURN FALSE","RETURN @BLKNUM EQ 19"}) {
                    var value=new JSONObject();var wire=ConsensusOracle.fixture(mode,script,3,1024);var parts=wire.split(" ");
                    if(mode==0){var bodyless=ConsensusOracle.txpow(parts[1]);bodyless.clearBody();bodyless.calculateTXPOWID();value.put("bodyless",ConsensusOracle.wire(bodyless));}
                    value.put("row",org.minima.objects.base.MiniData.getMiniDataVersion(new org.minima.database.userprefs.txndb.TxnRow("test",ConsensusOracle.txpow(parts[1]).getTransaction(),ConsensusOracle.txpow(parts[1]).getWitness())).to0xString());value.put("mode",mode);value.put("script",script);value.put("mmr",parts[0]);value.put("pow",parts[1]);value.put("block",parts[2]);value.put("body",ConsensusOracle.wire(ConsensusOracle.txpow(parts[1]).getTxBody()));fixtures.add(value);
                }
                System.setOut(stdout);System.out.println(fixtures.toJSONString());
            }else if("witnessFixtures".equals(request.get("kind"))){
                var fixtures=SearcherOracle.proofFixtures();var coins=new JSONArray();var branches=new JSONArray();
                for(Object data:(JSONArray)fixtures.get("proofs")){var cp=org.minima.objects.CoinProof.convertMiniDataVersion(new org.minima.objects.base.MiniData((String)data));coins.add(org.minima.objects.base.MiniData.getMiniDataVersion(cp.getCoin()).to0xString());branches.add(org.minima.objects.base.MiniData.getMiniDataVersion(cp.getMMRProof()).to0xString());}
                var scriptcoins=new JSONArray();for(String code:new String[]{"RETURN TRUE","RETURN FALSE","RETURN SIGNEDBY(0xFF)",""}){var proof=new org.minima.objects.ScriptProof(code);var coin=new org.minima.objects.Coin(new org.minima.objects.base.MiniData("0x01"),proof.getAddressData(),org.minima.objects.base.MiniNumber.ONE,org.minima.objects.Token.TOKENID_MINIMA);scriptcoins.add(org.minima.objects.base.MiniData.getMiniDataVersion(coin).to0xString());}
                var tokenfixtures=(JSONObject)SearcherOracle.fixtures();fixtures.put("tokenids",tokenfixtures.get("tokenids"));fixtures.put("tokencoins",tokenfixtures.get("coins"));fixtures.put("inputcoins",coins);fixtures.put("scriptcoins",scriptcoins);fixtures.put("branches",branches);System.out.println(fixtures.toJSONString());
            }else System.out.println(runCommands(request,args[0]).toJSONString());
        }
    }
}
