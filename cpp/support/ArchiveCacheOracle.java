import java.io.*;
import java.util.*;
import org.minima.database.archive.*;
import org.minima.database.txpowdb.sql.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
public class ArchiveCacheOracle {
    static String key(int i){String s="";for(int bit=0;bit<7;bit++)s+=((i>>bit)&1)==0?"BB":"Aa";return s;}
    static String ids(ArrayList<TxBlock> list){String out="";for(var b:list)out+=b.getTxPoW().getTxPoWID()+",";return out;}
    static TxPoW txpow(int i,int mode){var t=new TxPoW();t.setTimeMilli(new MiniNumber(i*1000));t.setNonce(new MiniNumber(i));t.setBlockNumber(new MiniNumber(i));t.getTxBody().mPRNG=new MiniData("0x12");for(int n=0;n<32;n++)t.setSuperParent(n,new MiniData("0x00"));t.getTxHeader().mTimeMilli=new MiniNumber(i*1000);t.setHeaderBodyHash();if((mode&1)!=0&&i%2==0)t.clearBody();t.calculateTXPOWID();return t;}
    public static void main(String[] args)throws Exception{var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{var scan=new Scanner(line);String op=scan.next(),out="";
        if(op.equals("make")){int n=scan.nextInt(),mode=scan.nextInt();var list=new ArrayList<TxPoW>();var wrapped=new TxPoWList(list);for(int i=0;i<n;i++)list.add(txpow(i,mode));out=MiniData.getMiniDataVersion(wrapped).to0xString();}
        else if(op.equals("list")){byte[] bytes=new MiniData(scan.next()).getBytes();boolean convert=scan.nextBoolean();var data=new DataInputStream(new ByteArrayInputStream(bytes));var list=new TxPoWList(new ArrayList<>());list.mTxPoWs.add(txpow(2,0));try{if(convert)list=TxPoWList.convertMiniDataVersion(new MiniData(bytes));else list.readDataStream(data);out=list==null?"null":MiniData.getMiniDataVersion(list).to0xString();}catch(Exception e){out=e instanceof IOException?"IO_ERROR":"ERROR";}if(!convert)out+="|"+(bytes.length-data.available())+"|"+list.mTxPoWs.size();}
        else if(op.equals("cache")){int n=scan.nextInt(),minimum=scan.nextInt(),mode=scan.nextInt();var db=new TxBlockDB();var rows=new ArrayList<TxBlock>();for(int i=0;i<n;i++){var b=new TxBlock(new TxPoW(key(i),i%37-5,1,true,"0x0"+(i%5),false));rows.add(b);db.addTxBlock(b);if((mode&1)!=0)db.addTxBlock(b);}
            for(var b:rows)out+=(db.findTxBlock(b.getTxPoW().getTxPoWID())==b)+",";out+="|";for(int i=0;i<5;i++)out+=ids(db.getChildBlocks("0x0"+i))+";";
            if((mode&2)!=0)for(int i=0;i<n;i+=3){var t=rows.get(i).getTxPoW();t.setTimeMilli(MiniNumber.ZERO);t.setNonce(new MiniNumber(i));t.calculateTXPOWID();}
            db.clearOld(new MiniNumber(minimum));out+="|";for(int i=0;i<5;i++)out+=ids(db.getChildBlocks("0x0"+i))+";";
            for(var b:rows)out+=(db.findTxBlock(b.getTxPoW().getTxPoWID())==b)+",";db.clearAll();out+="|";for(int i=0;i<5;i++)out+=db.getChildBlocks("0x0"+i).size()+",";
        }else throw new IllegalArgumentException(op);System.out.println(out);
    }catch(Exception e){System.out.println("ERROR");}}}
