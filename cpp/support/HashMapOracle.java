import java.io.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.nio.charset.StandardCharsets;
import org.minima.database.txpowdb.ram.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.utils.*;
public class HashMapOracle {
    static String text(String h){return new String(BaseConverter.decode16(h),StandardCharsets.UTF_8);}
    static String hex(String s){return BaseConverter.encode16(s.getBytes(StandardCharsets.UTF_8));}
    static String ids(ArrayList<TxPoW> list){String s="";for(var t:list)s+=t.getTxPoWID()+",";return s;}
    static TxPoW transaction(int i) throws Exception {
        var t=new TxPoW();t.setTimeMilli(MiniNumber.ZERO);t.setNonce(new MiniNumber(i));t.getTxBody().mPRNG=new MiniData("0x12");
        if(i%5!=0){var c=new Coin(new MiniData("0x"+Integer.toHexString(i+1)),new MiniData("0x1122"),new MiniNumber(i%13+1),Token.TOKENID_MINIMA);t.getTransaction().addInput(c);t.getWitness().addCoinProof(new CoinProof(c,new MMRProof()));}
        t.setHeaderBodyHash();t.calculateTXPOWID();return t;
    }
    public static void main(String[] args) throws Exception {var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{
        var scan=new Scanner(line);String op=scan.next(),out="";
        if(op.equals("map")){
            var map=new HashMap<String,Integer>();int count=scan.nextInt();
            for(int i=0;i<count;i++){String cmd=scan.next();Object ret;
                if(cmd.equals("put"))ret=map.put(text(scan.next()),scan.nextInt());
                else if(cmd.equals("get"))ret=map.get(text(scan.next()));
                else if(cmd.equals("remove"))ret=map.remove(text(scan.next()));
                else{map.clear();ret="clear";}
                out+=ret+":"+map.size()+":";for(var k:map.keySet()){out+=hex(k)+"="+map.get(k)+",";}out+="|";
            }
        }else if(op.equals("ram")){
            int count=scan.nextInt(),max=scan.nextInt(),mode=scan.nextInt();var db=new RamDB();db.MAX_UNUSED_TXNS=max;var txns=new ArrayList<TxPoW>();
            for(int i=0;i<count;i++){var t=transaction(i);txns.add(t);out+=db.addTxPoW(t)+":"+db.addTxPoW(t)+",";if((mode&1)!=0&&i%3==0)db.setOnMainChain(t.getTxPoWID());if((mode&2)!=0&&i%7==0)db.setInCascade(t.getTxPoWID());}
            out+="|"+db.getSize()+"|"+ids(db.getAllUnusedTxns())+"|";
            for(int i=0;i<count;i++)out+=db.checkForCoinID(new MiniData("0x"+Integer.toHexString(i+1)))+",";
            db.cleanUnusedTxn();out+="|"+db.getSize()+"|"+ids(db.getAllUnusedTxns())+"|";
            db.clearMainChainTxns();out+=ids(db.getAllUnusedTxns())+"|";
            for(var t:txns)out+=(db.getTxPoW(t.getTxPoWID())!=null)+",";
            var old=db.getCompleteMemPool();db.MAX_TIME=0;db.cleanDB();out+="|"+db.getSize()+":"+old.size();db.wipeRamDB();out+="|"+db.getSize();
        }else throw new IllegalArgumentException(op);
        System.out.println(out);
    }catch(Exception e){System.out.println("ERROR");}}
}
