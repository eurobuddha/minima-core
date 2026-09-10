import java.io.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.keys.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Reuses WalletCommandsOracle's real owner/command and recording SQL fixture. */
public class TokenValidationOracle extends WalletCommandsOracle {
    static String wire(org.minima.utils.Streamable value){return MiniData.getMiniDataVersion(value).to0xString();}
    static JSONObject keys()throws Exception{
        var key=new TreeKey(new MiniData("0x"+"12".repeat(32)),2,2);var coin=new MiniData("0x91");
        var signature=key.sign(coin);var wrong=key.sign(new MiniData("0x92"));var longSignature=new Signature();
        for(int i=0;i<9;i++)longSignature.addSignatureProof(signature.getAllSignatureProofs().get(0));
        var out=new JSONObject();out.put("coinid",coin.to0xString());out.put("publickey",key.getPublicKey().to0xString());
        out.put("signature",wire(signature));out.put("wrong",wire(wrong));out.put("tooMany",wire(longSignature));out.put("empty",wire(new Signature()));
        out.put("nodes",((JSONObject)SearcherOracle.proofFixtures()).get("nodes"));return out;
    }
    static JSONArray tokens(JSONObject request){
        var out=new JSONArray();for(Object name:(JSONArray)request.get("names")){
            var token=new Token(new MiniData(request.getString("coinid","0x91")),MiniNumber.ZERO,MiniNumber.ONE,new MiniString((String)name),new MiniString("RETURN TRUE"));
            var coin=new Coin(new MiniData("0x81"),new MiniData("0x01"),MiniNumber.ONE,token.getTokenID());coin.setToken(token);
            var row=new JSONObject();row.put("wire",wire(token));row.put("id",token.getTokenID().to0xString());row.put("coin",wire(coin));out.add(row);
        }return out;
    }
    public static void main(String[] args)throws Exception{
        installDriver();org.minima.system.params.GeneralParams.SEED_PHRASE="fixture seed";org.minima.system.params.GeneralParams.ANYSEED_PHRASE=true;
        var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null){var request=(JSONObject)new JSONParser().parse(line);Object result;
            if("tokenValidationKeys".equals(request.get("kind")))result=keys();
            else if("tokenValidationTokens".equals(request.get("kind")))result=tokens(request);
            else result=runCommands(request,args[0]);System.out.println(JSONValue.toJSONString(result));
        }
    }
}
