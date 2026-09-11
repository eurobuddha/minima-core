import java.security.*;
import org.minima.objects.Address;
import org.minima.objects.base.MiniData;
import org.minima.system.brains.TxPoWGenerator;
public class ObjectMainsOracle {
    static byte[] bytes;static int draws;
    public static class FixedRandom extends SecureRandomSpi {
        protected void engineSetSeed(byte[] seed){}
        protected void engineNextBytes(byte[] target){if(target.length!=bytes.length)throw new AssertionError("random length");System.arraycopy(bytes,0,target,0,target.length);draws++;}
        protected byte[] engineGenerateSeed(int count){throw new AssertionError("unexpected generateSeed");}
    }
    public static void main(String[] args)throws Exception {
        String name=args[0],mode=args[1];
        if(mode.equals("fixed")) {bytes=new MiniData(args[2]).getBytes();Provider p=new Provider("ObjectMainsFixed","1","Fixture only"){};p.put("SecureRandom.Fixed",FixedRandom.class.getName());Security.insertProviderAt(p,1);}
        try {
            String[] forwarded=mode.equals("null")?null:mode.equals("args")?new String[]{"ignored","--not-a-real-option"}:new String[0];
            if(name.equals("Address"))Address.main(forwarded);
            else if(name.equals("MiniData"))MiniData.main(forwarded);
            else if(name.equals("TxPoWGenerator"))TxPoWGenerator.main(forwarded);
            else if(name.equals("randomLength")){var data=MiniData.getRandomData(Integer.parseInt(mode));System.out.println(data.getLength()+":"+data.to0xString());}
            else throw new AssertionError("unknown main");
        }catch(Exception e){System.out.println("EXCEPTION:"+e);}
        if(mode.equals("fixed")&&draws!=1)throw new AssertionError("Expected exactly one random draw, got "+draws);
    }
}
