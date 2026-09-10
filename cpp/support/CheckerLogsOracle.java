import java.io.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.system.brains.TxPoWChecker;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Reuses ConsensusOracle's exact transaction fixtures; actual checker calls. */
public class CheckerLogsOracle extends NIOClientOracle {
    static JSONArray logs;static int prints,failAt;static String kind;
    static void fail(){switch(kind){case "error":throw new LinkageError("logger");case "oom":throw new OutOfMemoryError("logger");default:throw new IllegalStateException("logger");}}
    static JSONObject run(JSONObject input)throws Exception{
        String[] wires=input.getString("wire").split(" ");var mmr=MMR.ReadFromStream(ConsensusOracle.data(wires[0]));var tx=ConsensusOracle.txpow(wires[1]);var block=ConsensusOracle.txpow(wires[2]);
        String method=input.getString("method");kind=input.getString("kind");failAt=((Number)input.get("failAt")).intValue();prints=0;logs=new JSONArray();GeneralParams.SCRIPTLOGS=Boolean.TRUE.equals(input.get("trace"));
        if(Boolean.TRUE.equals(input.get("badMMR")))mmr=new MMR();if(Boolean.TRUE.equals(input.get("nullMMR")))mmr=null;
        if(input.containsKey("maxSize"))block.getMagic().mCurrentMaxTxPoWSize=new MiniNumber(input.getString("maxSize"));
        var old=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String full){int at=full.indexOf("] : ");if(at<0)return;String body=full.substring(at+4);if(body.startsWith("     "))return;logs.add(body);if(++prints==failAt)fail();}
        });Object result;
        try{result=switch(method){case "basic"->TxPoWChecker.checkTxPoWBasic(tx);case "simple"->TxPoWChecker.checkTxPoWSimple(mmr,tx,block,Boolean.TRUE.equals(input.get("log")));case "scripts"->TxPoWChecker.checkTxPoWScripts(mmr,tx,block);case "mmr"->TxPoWChecker.checkMMR(mmr,tx,Boolean.TRUE.equals(input.get("log")));case "signatures"->TxPoWChecker.checkSignatures(tx);default->throw new IllegalArgumentException(method);};}catch(Throwable e){result="escaped:"+e;}finally{System.setOut(old);}
        var out=new JSONObject();out.put("result",result);out.put("logs",logs);try{out.put("flags",ConsensusOracle.flags(tx));out.put("txid",tx.getTransaction().getTransactionID().to0xString());}catch(Throwable e){out.put("snapshotError",e.getClass().getName());}return out;
    }
    public static void main(String[] args)throws Exception{
        if(args.length>0){
            var output=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()));var rows=new JSONArray();
            for(int mode=0;mode<30;mode++)for(String script:new String[]{"RETURN TRUE","RETURN FALSE","RETURN @BLKNUM EQ 19"}){
                var row=new JSONObject();row.put("mode",mode);row.put("script",script);row.put("wire",ConsensusOracle.fixture(mode,script,3,1024));rows.add(row);
            }
            // Exercise HashSet(Collection) capacity transitions and both witnesses,
            // retaining an actual source transaction so the uniqueness gate runs.
            for(int count:new int[]{1,10,11,12,23,24,47,48,64}){
                var wires=ConsensusOracle.fixture(0,"RETURN TRUE",3,1024).split(" ");var tx=ConsensusOracle.txpow(wires[1]);
                var original=tx.getWitness().getAllCoinProofs().get(0);tx.getWitness().getAllCoinProofs().clear();
                for(int i=0;i<count;i++){
                    var coin=original.getCoin().deepCopy();coin.resetCoinID(new MiniData(String.format("0x%08X",i*7919+17)));
                    (i%2==0?tx.getWitness():tx.getBurnWitness()).addCoinProof(new CoinProof(coin,original.getMMRProof()));
                }
                tx.getBurnWitness().addCoinProof(tx.getWitness().getAllCoinProofs().get(0));tx.setHeaderBodyHash();tx.calculateTXPOWID();
                var row=new JSONObject();row.put("mode",30);row.put("script","duplicate IDs "+count);row.put("wire",wires[0]+" "+ConsensusOracle.wire(tx)+" "+wires[2]);rows.add(row);
            }
            var wires=ConsensusOracle.fixture(0,"RETURN TRUE",3,1024).split(" ");var tx=ConsensusOracle.txpow(wires[1]);var input=tx.getTransaction().getAllInputs().get(0);
            tx.getTransaction().getAllInputs().set(0,new Coin(input.getCoinID(),input.getAddress(),new MiniNumber("1000000001"),input.getTokenID()));tx.setHeaderBodyHash();tx.calculateTXPOWID();
            var row=new JSONObject();row.put("mode",31);row.put("script","invalid input amount");row.put("wire",wires[0]+" "+ConsensusOracle.wire(tx)+" "+wires[2]);rows.add(row);
            output.println(rows.toJSONString());return;
        }
        var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)System.out.println(run((JSONObject)new JSONParser().parse(line)).toJSONString());
    }
}
