import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.utils.*;
import org.minima.utils.json.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.utils.sphincs.FORS.*;

/** Reuses the HORST fixture's codecs/error observer and calls actual FORS. */
public class FORSOracle extends HORSTOracle {
    static FORS forest;
    static FORSSignature readFORS(String hex) throws Exception {FORSSignature s=new FORSSignature();s.readDataStream(new DataInputStream(new ByteArrayInputStream(data(hex).getBytes())));return s;}
    static void mutate(FORSSignature s,JSONObject r) throws Exception {
        HORSTOracle.mutate(s.getHORSTSignature(),r);
        if(r.containsKey("roots"))while(s.getHORSTRoots().size()>integer(r,"roots",0))s.getHORSTRoots().remove(s.getHORSTRoots().size()-1);
        if(r.containsKey("outerProofs"))while(s.getHORSTTreeProofs().size()>integer(r,"outerProofs",0))s.getHORSTTreeProofs().remove(s.getHORSTTreeProofs().size()-1);
        if(r.containsKey("nullRoot"))s.getHORSTRoots().set(integer(r,"nullRoot",0),null);
        if(r.containsKey("nullOuterProof"))s.getHORSTTreeProofs().set(integer(r,"nullOuterProof",0),null);
        if(r.containsKey("rootData"))s.getHORSTRoots().set(integer(r,"index",0),new MMRData(data(r.get("rootData")),new MiniNumber((String)r.get("rootSum"))));
        if(r.containsKey("outerProof"))s.getHORSTTreeProofs().set(integer(r,"index",0),MMRProof.ReadFromStream(new DataInputStream(new ByteArrayInputStream(data(r.get("outerProof")).getBytes()))));
        if(r.containsKey("oversizeOuterProof"))s.getHORSTTreeProofs().get(integer(r,"oversizeOuterProof",0)).addProofChunk(false,new MMRData(new MiniData(new byte[65]),new MiniNumber(1)));
        if(Boolean.TRUE.equals(r.get("extra"))){s.getHORSTRoots().add(null);s.getHORSTTreeProofs().add(null);}
    }
    static JSONObject snapshot(FORSSignature s) throws Exception {
        JSONObject r=new JSONObject();r.put("values",s.getHORSTSignature().getSignatureValues().size());r.put("proofs",s.getHORSTSignature().getPublicKeyTreeProofs().size());r.put("roots",s.getHORSTRoots().size());r.put("outerProofs",s.getHORSTTreeProofs().size());
        ByteArrayOutputStream out=new ByteArrayOutputStream();try{s.writeDataStream(new DataOutputStream(out));}catch(Exception e){r.put("writeError",error(e));}
        r.put("wireHash",hash(out.toByteArray()));r.put("wireLength",out.size());try{r.put("json",s.toJSON());}catch(Exception e){r.put("jsonError",error(e));}return r;
    }
    public static void main(String[] args) throws Exception {
        if(args.length>0&&args[0].equals("--demo")){FORS.main(new String[0]);return;}
        BufferedReader in=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=in.readLine())!=null) {
            JSONObject request=(JSONObject)JSONValue.parseWithException(line),result=new JSONObject();int[] logs={0};JSONArray kinds=new JSONArray();PrintStream original=System.out;
            System.setOut(new PrintStream(new ByteArrayOutputStream()){@Override public void println(String text){int pos=text.indexOf("] : java.");if(pos>=0){logs[0]++;if(kinds.size()<2)kinds.add(text.substring(pos+4).split(": ",2)[0]);}}});
            try {
                String action=(String)request.get("action");
                if(action.equals("init")){forest=new FORS(data(request.get("seed")));result.put("root",forest.getForsRoot().toJSON());result.put("rootIdentity",forest.getForsRoot()==forest.getForsRoot());}
                else if(action.equals("sign")){FORSSignature s=forest.signMessage(data(request.get("message")));result.put("wire",new MiniData(wire(s)).to0xString());result.put("json",s.toJSON());result.put("root",forest.getForsRoot().toJSON());}
                else if(action.equals("verify")){
                    FORSSignature s=request.get("wire")==null?null:readFORS((String)request.get("wire"));if(s!=null)mutate(s,request);
                    MMRData root=request.get("root")==null?null:new MMRData(data(request.get("root")),new MiniNumber((String)request.get("sum")));
                    result.put("valid",FORS.verifySignature(data(request.get("message")),s,root));
                }else if(action.equals("read")){
                    FORSSignature s=readFORS((String)request.get("initial"));var oldSig=s.getHORSTSignature();var oldRoots=s.getHORSTRoots();var oldProofs=s.getHORSTTreeProofs();byte[] bytes=data(request.get("wire")).getBytes();ByteArrayInputStream source=new ByteArrayInputStream(bytes);
                    try{s.readDataStream(new DataInputStream(source));}catch(Exception e){result.put("readError",error(e));}
                    result.put("position",bytes.length-source.available());result.put("sameSignature",oldSig==s.getHORSTSignature());result.put("sameRoots",oldRoots==s.getHORSTRoots());result.put("sameProofs",oldProofs==s.getHORSTTreeProofs());
                    result.put("oldValues",oldSig.getSignatureValues().size());result.put("oldRoots",oldRoots.size());result.put("oldProofs",oldProofs.size());result.put("state",snapshot(s));
                }else if(action.equals("convert"))result.put("state",snapshot(FORSSignature.convertMiniDataVersion(data(request.get("wire")))));
                else if(action.equals("mutate")){var s=readFORS((String)request.get("wire"));mutate(s,request);result.put("state",snapshot(s));}
                else if(action.equals("alias")){
                    var a=forest.signMessage(data(request.get("message")));var b=forest.signMessage(data(request.get("message")));boolean roots=true,secrets=true,proofs=true;
                    for(int i=0;i<16;i++){roots&=a.getHORSTRoots().get(i)==b.getHORSTRoots().get(i);secrets&=a.getHORSTSignature().getSignatureValues().get(i)==b.getHORSTSignature().getSignatureValues().get(i);proofs&=a.getHORSTTreeProofs().get(i)!=b.getHORSTTreeProofs().get(i);}
                    result.put("sharedRoots",roots);result.put("sharedSecrets",secrets);result.put("distinctProofs",proofs);result.put("distinctLists",a.getHORSTRoots()!=b.getHORSTRoots());
                    var list=a.getHORSTRoots();list.remove(0);result.put("aliasSize",a.getHORSTRoots().size());result.put("otherSize",b.getHORSTRoots().size());
                }else throw new IllegalArgumentException("unknown fixture action");
            }catch(Exception e){result.put("error",error(e));}finally{System.setOut(original);}
            result.put("logCount",logs[0]);result.put("logKinds",kinds);System.out.println(result.toJSONString());
        }
    }
}
