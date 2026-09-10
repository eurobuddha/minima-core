import java.io.*;
import java.util.*;
import org.minima.objects.*;
import org.minima.database.MinimaDB;
import org.minima.system.brains.*;
import org.minima.system.params.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class TxBlockOwnershipOracle {
    static int num(JSONObject op,String key,int fallback){return op.containsKey(key)?((Number)op.get(key)).intValue():fallback;}
    static int id(ArrayList<TxPoW> ids,TxPoW tx){if(tx==null)return -1;int i=ids.indexOf(tx);if(i<0){i=ids.size();ids.add(tx);}return i;}
    static Object run(JSONObject input)throws Exception{
        if(input.containsKey("checker"))return check(input);
        var target=ObjectOracle.blockFixture(num(input,"size",2),num(input,"flags",1),num(input,"time",7));var sourceBytes=MiniData.getMiniDataVersion(target).getBytes();var held=target.getTxPoW();var main=ArchiveManagerOracle.txpow(11,0);var other=ArchiveManagerOracle.txpow(12,0);var list=new ArrayList<TxPoW>(Arrays.asList(other,main));TxPoWTreeNode node=null;
        var removed=new ArrayList<String>();for(var coin:target.getOutputCoins())removed.add(coin.getCoinID().to0xString());var ids=new ArrayList<TxPoW>(Arrays.asList(held,main,other));var rows=new JSONArray();
        for(Object raw:(JSONArray)input.get("ops")){var op=(JSONObject)raw;var row=new JSONObject();try{switch((String)op.get("cmd")){
            case "read":{int size=Math.min(sourceBytes.length,num(op,"length",sourceBytes.length));var bytes=new ByteArrayInputStream(Arrays.copyOf(sourceBytes,size));try{target.readDataStream(new DataInputStream(bytes));}finally{row.put("offset",size-bytes.available());}break;}
            case "held":held.setNonce(new MiniNumber(num(op,"nonce",0)));break;case "main":main.setNonce(new MiniNumber(num(op,"nonce",0)));break;case "target":target.getTxPoW().setNonce(new MiniNumber(num(op,"nonce",0)));break;
            case "simple":target=new TxBlock(Boolean.TRUE.equals(op.get("null"))?null:main);break;case "link":main.addBlockTxPOW(other.getTxPoWIDData());break;case "listNull":list=null;break;case "nullEntry":list.add(num(op,"index",0),null);break;case "empty":list.clear();break;
            case "construct":target=new TxBlock(Boolean.TRUE.equals(op.get("nullParent"))?null:new MMR(),Boolean.TRUE.equals(op.get("nullMain"))?null:main,list);break;case "node":node=new TxPoWTreeNode(target,false);break;case "info":break;default:throw new IllegalArgumentException((String)op.get("cmd"));
        }}catch(Exception e){row.put("error",e.toString());}
        row.put("tx",id(ids,target.getTxPoW()));row.put("held",held.getNonce().toString());row.put("main",main.getNonce().toString());try{row.put("wire",ObjectOracle.wire(target));}catch(Exception e){row.put("wireError",e.toString());}row.put("peaks",target.getPreviousPeaks().size());row.put("spent",target.getInputCoinProofs().size());row.put("outputs",target.getOutputCoins().size());if(node!=null){row.put("nodeTx",id(ids,node.getTxPoW()));row.put("nodeNonce",node.getTxPoW().getNonce().toString());}var states=new JSONArray();for(String key:removed){var value=target.removedState(key);states.add(value==null?null:Coin.convertStateListToJSON(value));}row.put("removed",states);rows.add(row);}
        var out=new JSONObject();out.put("bytes",sourceBytes.length);out.put("ops",rows);return out;
    }
    static Object check(JSONObject input)throws Exception{
        GeneralParams.BLOCK_LOGS=false;GeneralParams.USE_SQL_COINDB=false;MinimaDB.createDB();
        var parent=new TxPoW();parent.setBlockNumber(new MiniNumber(1));parent.setTimeMilli(new MiniNumber(0));parent.setBlockDifficulty(Magic.MIN_TXPOW_WORK);for(int i=0;i<GlobalParams.MINIMA_CASCADE_LEVELS;i++)parent.setSuperParent(i,MiniData.ZERO_TXPOWID);parent.setHeaderBodyHash();parent.calculateTXPOWID();
        var tip=new TxPoWTreeNode(parent);tip.getMMR().addEntry(MMRData.CreateMMRDataLeafNode(new MiniNumber(1),new MiniNumber(1)));tip.getMMR().finalizeSet();MinimaDB.getDB().getTxPoWTree().setRoot(tip);MinimaDB.getDB().getUserDB().setHashRate(new MiniNumber(1000000));
        var block=TxPoWGenerator.generateTxPoW(new Transaction(),new Witness());var list=new ArrayList<TxPoW>();var txn=new TxPoW();txn.setTxDifficulty(Magic.MIN_TXPOW_WORK);txn.calculateTXPOWID();for(int i=0;i<num(input,"count",2);i++)list.add(txn);
        int mode=num(input,"mode",0);if(mode==1)list=null;else if(mode==2)list.add(0,null);else if(mode==3)list.add(null);else if(mode==4)block=null;else if(mode==5)block.getTxHeader().mChainID=new MiniData("0x01");
        var steps=new JSONArray();for(int i=0;i<3;i++){var row=new JSONObject();boolean valid;if(Boolean.TRUE.equals(input.get("timed"))){var checker=new TimedChecker();valid=checker.checkTxPoWBlock(tip,block,list);row.put("finished",NIOClientOracle.get(checker,"mFinishedRunning"));}else valid=TxPoWChecker.checkTxPoWBlock(tip,block,list);row.put("valid",valid);var flags=new JSONArray();for(var t:new Transaction[]{txn.getTransaction(),txn.getBurnTransaction()}){flags.add(t.mHaveCheckedMonotonic);flags.add(t.mIsMonotonic);flags.add(t.mIsValid);}row.put("flags",flags);steps.add(row);if(i==0)txn.getTransaction().mIsValid=false;if(i==1)txn.getTransaction().clearIsMonotonic();}
        var out=new JSONObject();out.put("steps",steps);return out;
    }
    public static void main(String[] args)throws Exception{var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception e){e.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
