import java.io.*;
import java.nio.charset.StandardCharsets;
import org.minima.utils.*;
import org.minima.utils.json.*;
import org.minima.objects.base.*;
import org.minima.objects.keys.*;
import org.minima.objects.mmr.*;
import org.minima.utils.sphincs.SPHINCSSignature;
import org.minima.utils.sphincs.FORS.FORSSignature;

/** Container validation reuses the actual FORS/HORST fixture helpers. */
public class SPHINCSSignatureOracle extends FORSOracle {
    static JSONObject snapshot(SPHINCSSignature s) throws Exception {
        JSONObject r=new JSONObject();r.put("wotsNull",s.getWOTSSignature()==null);r.put("rootNull",s.getFORSRoot()==null);r.put("forsNull",s.getFORSSignature()==null);
        if(s.getWOTSSignature()!=null)r.put("wotsCount",s.getWOTSSignature().getAllSignatureProofs().size());
        if(s.getFORSRoot()!=null)r.put("root",s.getFORSRoot().toJSON());
        if(s.getFORSSignature()!=null){var f=s.getFORSSignature();r.put("values",f.getHORSTSignature().getSignatureValues().size());r.put("proofs",f.getHORSTSignature().getPublicKeyTreeProofs().size());r.put("roots",f.getHORSTRoots().size());r.put("outerProofs",f.getHORSTTreeProofs().size());}
        ByteArrayOutputStream out=new ByteArrayOutputStream();try{s.writeDataStream(new DataOutputStream(out));}catch(Exception e){r.put("writeError",error(e));}
        r.put("wireHash",hash(out.toByteArray()));r.put("wireLength",out.size());try{r.put("json",s.toJSON());}catch(Exception e){r.put("jsonError",error(e));}return r;
    }
    public static void main(String[] args) throws Exception {
        BufferedReader in=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=in.readLine())!=null){JSONObject q=(JSONObject)JSONValue.parseWithException(line),r=new JSONObject();int[] logs={0};JSONArray kinds=new JSONArray();PrintStream original=System.out;
            System.setOut(new PrintStream(new ByteArrayOutputStream()){@Override public void println(String text){int pos=text.indexOf("] : java.");if(pos>=0){logs[0]++;if(kinds.size()<2)kinds.add(text.substring(pos+4).split(": ",2)[0]);}}});
            try {
                String action=(String)q.get("action");
                if(action.equals("convert"))r.put("state",snapshot(SPHINCSSignature.convertMiniDataVersion(data(q.get("wire")))));
                else {
                    Signature wots=q.get("wots")==null?null:Signature.ReadFromStream(new DataInputStream(new ByteArrayInputStream(data(q.get("wots")).getBytes())));
                    MMRData root=q.get("root")==null?null:new MMRData(data(q.get("root")),new MiniNumber((String)q.get("sum")));
                    FORSSignature fors=q.get("fors")==null?null:readFORS((String)q.get("fors"));
                    SPHINCSSignature s=Boolean.TRUE.equals(q.get("default"))?new SPHINCSSignature():new SPHINCSSignature(wots,root,fors);
                    var oldWots=s.getWOTSSignature();var oldRoot=s.getFORSRoot();var oldFors=s.getFORSSignature();
                    if(action.equals("read")) {
                        byte[] bytes=data(q.get("wire")).getBytes();ByteArrayInputStream source=new ByteArrayInputStream(bytes);
                        try{s.readDataStream(new DataInputStream(source));}catch(Exception e){r.put("readError",error(e));}
                        r.put("position",bytes.length-source.available());r.put("sameWots",oldWots==s.getWOTSSignature());r.put("sameRoot",oldRoot==s.getFORSRoot());r.put("sameFors",oldFors==s.getFORSSignature());
                        r.put("oldWotsCount",oldWots==null?-1:oldWots.getAllSignatureProofs().size());r.put("oldRoots",oldFors==null?-1:oldFors.getHORSTRoots().size());
                    } else {
                        r.put("wotsIdentity",wots==s.getWOTSSignature());r.put("rootIdentity",root==s.getFORSRoot());r.put("forsIdentity",fors==s.getFORSSignature());
                        if(Boolean.TRUE.equals(q.get("addWots")))wots.addSignatureProof(new SignatureProof(new MiniData("0xAA"),new MiniData("0xBB"),new MMRProof()));
                        if(Boolean.TRUE.equals(q.get("clearForsRoots")))fors.getHORSTRoots().clear();
                        if(Boolean.TRUE.equals(q.get("badWotsProof")))wots.getAllSignatureProofs().get(0).getProof().addProofChunk(false,new MMRData(new MiniData(new byte[65]),new MiniNumber(1)));
                        if(q.containsKey("forsMutation"))FORSOracle.mutate(fors,(JSONObject)q.get("forsMutation"));
                    }
                    r.put("state",snapshot(s));
                }
            }catch(Exception e){r.put("error",error(e));}finally{System.setOut(original);}
            r.put("logCount",logs[0]);r.put("logKinds",kinds);System.out.println(r.toJSONString());
        }
    }
}
