import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.kissvm.*;
import org.minima.kissvm.values.*;
import org.minima.kissvm.tokens.*;
import org.minima.kissvm.expressions.*;
import org.minima.kissvm.exceptions.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.keys.*;
import org.minima.objects.mmr.*;
import org.minima.utils.BaseConverter;
import org.minima.utils.json.*;
public class KissVMOracle {
    static String str(String hex) {return new String(BaseConverter.decode16(hex),StandardCharsets.UTF_8);}
    static JSONArray value(Value value) {var out=new JSONArray();out.add(value.getValueType());out.add(value.toString());if(value instanceof NumberValue n) out.add(MiniData.getMiniDataVersion(n.getNumber()).to0xString());return out;}
    static Contract contract(String script,int max,boolean rich) {
        var txn=new Transaction();txn.addStateVariable(new StateVariable(0,"17"));txn.addStateVariable(new StateVariable(1,"[hello]"));
        var prev=new ArrayList<StateVariable>();prev.add(new StateVariable(0,"12"));prev.add(new StateVariable(1,"[old]"));
        var witness=new Witness();var signatures=new ArrayList<MiniData>();Token token=null;
        if(rich) {
            token=new Token(new MiniData("0x99"),new MiniNumber(2),MiniNumber.ONE,new MiniString("Token"),new MiniString("RETURN TRUE"));
            for(int i=0;i<3;i++) {
                MiniData id=i==0?Token.TOKENID_MINIMA:i==1?token.getTokenID():new MiniData("0x77");
                Coin coin=new Coin(new MiniData("0x1"+i),new MiniData("0x2"+i),new MiniNumber(i==0?"7":"0.25"),id,true);if(i==1) coin.setToken(token);txn.addInput(coin);
                Coin proofcoin=coin.deepCopy();proofcoin.resetCoinID(new MiniData("0xA"+i));witness.addCoinProof(new CoinProof(proofcoin,new MMRProof()));
                Coin output=new Coin(new MiniData("0x2"+i),new MiniNumber(i==0?"5":"0.125"),id,i==0);if(i==1) output.setToken(token);txn.addOutput(output);
            }
            signatures.add(new MiniData("0xCA"));signatures.add(new MiniData("0xFE"));witness.addScript(new ScriptProof("RETURN TRUE"));witness.addScript(new ScriptProof("LET x=42"));
        }
        var ctr=new Contract(script,signatures,witness,txn,prev);ctr.setMaxInstructions(max);
        if(rich) {ctr.setGlobals(new MiniNumber(123),new MiniNumber(987654321),txn,0,new MiniNumber(100),script);ctr.setVariable("tok",new HexValue(token.getTokenID()));}
        ctr.setVariable("a",new NumberValue(7));ctr.setVariable("b",new NumberValue(3));ctr.setVariable("t",BooleanValue.TRUE);ctr.setVariable("f",BooleanValue.FALSE);
        ctr.setVariable("h",new HexValue("0x00FF"));ctr.setVariable("s",new StringValue("hello"));
        ctr.setGlobalVariable("@BLOCK",new NumberValue(123));ctr.setGlobalVariable("@CREATED",new NumberValue(100));return ctr;
    }
    public static void main(String[] args) throws Exception {
        if(args.length>0&&args[0].equals("--fixtures")) {
            var fixtures=new JSONArray();TreeKey key=new TreeKey(new MiniData("0x000102"),2,2);MiniData message=new MiniData("0x1234");Signature sig=key.sign(message);String signature=MiniData.getMiniDataVersion(sig).to0xString();
            fixtures.add("CHECKSIG("+key.getPublicKey()+" "+message+" "+signature+")");fixtures.add("CHECKSIG("+key.getPublicKey()+" 0x1235 "+signature+")");fixtures.add("CHECKSIG(0x00 "+message+" "+signature+")");
            for(boolean hex:new boolean[]{true,false}) {
                MMR mmr=new MMR();for(int i=0;i<3;i++) {MMRData leaf=hex?MMRData.CreateMMRDataLeafNode(new MiniData("0x1122"),new MiniNumber(i+7)):MMRData.CreateMMRDataLeafNode(new MiniString("hello"),new MiniNumber(i+7));mmr.addEntry(leaf);}
                MMRData root=mmr.getRoot();MMRProof proof=mmr.getProof(new MMREntryNumber(1));String chain=MiniData.getMiniDataVersion(proof).to0xString();String value=hex?"0x1122":"[hello]";
                fixtures.add("PROOF("+value+" 8 "+root.getData()+" "+root.getValue()+" "+chain+")");fixtures.add("PROOF("+value+" 7 "+root.getData()+" "+root.getValue()+" "+chain+")");
            }
            fixtures.add("MAST "+new Address("RETURN TRUE").getAddressData());fixtures.add("MAST "+new Address("LET x=42").getAddressData()+" RETURN x EQ 42");
            System.out.println(BaseConverter.encode16(fixtures.toString().getBytes(StandardCharsets.UTF_8)));return;
        }
        var input=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=input.readLine())!=null) {
            var out=new JSONArray();
            try {
                var a=line.split(" ");var script=str(a[1]);var mode=a[0];int max=Integer.parseInt(a[2]);
                if(mode.equals("clean")) out.add(Contract.cleanScript(script));
                else {
                    var ctr=contract(mode.equals("run")?script:"",max,a.length>3&&a[3].equals("1"));int offset=ctr.getCompleteTraceLog().length();String error="";
                    try {
                        if(mode.equals("run")) ctr.run();
                        else {var expression=ExpressionParser.getExpression(ScriptToken.tokenize(script),0);out.add(expression.toString());out.add(value(expression.getValue(ctr)));}
                    }catch(MinimaException e) {error=e.toString();}catch(Exception e) {error="ERROR";}
                    String contractError=ctr.getException();if(!contractError.isEmpty()&&!contractError.startsWith("org.minima.kissvm.exceptions.")) contractError="ERROR";
                    out.add(ctr.isParseOK());out.add(ctr.isSuccess());out.add(ctr.isSuccessSet());out.add(ctr.isException());out.add(contractError);out.add(error);
                    out.add(ctr.getNumberOfInstructions());out.add(ctr.getStackDepth());out.add(ctr.isMonotonic());out.add(ctr.getAllVariables());
                    out.add(error.equals("ERROR")||contractError.equals("ERROR")?"":ctr.getCompleteTraceLog().substring(offset));
                }
            }catch(Exception error) {out.add("ERROR");}
            System.out.println(BaseConverter.encode16(out.toString().getBytes(StandardCharsets.UTF_8)));
        }
    }
}
