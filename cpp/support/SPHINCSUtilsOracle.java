import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.utils.json.*;
import org.minima.utils.sphincs.*;
import org.minima.kissvm.Contract;
import org.minima.kissvm.values.NumberValue;

public class SPHINCSUtilsOracle extends FORSOracle {
    static Transaction transaction(JSONObject q) {
        if(Boolean.TRUE.equals(q.get("nullTransaction")))return null;
        Transaction t=new Transaction();for(Object n:(JSONArray)q.get("inputs"))t.addInput(ObjectOracle.coin(((Number)n).intValue()));
        for(Object n:(JSONArray)q.get("outputs"))t.addOutput(ObjectOracle.coin(((Number)n).intValue()));
        if(Boolean.TRUE.equals(q.get("eltoo"))&&!t.getAllInputs().isEmpty())t.getAllInputs().get(0).resetCoinID(Coin.COINID_ELTOO);
        if(Boolean.TRUE.equals(q.get("oldState"))){for(int i=0;i<80;i++)t.addStateVariable(new StateVariable(i,"[old]"));t.addStateVariable(new StateVariable(100,"[old]"));t.addStateVariable(new StateVariable(101,"[old]"));t.addStateVariable(new StateVariable(200,"[keep]"));}
        return t;
    }
    static JSONObject snapshot(Transaction t) throws Exception {
        JSONObject r=new JSONObject();if(t==null){r.put("null",true);return r;}r.put("cachedID",t.getTransactionID().to0xString());
        JSONArray states=new JSONArray(),ids=new JSONArray();for(var v:t.getCompleteState())states.add(v.toJSON());for(var c:t.getAllOutputs())ids.add(c.getCoinID().to0xString());
        r.put("state",states);r.put("outputIDs",ids);r.put("wire",new MiniData(wire(t)).to0xString());return r;
    }
    static JSONObject run(Transaction t,MiniData publicKey,JSONObject q) {
        if(q.containsKey("changeState"))t.addStateVariable(new StateVariable(integer(q,"changeState",0),"0x00"));
        if(Boolean.TRUE.equals(q.get("changeInput")))t.getAllInputs().get(0).resetCoinID(new MiniData("0xAB"));
        if(Boolean.TRUE.equals(q.get("changeOutput")))t.getAllOutputs().get(0).setStoreState(!t.getAllOutputs().get(0).storeState());
        Witness w=new Witness();for(Coin c:t.getAllInputs())w.addCoinProof(new CoinProof(c,new MMRProof()));
        Contract c=new Contract(SPHINCSUtils.getKISSVMScript(publicKey),new ArrayList<MiniData>(),w,t,new ArrayList<StateVariable>(),false);
        c.setGlobalVariable("@TOTIN",new NumberValue(t.getAllInputs().size()));c.setGlobalVariable("@TOTOUT",new NumberValue(t.getAllOutputs().size()));c.run();
        JSONObject r=new JSONObject();r.put("valid",c.isSuccess());r.put("parse",c.isParseOK());r.put("exception",c.isException());r.put("error",c.getException());r.put("operations",c.getNumberOfInstructions());r.put("variables",c.getAllVariables());return r;
    }
    public static void main(String[] args) throws Exception {
        BufferedReader in=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=in.readLine())!=null){JSONObject q=(JSONObject)JSONValue.parseWithException(line),r=new JSONObject();Transaction t=null;int[] logs={0};PrintStream original=System.out;
            System.setOut(new PrintStream(new ByteArrayOutputStream()){@Override public void println(String text){if(text.contains("] : java."))logs[0]++;}});
            try {
                String action=(String)q.get("action");
                if(action.equals("script")){r.put("constant",SPHINCSUtils.KISSVM_SPHINCS_SCRIPT);r.put("script",SPHINCSUtils.getKISSVMScript(data(q.get("public"))));}
                else {
                    t=transaction(q);
                    if(action.equals("message"))r.put("message",SPHINCSUtils.calculateTransactionID(t).to0xString());
                    else {
                        SPHINCSSignature s=null;MiniData publicKey=data(q.get("public"));
                        if(action.equals("signSetup")){SPHINCS signer=new SPHINCS(data(q.get("seed")));MiniData message=SPHINCSUtils.calculateTransactionID(t);s=signer.signMessage(message);publicKey=signer.getPublicKey();r.put("message",message.to0xString());r.put("public",publicKey.to0xString());r.put("signature",new MiniData(wire(s)).to0xString());}
                        else if(q.get("wire")!=null){s=new SPHINCSSignature();s.readDataStream(new DataInputStream(new ByteArrayInputStream(data(q.get("wire")).getBytes())));}
                        if(s!=null){if(q.containsKey("mutation"))mutate(s.getFORSSignature(),(JSONObject)q.get("mutation"));int mask=integer(q,"nulls",0);s=new SPHINCSSignature((mask&1)!=0?null:s.getWOTSSignature(),(mask&2)!=0?null:s.getFORSRoot(),(mask&4)!=0?null:s.getFORSSignature());}
                        SPHINCSUtils.setupTransaction(t,s);
                        if(action.equals("run")||action.equals("signSetup"))r.put("contract",run(t,publicKey,q));
                    }
                }
            }catch(Exception e){r.put("error",error(e));}
            try{r.put("transaction",snapshot(t));}catch(Exception e){r.put("snapshotError",error(e));}finally{System.setOut(original);}
            r.put("logCount",logs[0]);System.out.println(r.toJSONString());
        }
    }
}
