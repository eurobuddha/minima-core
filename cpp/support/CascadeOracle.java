import java.io.*;
import java.math.*;
import java.util.*;
import java.nio.charset.StandardCharsets;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.database.cascade.*;
import org.minima.system.params.GlobalParams;
import org.minima.utils.*;
public class CascadeOracle {
    static String wire(Streamable obj) throws Exception {var out=new ByteArrayOutputStream();obj.writeDataStream(new DataOutputStream(out));return BaseConverter.encode16(out.toByteArray());}
    static String weight(BigDecimal x) {return x.unscaledValue()+":"+x.scale()+":"+x.toString();}
    static TxPoW pow(int n,int divisor) {
        var t=new TxPoW();t.setTimeMilli(new MiniNumber(1788883200000L+n));t.setNonce(new MiniNumber(n));t.setBlockNumber(new MiniNumber(n));
        t.getTxBody().mPRNG=new MiniData("0x000102");t.setBlockDifficulty(new MiniData(Crypto.MAX_VAL.divide(BigInteger.valueOf(divisor))));
        t.setHeaderBodyHash();t.calculateTXPOWID();return t;
    }
    static String result(Cascade c) throws Exception {
        String s=wire(c)+"|"+weight(c.getTotalWeight())+"|"+c.getLength()+"|"+Cascade.checkCascadeCorrect(c);
        for(var node=c.getTip();node!=null;node=node.getParent()) s+="|"+node.getTxPoW().getBlockNumber()+":"+node.getLevel()+":"+node.getSuperLevel()+":"+weight(node.getCurrentWeight())+":"+node.getTxPoW().hasBody();
        return s+"|"+BaseConverter.encode16(c.printCascade().getBytes(StandardCharsets.UTF_8));
    }
    public static void main(String[] args) throws Exception {
        var in=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=in.readLine())!=null) {try {
            var p=line.split(" ");String op=p[0];
            if(op.equals("weight")) {var x=new BigDecimal(new BigInteger(p[1]),Integer.parseInt(p[2]));System.out.println(weight(x.multiply(new BigDecimal(new BigInteger(p[3])),MathContext.DECIMAL32))+"|"+weight(x.add(new BigDecimal(new BigInteger(p[4]),Integer.parseInt(p[5])))));continue;}
            if(op.equals("node")) {var n=new CascadeNode(pow(Integer.parseInt(p[1]),Integer.parseInt(p[2])));n.setLevel(Integer.parseInt(p[3]));System.out.println(wire(n)+"|"+weight(n.getCurrentWeight())+"|"+BaseConverter.encode16(n.toString().getBytes(StandardCharsets.UTF_8)));continue;}
            if(op.equals("readnode")) {var b=new ByteArrayInputStream(BaseConverter.decode16(p[1]));var n=CascadeNode.ReadFromStream(new DataInputStream(b));System.out.println(wire(n)+"|"+weight(n.getCurrentWeight())+"|"+n.getSuperLevel()+"|"+n.getTxPoW().hasBody()+"|"+b.available());continue;}
            GlobalParams.MINIMA_CASCADE_LEVEL_NODES=Integer.parseInt(p[1]);
            Cascade c;
            if(op.equals("read")) {var b=new ByteArrayInputStream(BaseConverter.decode16(p[2]));c=Cascade.ReadFromStream(new DataInputStream(b));System.out.println(result(c)+"|"+b.available());continue;}
            c=new Cascade();int count=Integer.parseInt(p[2]),divisor=Integer.parseInt(p[3]),mode=Integer.parseInt(p[4]);var parents=new MiniData[32];Arrays.fill(parents,new MiniData("0x00"));
            for(int i=0;i<count;i++) {var t=pow(i,divisor);for(int j=0;j<32;j++)t.setSuperParent(j,parents[j]);t.calculateTXPOWID();for(int j=0;j<=t.getSuperLevel();j++)parents[j]=t.getTxPoWIDData();c.addToTip(t);if((mode&1)!=0)c.cascadeChain();}
            if(c.getTip()!=null) {
                if((mode&2)!=0)c.getTip().setLevel(3);
                if((mode&4)!=0)c.getTip().getTxPoW().setSuperParent(0,new MiniData("0x00"));
                if((mode&8)!=0)c.getTip().getTxPoW().setSuperParent(1,new MiniData("0x1234"));
                if((mode&16)!=0)c.getTip().getTxPoW().setSuperParent(0,c.getTip().getTxPoW().getTxPoWIDData());
            }
            if((mode&32)!=0)c.cascadeChain();
            if((mode&64)!=0)c=c.deepCopy();
            System.out.println(result(c));
        } catch(Exception e) {System.out.println("ERROR");}}
    }
}
