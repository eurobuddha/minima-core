import java.io.*;
import java.math.*;
import java.util.*;
import org.minima.database.*;
import org.minima.database.archive.*;
import org.minima.database.cascade.*;
import org.minima.database.txpowtree.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.utils.Crypto;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.*;
public class IBDOracle {
    static JSONArray calls=new JSONArray();static Main main;
    static int integer(JSONObject o,String key,int fallback){Object v=o.get(key);return v==null?fallback:((Number)v).intValue();}
    static void set(Class<?> c,Object obj,String key,Object value)throws Exception{var f=c.getDeclaredField(key);f.setAccessible(true);f.set(obj,value);}
    static TxPoW tx(int height,int variant){var t=ArchiveCacheOracle.txpow(height,0);if(variant!=0){BigInteger d=Crypto.MAX_VAL.multiply(BigInteger.valueOf(variant==1?2:1)).divide(BigInteger.valueOf(variant==1?3:2));t.setBlockDifficulty(new MiniData(d));long nonce=height+100L*variant;do{t.setNonce(new MiniNumber(nonce++));t.calculateTXPOWID();}while(!t.isBlock());}return t;}
    static IBD make(JSONObject config){var ibd=new IBD();var cascade=new Cascade();int variant=integer(config,"variant",0);if(integer(config,"cascade",0)>=0){for(int i=0;i<integer(config,"cascade",0);i++)cascade.addToTip(tx(integer(config,"start",1)+i,variant));cascade.cascadeChain();if(integer(config,"bad",0)>0&&cascade.getTip()!=null)cascade.getTip().setLevel(1);ibd.setCascade(cascade);}int start=integer(config,"start",1)+Math.max(integer(config,"cascade",0),0);for(int i=0;i<integer(config,"count",0);i++)ibd.getTxBlocks().add(new TxBlock(tx(start+i,variant)));return ibd;}
    static JSONObject snapshot(IBD ibd){var out=new JSONObject();out.put("hascascade",ibd.hasCascade());out.put("cascadeblocks",ibd.hasCascadeWithBlocks());var blocks=new JSONArray();for(var block:ibd.getTxBlocks())blocks.add(block.getTxPoW().getTxPoWID());out.put("blocks",blocks);var cascade=new JSONArray();if(ibd.getCascade()!=null)for(var tip=ibd.getCascade().getTip();tip!=null;tip=tip.getParent())cascade.add(tip.getTxPoW().getTxPoWID());out.put("cascade",cascade);out.put("weight",ibd.getTotalWeight().toString());out.put("valid",ibd.checkValidData());try{out.put("root",ibd.getTreeRoot().toString());}catch(Exception e){out.put("root","ERROR");}try{out.put("tip",ibd.getTreeTip().toString());}catch(Exception e){out.put("tip","ERROR");}out.put("wire",MiniData.getMiniDataVersion(ibd).to0xString());return out;}
    static class Archive extends ArchiveManager {
        ArrayList<TxBlock> blocks;Cascade cascade;boolean fail;
        Archive(IBD ibd,boolean fail){blocks=ibd.getTxBlocks();cascade=ibd.getCascade();this.fail=fail;}
        void check(){if(fail)throw new IllegalStateException("Injected archive failure");}
        @Override public MiniNumber exists(String id){calls.add("exists:"+id);check();for(var b:blocks)if(b.getTxPoW().getTxPoWID().equals(id))return b.getTxPoW().getBlockNumber();return MiniNumber.MINUSONE;}
        @Override public ArrayList<TxBlock> loadBlockRange(MiniNumber start,MiniNumber end,boolean descending){calls.add("range:"+start+":"+end+":"+descending);check();var result=new ArrayList<TxBlock>();for(var b:blocks){var n=b.getTxPoW().getBlockNumber();if(n.isMore(start)&&n.isLess(end))result.add(b);}if(descending)Collections.reverse(result);return result;}
        @Override public ArrayList<TxBlock> loadSyncBlockRange(MiniNumber start){calls.add("sync:"+start);check();var result=new ArrayList<TxBlock>();for(var b:blocks)if(b.getTxPoW().getBlockNumber().isLess(start))result.add(b);Collections.reverse(result);return result;}
        @Override public TxBlock loadLastBlock(){calls.add("last");check();return blocks.isEmpty()?null:blocks.getFirst();}
        @Override public Cascade loadCascade(){calls.add("cascade");check();return cascade;}
    }
    static class DB extends MinimaDB {
        TxPowTree tree;Cascade cascade;Archive archive;int depth=0;
        DB(IBD current,Archive archive){tree=new TxPowTree();this.archive=archive;cascade=current.getCascade();TxPoWTreeNode root=null,last=null;for(var block:current.getTxBlocks()){var node=new TxPoWTreeNode(block.getTxPoW());if(root==null)root=node;else last.addChildNode(node);last=node;}if(root!=null)tree.setRoot(root);}
        @Override public TxPowTree getTxPoWTree(){return tree;}
        @Override public Cascade getCascade(){return cascade;}
        @Override public ArchiveManager getArchive(){return archive;}
        @Override public void readLock(boolean lock){super.readLock(lock);depth+=lock?1:-1;calls.add("lock:"+lock);}
    }
    static Object run(JSONObject req)throws Exception {
        calls=new JSONArray();String kind=(String)req.get("kind");
        if(kind.equals("wire")){var bytes=new MiniData((String)req.get("data")).getBytes();var data=new DataInputStream(new ByteArrayInputStream(bytes));var ibd=new IBD();var out=new JSONObject();try{ibd.readDataStream(data);out.put("result",snapshot(ibd));}catch(Exception e){out.put("error",e instanceof IOException?"IO_ERROR":"ERROR");}out.put("consumed",bytes.length-data.available());out.put("count",ibd.getTxBlocks().size());out.put("cascade",ibd.hasCascade());return out;}
        var current=make((JSONObject)req.get("current"));var archive=new Archive(make((JSONObject)req.get("archive")),integer(req,"fail",0)>0);var db=new DB(current,archive);set(MinimaDB.class,null,"mMinimaDB",db);set(Main.class,main,"mShuttingdown",integer(req,"shutdown",0)>0);
        if(kind.equals("make"))return snapshot(current);
        if(kind.equals("shorten")){String id="0x00";int at=integer(req,"index",-1);var list=new ArrayList<String>();for(var b:current.getTxBlocks())list.add(b.getTxPoW().getTxPoWID());if(current.getCascade()!=null)for(var tip=current.getCascade().getTip();tip!=null;tip=tip.getParent())list.add(tip.getTxPoW().getTxPoWID());if(at>=0&&at<list.size())id=list.get(at);return snapshot(IBD.createShortenedIBD(current,id));}
        if(kind.equals("compare"))return IBD.checkOurChainHeavier(make((JSONObject)req.get("other")));
        var ibd=new IBD();Object result="OK";if(integer(req,"preload",0)>0){ibd.getTxBlocks().add(new TxBlock(tx(999,0)));ibd.setCascade(new Cascade());}
        if(kind.equals("complete"))ibd.createCompleteIBD();
        else if(kind.equals("sync"))ibd.createSyncIBD(tx(integer(req,"first",1),0));
        else if(kind.equals("archive"))ibd.createArchiveIBD(new MiniNumber(integer(req,"first",0)),archive,integer(req,"local",0)>0);
        else if(kind.equals("greet")){var greeting=new Greeting();greeting.setTopBlock(new MiniNumber(integer(req,"top",-1)));for(Object x:(JSONArray)req.get("chain")){var item=(JSONArray)x;greeting.getChain().add(tx(((Number)item.get(0)).intValue(),((Number)item.get(1)).intValue()).getTxPoWIDData());}result=ibd.createIBD(greeting);}
        else throw new IllegalArgumentException(kind);
        var out=new JSONObject();out.put("result",result);out.put("ibd",snapshot(ibd));out.put("calls",calls);out.put("depth",db.depth);return out;
    }
    public static void main(String[] args)throws Exception {
        GeneralParams.USE_SQL_COINDB=false;
        // Allocate only to read Main's shutdown flag. Its constructor starts a node
        // and is deliberately never called by this offline oracle.
        var u=Class.forName("sun.misc.Unsafe");var f=u.getDeclaredField("theUnsafe");f.setAccessible(true);main=(Main)u.getMethod("allocateInstance",Class.class).invoke(f.get(null),Main.class);set(Main.class,null,"mMainInstance",main);
        var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception e){System.out.println("ERROR");}
    }
}
