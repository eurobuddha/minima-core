import java.io.*;
import java.nio.charset.StandardCharsets;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.utils.json.*;
import org.minima.utils.sphincs.TransactionTest;
public class TransactionTestOracle extends HORSTOracle {
    static Coin coin(JSONObject q) {
        if(Boolean.TRUE.equals(q.get("nullCoin")))return null;Coin base=ObjectOracle.coin(integer(q,"index",0));
        Coin c=new Coin(base.getCoinID(),q.containsKey("address")?data(q.get("address")):base.getAddress(),q.containsKey("amount")?new MiniNumber((String)q.get("amount")):base.getAmount(),q.containsKey("tokenID")?data(q.get("tokenID")):base.getTokenID(),q.containsKey("keep")?(boolean)q.get("keep"):base.storeState());
        c.setToken(base.getToken());if(q.containsKey("scale"))c.setToken(new Token(new MiniData("0x99"),new MiniNumber((String)q.get("scale")),MiniNumber.ONE,new MiniString("fixture"),new MiniString("RETURN TRUE")));return c;
    }
    public static void main(String[] args) throws Exception {
        if(args.length>0&&args[0].equals("--demo")){TransactionTest.main(new String[0]);return;}
        BufferedReader in=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=in.readLine())!=null){JSONObject q=(JSONObject)JSONValue.parseWithException(line),r=new JSONObject();ByteArrayOutputStream bytes=new ByteArrayOutputStream();PrintStream original=System.out;System.setOut(new PrintStream(bytes,true,StandardCharsets.UTF_8));
            try{if(q.get("action").equals("log"))TransactionTest.log((String)q.get("message"));else r.put("value",TransactionTest.getOutCoinString(coin(q)));}catch(Exception e){r.put("error",error(e));}finally{System.out.flush();System.setOut(original);}
            r.put("printed",bytes.toString(StandardCharsets.UTF_8));System.out.println(r.toJSONString());
        }
    }
}
