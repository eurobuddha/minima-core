import java.io.*;
import java.util.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.system.commands.backup.mmrsync.*;
import org.minima.utils.Streamable;
public class MMRSyncOracle {
    static String hex(Streamable value){if(value==null)return "null";try{var out=new ByteArrayOutputStream();value.writeDataStream(new DataOutputStream(out));return new MiniData(out.toByteArray()).to0xString();}catch(Exception e){return "ERROR";}}
    static String list(ArrayList<? extends Streamable> values){if(values==null)return "null";String out="[";for(var value:values)out+=hex(value)+",";return out+"]";}
    static Streamable make(String type,int n){
        var ibd=new IBD();for(int i=0;i<n;i++)ibd.getTxBlocks().add(new TxBlock(ArchiveCacheOracle.txpow(i,0)));
        if(type.equals("backup")){var mega=new MegaMMR();for(int i=0;i<n;i++)mega.getMMR().addEntry(MegaMMR.getCoinData(new MiniNumber(i+1)));return new MegaMMRBackup(mega,ibd);}
        if(type.equals("ibd")){var proofs=new ArrayList<CoinProof>();var result=new MegaMMRIBD(ibd,proofs);for(int i=0;i<n;i++)proofs.add(new CoinProof(new Coin(new MiniData("0x1234"),new MiniNumber(i+1),new MiniData("0x00")),new MMRProof(new MiniNumber(i))));result.setPeersList("127.0.0.1:9001,π.example:9002");return result;}
        var addresses=new ArrayList<MiniData>();var keys=new ArrayList<MiniData>();var result=new MegaMMRSyncData(addresses,keys);for(int i=0;i<n;i++){addresses.add(new MiniData(new byte[i]));keys.add(new MiniData(new byte[i+1]));}return result;
    }
    static String state(Streamable value){
        if(value instanceof MegaMMRBackup b)return hex(b.getMegaMMR())+"|"+hex(b.getIBD());
        if(value instanceof MegaMMRIBD b)return hex(b.getIBD())+"|"+list(b.getAllCoinProofs())+"|"+(b.getPeersList()==null?"null":new MiniData(new MiniString(b.getPeersList()).getData()).to0xString());
        var b=(MegaMMRSyncData)value;return list(b.getAllAddresses())+"|"+list(b.getAllPublicKeys());
    }
    static Object[] refs(Streamable value){if(value instanceof MegaMMRBackup b)return new Object[]{b.getMegaMMR(),b.getIBD()};if(value instanceof MegaMMRIBD b)return new Object[]{b.getIBD(),b.getAllCoinProofs()};var b=(MegaMMRSyncData)value;return new Object[]{b.getAllAddresses(),b.getAllPublicKeys()};}
    public static void main(String[] args)throws Exception{var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{var scan=new Scanner(line);String op=scan.next(),type=scan.next();int n=scan.nextInt();Streamable value=n<0?(type.equals("backup")?new MegaMMRBackup():type.equals("ibd")?new MegaMMRIBD():new MegaMMRSyncData()):make(type,n);
        if(op.equals("make")){System.out.println(hex(value));continue;}
        var bytes=new MiniData(scan.next()).getBytes();var data=new DataInputStream(new ByteArrayInputStream(bytes));var before=refs(value);String result="OK";try{value.readDataStream(data);}catch(Exception e){result=e instanceof IOException?"IO_ERROR":"ERROR";}var after=refs(value);System.out.println(result+"|"+(bytes.length-data.available())+"|"+(before[0]==after[0])+"|"+(before[1]==after[1])+"|"+state(value));
    }catch(Exception e){System.out.println("ERROR");}}
}
