import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.utils.*;
import org.minima.utils.json.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.utils.sphincs.HORST.*;

/** Calls the unmodified pinned classes; request mutations are fixture controls. */
public class HORSTOracle {
    static HORST key;
    static MiniData data(Object s) {return s==null?null:new MiniData((String)s);}
    static int integer(JSONObject r,String name,int fallback) {return r.containsKey(name)?((Number)r.get(name)).intValue():fallback;}
    static JSONObject error(Exception e) {JSONObject r=new JSONObject();r.put("exception",e.getClass().getName());r.put("message",e instanceof NullPointerException||e instanceof EOFException||e.getMessage()==null?"":e.getMessage());return r;}
    static byte[] wire(Streamable s) throws Exception {ByteArrayOutputStream out=new ByteArrayOutputStream();s.writeDataStream(new DataOutputStream(out));return out.toByteArray();}
    static String hash(byte[] b) {return new MiniData(Crypto.getInstance().hashData(b)).to0xString();}
    static HORSTSignature read(String s) throws Exception {HORSTSignature v=new HORSTSignature();v.readDataStream(new DataInputStream(new ByteArrayInputStream(data(s).getBytes())));return v;}
    static void mutate(HORSTSignature s,JSONObject r) throws Exception {
        if(r.containsKey("values"))while(s.getSignatureValues().size()>integer(r,"values",0))s.getSignatureValues().remove(s.getSignatureValues().size()-1);
        if(r.containsKey("proofs"))while(s.getPublicKeyTreeProofs().size()>integer(r,"proofs",0))s.getPublicKeyTreeProofs().remove(s.getPublicKeyTreeProofs().size()-1);
        if(r.containsKey("nullValue"))s.getSignatureValues().set(integer(r,"nullValue",0),null);
        if(r.containsKey("nullProof"))s.getPublicKeyTreeProofs().set(integer(r,"nullProof",0),null);
        if(r.containsKey("value"))s.getSignatureValues().set(integer(r,"index",0),data(r.get("value")));
        if(r.containsKey("proof"))s.getPublicKeyTreeProofs().set(integer(r,"index",0),MMRProof.ReadFromStream(new DataInputStream(new ByteArrayInputStream(data(r.get("proof")).getBytes()))));
        if(r.containsKey("oversizeProof"))s.getPublicKeyTreeProofs().get(integer(r,"oversizeProof",0)).addProofChunk(false,new MMRData(new MiniData(new byte[65]),new MiniNumber(1)));
        if(r.containsKey("swap"))Collections.swap(s.getSignatureValues(),0,integer(r,"swap",0));
        if(Boolean.TRUE.equals(r.get("extra"))) {s.getSignatureValues().add(null);s.getPublicKeyTreeProofs().add(null);}
    }
    static JSONObject snapshot(HORSTSignature s) throws Exception {
        JSONObject result=new JSONObject();result.put("values",s.getSignatureValues().size());result.put("proofs",s.getPublicKeyTreeProofs().size());
        ByteArrayOutputStream out=new ByteArrayOutputStream();
        try{s.writeDataStream(new DataOutputStream(out));}catch(Exception e){result.put("writeError",error(e));}
        result.put("wireHash",hash(out.toByteArray()));result.put("wireLength",out.size());
        try{result.put("json",s.toJSON());}catch(Exception e){result.put("jsonError",error(e));}
        return result;
    }
    public static void main(String[] args) throws Exception {
        if(args.length>0&&args[0].equals("--demo")){HORST.main(new String[0]);return;}
        BufferedReader input=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=input.readLine())!=null) {
            JSONObject request=(JSONObject)JSONValue.parseWithException(line),result=new JSONObject();
            int[] logs={0};JSONArray kinds=new JSONArray();PrintStream original=System.out;
            System.setOut(new PrintStream(new ByteArrayOutputStream()){@Override public void println(String text){int pos=text.indexOf("] : java.");if(pos>=0){logs[0]++;if(kinds.size()<2)kinds.add(text.substring(pos+4).split(": ",2)[0]);}}});
            try {
                String action=(String)request.get("action");
                if(action.equals("init")) {
                    key=new HORST(data(request.get("seed")));result.put("root",key.getHORSTRoot().toJSON());result.put("privateSize",key.getPrivateKey().getSize());result.put("publicSize",key.getPublicKey().getSize());
                    ByteArrayOutputStream priv=new ByteArrayOutputStream(),pub=new ByteArrayOutputStream();
                    for(int i=0;i<key.getPrivateKey().getSize();i++){priv.write(key.getPrivateKey().getKey(i).getBytes());pub.write(key.getPublicKey().getKey(i).getBytes());}
                    result.put("privateHash",hash(priv.toByteArray()));result.put("publicHash",hash(pub.toByteArray()));result.put("rootIdentity",key.getHORSTRoot()==key.getPublicKey().getPublicKeyTreeRoot());
                } else if(action.equals("key")) {
                    int i=integer(request,"index",0);String kind=(String)request.get("kind");
                    if(kind.equals("private")){result.put("data",key.getPrivateKey().getKey(i).to0xString());result.put("identity",key.getPrivateKey().getKey(i)==key.getPrivateKey().getKey(i));}
                    else if(kind.equals("public"))result.put("data",key.getPublicKey().getKey(i).to0xString());
                    else result.put("wire",new MiniData(wire(key.getPublicKey().getKeyTreeProof(i))).to0xString());
                } else if(action.equals("ref"))result.put("ref",HORST.getKeyRef(integer(request,"index",0),data(request.get("data"))));
                else if(action.equals("sign")) {
                    HORSTSignature s=key.signMessage(data(request.get("message")));result.put("wire",new MiniData(wire(s)).to0xString());result.put("root",key.getHORSTRoot().toJSON());result.put("json",s.toJSON());
                    MiniData hm=new MiniData(Crypto.getInstance().hashData(data(request.get("message")).getBytes()));boolean identity=true;
                    for(int i=0;i<16;i++)identity&=s.getSignatureValues().get(i)==key.getPrivateKey().getKey(HORST.getKeyRef(i,hm));result.put("identity",identity);
                } else if(action.equals("verify")) {
                    HORSTSignature s=request.get("wire")==null?null:read((String)request.get("wire"));if(s!=null)mutate(s,request);
                    MMRData root=request.get("root")==null?null:new MMRData(data(request.get("root")),new MiniNumber((String)request.get("sum")));
                    result.put("valid",HORST.verifySignature(data(request.get("message")),s,root));
                } else if(action.equals("read")) {
                    HORSTSignature s=read((String)request.get("initial"));var oldValues=s.getSignatureValues();var oldProofs=s.getPublicKeyTreeProofs();
                    byte[] bytes=data(request.get("wire")).getBytes();ByteArrayInputStream in=new ByteArrayInputStream(bytes);
                    try{s.readDataStream(new DataInputStream(in));}catch(Exception e){result.put("readError",error(e));}
                    result.put("position",bytes.length-in.available());result.put("oldValues",oldValues.size());result.put("oldProofs",oldProofs.size());
                    result.put("sameValues",oldValues==s.getSignatureValues());result.put("sameProofs",oldProofs==s.getPublicKeyTreeProofs());result.put("state",snapshot(s));
                } else if(action.equals("convert"))result.put("state",snapshot(HORSTSignature.convertMiniDataVersion(data(request.get("wire")))));
                else if(action.equals("mutate")) {HORSTSignature s=read((String)request.get("wire"));mutate(s,request);result.put("state",snapshot(s));}
                else if(action.equals("alias")) {
                    HORSTSignature s=key.signMessage(data(request.get("message")));var values=s.getSignatureValues();var proofs=s.getPublicKeyTreeProofs();
                    MMRProof proof=proofs.get(0);proof.addProofChunk(false,new MMRData(new MiniData("0x1122"),new MiniNumber(3)));
                    result.put("proofIdentity",proofs.get(0)==proof);result.put("proofLength",s.getPublicKeyTreeProofs().get(0).getProofLength());
                    values.remove(0);proofs.remove(0);result.put("state",snapshot(s));
                } else if(action.equals("publicNull"))new PublicKey(null);
                else if(action.equals("privateNull")) {PrivateKey p=new PrivateKey(null);result.put("size",p.getSize());result.put("firstNull",p.getKey(0)==null);result.put("lastNull",p.getKey(65535)==null);}
                else throw new IllegalArgumentException("unknown fixture action");
            } catch(Exception e) {result.put("error",error(e));}
            finally {System.setOut(original);}
            result.put("logCount",logs[0]);result.put("logKinds",kinds);System.out.println(result.toJSONString());
        }
    }
}
