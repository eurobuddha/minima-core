import java.io.*;
import java.util.*;
import org.minima.database.archive.RawArchiveInput;
import org.minima.database.cascade.Cascade;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.utils.Crypto;
public class RawArchiveOracle {
    static String describe(IBD ibd){return ibd.getTxBlocks().size()+","+ibd.hasCascade()+","+Crypto.getInstance().hashObject(ibd).to0xString();}
    public static void main(String[] args)throws Exception{var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{var scan=new Scanner(line);String op=scan.next();
        if(op.equals("make")){int n=scan.nextInt(),cascade=scan.nextInt(),declared=scan.nextInt();var bytes=new ByteArrayOutputStream();var out=new DataOutputStream(bytes);new MiniByte(cascade>=0).writeDataStream(out);if(cascade>=0){var c=new Cascade();for(int i=0;i<cascade;i++)c.addToTip(ArchiveCacheOracle.txpow(i,0));c.writeDataStream(out);}new MiniNumber(declared).writeDataStream(out);for(int i=0;i<n;i++)new TxBlock(ArchiveCacheOracle.txpow(i,0)).writeDataStream(out);System.out.println(new MiniData(bytes.toByteArray()).to0xString());continue;}
        var raw=new RawArchiveInput(new File(scan.next()));String result="";while(scan.hasNext()){String action=scan.next();try{if(action.equals("c")){raw.connect();result+="OK";}else if(action.equals("s")){raw.stop();result+="STOP";}else result+=describe(raw.getNextIBD());}catch(Exception e){result+=e instanceof IOException?"IO_ERROR":"ERROR";}result+="|";}
        result+="cascade:"+(raw.getCascade()==null?"null":Crypto.getInstance().hashObject(raw.getCascade()).to0xString());try{raw.stop();}catch(Exception e){}System.out.println(result);
    }catch(Exception e){System.out.println("ERROR");}}
}
