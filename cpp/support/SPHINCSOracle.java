import java.io.*;
import java.nio.charset.StandardCharsets;
import org.minima.objects.base.*;
import org.minima.objects.keys.*;
import org.minima.objects.mmr.*;
import org.minima.utils.json.*;
import org.minima.utils.sphincs.*;
import org.minima.utils.sphincs.FORS.*;

/** Calls the actual pinned signer; reuses the existing wire and error observers. */
public class SPHINCSOracle extends FORSOracle {
    static SPHINCS signer=new SPHINCS();
    static JSONObject state() {
        JSONObject r=new JSONObject();r.put("public",signer.getPublicKey()==null?null:signer.getPublicKey().to0xString());
        r.put("private",signer.getPrivateKey()==null?null:signer.getPrivateKey().to0xString());
        r.put("total",signer.getTotalWotsKeys()==null?null:signer.getTotalWotsKeys().toString());return r;
    }
    public static void main(String[] args) throws Exception {
        if(args.length>0&&args[0].equals("--demo")){SPHINCS.main(new String[0]);return;}
        BufferedReader in=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=in.readLine())!=null) {
            JSONObject q=(JSONObject)JSONValue.parseWithException(line),r=new JSONObject();JSONArray logs=new JSONArray();PrintStream original=System.out;
            var oldPub=signer.getPublicKey();var oldPrivate=signer.getPrivateKey();var oldTotal=signer.getTotalWotsKeys();
            System.setOut(new PrintStream(new ByteArrayOutputStream()){@Override public void println(String text){int pos=text.indexOf("] : ");logs.add(pos>=0?text.substring(pos+4):text);}});
            try {
                String action=(String)q.get("action");
                if(action.equals("fresh"))signer=new SPHINCS();
                else if(action.equals("seed"))signer.initSeed(data(q.get("seed")));
                else if(action.equals("private")){MiniData priv=data(q.get("private"));try{signer.initPrivateKey(priv);}finally{r.put("inputIdentity",priv==signer.getPrivateKey());}}
                else if(action.equals("sign")||action.equals("static")){
                    SPHINCSSignature s=action.equals("static")?SPHINCS.signMessage(data(q.get("message")),data(q.get("private"))):signer.signMessage(data(q.get("message")));
                    r.put("wire",new MiniData(wire(s)).to0xString());r.put("json",s.toJSON());
                }else if(action.equals("verify")){
                    SPHINCSSignature s=null;
                    if(q.get("wire")!=null){s=new SPHINCSSignature();s.readDataStream(new DataInputStream(new ByteArrayInputStream(data(q.get("wire")).getBytes())));
                        int mask=integer(q,"nulls",0);s=new SPHINCSSignature((mask&1)!=0?null:s.getWOTSSignature(),(mask&2)!=0?null:s.getFORSRoot(),(mask&4)!=0?null:s.getFORSSignature());}
                    r.put("valid",SPHINCS.verifySignature(data(q.get("message")),s,data(q.get("public"))));
                }else if(action.equals("log"))SPHINCS.log((String)q.get("message"));
                else if(!action.equals("state"))throw new IllegalArgumentException("Unknown fixture action");
            }catch(Exception e){r.put("error",error(e));}finally{System.setOut(original);}
            r.put("state",state());r.put("samePublic",oldPub==signer.getPublicKey());r.put("samePrivate",oldPrivate==signer.getPrivateKey());r.put("sameTotal",oldTotal==signer.getTotalWotsKeys());r.put("logs",logs);System.out.println(r.toJSONString());
        }
    }
}
