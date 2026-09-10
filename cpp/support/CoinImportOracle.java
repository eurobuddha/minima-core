import java.io.*;
import java.util.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class CoinImportOracle extends WalletCommandsOracle {
    static String wire(org.minima.utils.Streamable value)throws Exception{return MiniData.getMiniDataVersion(value).to0xString();}
    static JSONObject fixtures()throws Exception{
        JSONObject result=SearcherOracle.proofFixtures();JSONArray nodes=(JSONArray)result.get("nodes"),pruned=new JSONArray(),missing=new JSONArray();MMR parent=null;
        Coin coin=CoinProof.convertMiniDataVersion(new MiniData((String)((JSONArray)result.get("proofs")).get(0))).getCoin();
        for(int i=0;i<nodes.size();i++){
            JSONObject spec=(JSONObject)nodes.get(i);MMR mmr=MMR.ReadFromStream(ConsensusOracle.data(spec.getString("mmr")));mmr.setParent(parent);
            if(i==3){
                CoinProof proof=new CoinProof(coin,mmr.getProofToPeak(coin.getMMREntryNumber()));result.put("importProof",wire(proof));result.put("importJSON",proof.toJSON());result.put("coin",wire(coin));
                var future=new MMRProof(new MiniNumber(999));for(int j=0;j<proof.getMMRProof().getProofLength();j++)future.addProofChunk(proof.getMMRProof().getProofChunk(j));result.put("futureProof",wire(new CoinProof(coin,future)));
                var spent=CoinProof.convertMiniDataVersion(new MiniData(wire(proof)));spent.getCoin().setSpent(true);result.put("spentImport",wire(spent));
            }
            MMR peaks=new MMR(mmr);peaks.clearParent();peaks.setBlockTime(mmr.getBlockTime());
            JSONObject sparse=new JSONObject(spec);sparse.put("mmr",wire(peaks));sparse.put("coins",new JSONArray());sparse.put("relevant",new JSONArray());pruned.add(sparse);
            JSONObject gap=new JSONObject(sparse);if(i==3){TxPoW pow=TxPoW.convertMiniDataVersion(new MiniData(spec.getString("pow")));pow.setBlockNumber(new MiniNumber(333));pow.calculateTXPOWID();gap.put("pow",wire(pow));}missing.add(gap);parent=mmr;
        }
        result.put("pruned",pruned);result.put("missingTree",missing);return result;
    }
    public static void main(String[] args)throws Exception{
        installDriver();GeneralParams.SEED_PHRASE="fixture seed";GeneralParams.ANYSEED_PHRASE=true;
        var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null){JSONObject request=(JSONObject)new JSONParser().parse(line);System.out.println("coinImportFixtures".equals(request.get("kind"))?fixtures():runCommands(request,args[0]));}
    }
}
