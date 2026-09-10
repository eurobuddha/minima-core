import java.io.*;
import java.util.*;
import java.lang.reflect.*;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import org.minima.database.*;
import org.minima.database.txpowdb.*;
import org.minima.database.txpowtree.*;
import org.minima.database.wallet.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.system.brains.*;
import org.minima.system.params.*;
import org.minima.system.commands.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.*;
public class SearcherOracle {
    static void set(Object target,String name,Object value)throws Exception{Field field=target.getClass().getDeclaredField(name);field.setAccessible(true);field.set(target,value);}
    static class Pool extends TxPoWDB {HashMap<String,TxPoW> values=new HashMap<>();public TxPoW getTxPoW(String id){return values.get(id);}}
    static int num(JSONObject object,String name){return ((Number)object.get(name)).intValue();}
    static MiniData hex(JSONObject object,String name){return new MiniData((String)object.get(name));}
    static Object fixtures(){var result=new JSONObject();var tokens=new JSONArray();var tokenids=new JSONArray();var tokenlist=new ArrayList<Token>();for(int i=0;i<3;i++){var token=new Token(new MiniData("0xa"+i),new MiniNumber(i),new MiniNumber(10),new MiniString("token"+i),new MiniString("RETURN TRUE"),new MiniNumber(i));tokenlist.add(token);tokenids.add(token.getTokenID().to0xString());tokens.add(MiniData.getMiniDataVersion(token).to0xString());}result.put("tokens",tokens);result.put("tokenids",tokenids);var coins=new JSONArray();for(int i=0;i<32;i++){Token token=i%4==0?null:tokenlist.get(i%4-1);var coin=new Coin(new MiniData("0x"+Integer.toHexString(i%8+1)),new MiniData("0x0"+(i%3+1)),new MiniNumber(i%5),token==null?Token.TOKENID_MINIMA:token.getTokenID());coin.setToken(token);coin.setSpent((i/8)%2==1);coin.setMMREntryNumber(new MMREntryNumber(i));coin.setBlockCreated(new MiniNumber(i%7));var state=new ArrayList<StateVariable>();state.add(new StateVariable(0,"[state"+(i%3)+"]"));state.add(new StateVariable(1,"0x0"+(i%3+1)));coin.setState(state);coins.add(MiniData.getMiniDataVersion(coin).to0xString());}result.put("coins",coins);var pow=new JSONArray();var ids=new JSONArray();for(int i=0;i<8;i++){var txpow=ArchiveCacheOracle.txpow(i,0);txpow.getTransaction().addInput(Coin.convertMiniDataVersion(new MiniData((String)coins.get(i))));txpow.getBurnTransaction().addInput(Coin.convertMiniDataVersion(new MiniData((String)coins.get(i+8))));txpow.getTransaction().addOutput(Coin.convertMiniDataVersion(new MiniData((String)coins.get(i+16))));txpow.getBurnTransaction().addOutput(Coin.convertMiniDataVersion(new MiniData((String)coins.get(i+24))));txpow.getTransaction().addStateVariable(new StateVariable(2,"0x0"+(i%3+1)));txpow.setHeaderBodyHash();txpow.calculateTXPOWID();pow.add(MiniData.getMiniDataVersion(txpow).to0xString());ids.add(txpow.getTxPoWID());}result.put("pow",pow);result.put("ids",ids);return result;}
    static JSONObject proofFixtures()throws Exception{return proofFixtures(null);}
    static JSONObject proofFixtures(Token fundingToken)throws Exception{return proofFixtures(fundingToken,null);}
    static JSONObject proofFixtures(Token fundingToken,JSONObject options)throws Exception{
        var result=new JSONObject();var nodes=new JSONArray();var proofs=new JSONArray();var proofids=new JSONArray();var allcoins=new ArrayList<Coin>();MMR parent=null;
        for(int i=0;i<12;i++){
            var mmr=new MMR(parent);mmr.setBlockTime(new MiniNumber(i));var coins=new JSONArray();
            if(i<4){var coin=new Coin(new MiniData("0x"+Integer.toHexString(101+i)),new MiniData(options==null?"0x01":options.getString("address")),new MiniNumber(i+1),fundingToken!=null&&i==0?fundingToken.getTokenID():Token.TOKENID_MINIMA);if(i==0)coin.setToken(fundingToken);if(options!=null){var states=new ArrayList<StateVariable>();for(Object entry:(JSONArray)options.get("state")){var spec=(JSONObject)entry;states.add(new StateVariable(((Number)spec.get("port")).intValue(),spec.getString("data")));}coin.setState(states);}coin.setMMREntryNumber(mmr.getEntryNumber());coin.setBlockCreated(new MiniNumber(i));mmr.addEntry(MMRData.CreateMMRDataLeafNode(coin,coin.getAmount()));allcoins.add(coin);coins.add(MiniData.getMiniDataVersion(coin).to0xString());proofids.add(coin.getCoinID().to0xString());proofs.add(MiniData.getMiniDataVersion(new CoinProof(coin,mmr.getProofToPeak(coin.getMMREntryNumber()))).to0xString());}
            if(i==6){var coin=allcoins.get(1).deepCopy();coin.setSpent(true);mmr.updateEntry(coin.getMMREntryNumber(),parent.getProof(coin.getMMREntryNumber()),MMRData.CreateMMRDataLeafNode(coin,MiniNumber.ZERO));coins.add(MiniData.getMiniDataVersion(coin).to0xString());result.put("spentproof",MiniData.getMiniDataVersion(new CoinProof(coin,mmr.getProofToPeak(coin.getMMREntryNumber()))).to0xString());}
            var node=new JSONObject();node.put("pow",MiniData.getMiniDataVersion(ArchiveCacheOracle.txpow(i,0)).to0xString());node.put("coins",coins);node.put("relevant",coins);node.put("txns",new JSONArray());node.put("mmr",MiniData.getMiniDataVersion(mmr).to0xString());nodes.add(node);parent=mmr;
        }
        result.put("nodes",nodes);result.put("proofs",proofs);result.put("ids",proofids);return result;
    }
    static Object coin(Coin coin){return coin==null?null:MiniData.getMiniDataVersion(coin).to0xString();}
    static Object token(Token token){return token==null?null:MiniData.getMiniDataVersion(token).to0xString();}
    static JSONArray coins(ArrayList<Coin> coins){var result=new JSONArray();for(var c:coins)result.add(coin(c));return result;}
    static Object run(JSONObject request)throws Exception {
        if("fixtures".equals(request.get("kind")))return fixtures();
        if("prooffixtures".equals(request.get("kind")))return proofFixtures();
        MinimaDB.createDB();GeneralParams.USE_SQL_COINDB=false;GeneralParams.TEST_PARAMS=Boolean.TRUE.equals(request.get("testparams"));GeneralParams.IS_MEGAMMR=(boolean)request.get("mega");GeneralParams.DB_IGNORE_LOCKS=false;var db=MinimaDB.getDB();var lockfield=MinimaDB.class.getDeclaredField("mRWLock");lockfield.setAccessible(true);var lock=(ReentrantReadWriteLock)lockfield.get(db);
        var imported=TxPoWSearcher.class.getDeclaredField("mImportedTokens");imported.setAccessible(true);((ArrayList<?>)imported.get(null)).clear();
        var nodes=new ArrayList<TxPoWTreeNode>();var pool=new Pool();set(db,"mTxPoWDB",pool);
        for(Object value:(JSONArray)request.get("pool")){var t=TxPoW.convertMiniDataVersion(new MiniData((String)value));pool.values.put(t.getTxPoWID(),t);}
        for(Object value:(JSONArray)request.get("nodes")){var spec=(JSONObject)value;var t=TxPoW.convertMiniDataVersion(hex(spec,"pow"));for(Object id:(JSONArray)spec.get("txns"))t.getBlockTransactions().add(new MiniData((String)id));t.setHeaderBodyHash();t.calculateTXPOWID();var node=new TxPoWTreeNode(t);if(spec.containsKey("mmr"))node.getMMR().readDataStream(new DataInputStream(new ByteArrayInputStream(hex(spec,"mmr").getBytes())));for(Object c:(JSONArray)spec.get("coins"))node.addCoinToNode(Coin.convertMiniDataVersion(new MiniData((String)c)));for(Object c:(JSONArray)spec.get("relevant"))node.getRelevantCoins().add(Coin.convertMiniDataVersion(new MiniData((String)c)));if(!nodes.isEmpty())nodes.getLast().addChildNode(node);nodes.add(node);}
        var tree=db.getTxPoWTree();set(tree,"mTip",nodes.isEmpty()?null:nodes.getLast());
        for(Object value:(JSONArray)request.get("megacoins")){var c=Coin.convertMiniDataVersion(new MiniData((String)value));db.getMegaMMR().getAllCoins().put(c.getCoinID().to0xString(),c);}
        for(Object value:(JSONArray)request.get("tokens"))TxPoWSearcher.importToken(value==null?null:Token.convertMiniDataVersion(new MiniData((String)value)));
        var wallet=db.getWallet();for(var pair:new String[][]{{"mAllTrackedAddress","addresses"},{"mAllSimpleAddress","simple"},{"mAllKeys","keys"}}){var field=Wallet.class.getDeclaredField(pair[0]);field.setAccessible(true);var values=(HashSet<String>)field.get(wallet);for(Object value:(JSONArray)request.get(pair[1]))values.add((String)value);}
        var result=new JSONArray();for(Object value:(JSONArray)request.get("queries")){var q=(JSONObject)value;Object response;try{String op=(String)q.get("op");var start=num(q,"start")<0?null:nodes.get(num(q,"start"));
            if(q.containsKey("nodeid"))q.put("id",nodes.get(num(q,"nodeid")).getTxPoW().getTxPoWID());
            switch(op){
            case "command":{var command=CommandRunner.getRunner().getCommandOnly((String)q.get("name"));command.getParams().putAll((JSONObject)q.get("params"));if(Boolean.TRUE.equals(q.get("meta"))){var meta=new JSONObject();meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("valid",command.getValidParams());meta.put("fresh",command.getFunction().getJSONReply());response=meta;}else response=command.runCommand();break;}
            case "coins":response=coins(TxPoWSearcher.searchCoins(start,(boolean)q.get("relevant"),(boolean)q.get("cid"),hex(q,"id"),(boolean)q.get("cam"),new MiniNumber((String)q.get("amount")),(boolean)q.get("caddr"),hex(q,"address"),(boolean)q.get("ctok"),hex(q,"token"),(boolean)q.get("cstate"),(String)q.get("state"),(boolean)q.get("wild"),(boolean)q.get("simple"),num(q,"depth"),(boolean)q.get("mega")));break;
            case "relevant":response=coins(TxPoWSearcher.getAllRelevantUnspentCoins(start));break;
            case "relevanttoken":response=coins(TxPoWSearcher.getRelevantUnspentCoins(start,(String)q.get("token"),(boolean)q.get("simple")));break;
            case "floating":response=coin(TxPoWSearcher.getFloatingCoin(start,new MiniNumber((String)q.get("amount")),hex(q,"address"),hex(q,"token")));break;
            case "coin":response=coin(TxPoWSearcher.searchCoin(hex(q,"id"),(boolean)q.get("mega")));break;
            case "coindefault":response=coin(TxPoWSearcher.searchCoin(hex(q,"id")));break;
            case "nodecoin":response=nodes.indexOf(TxPoWSearcher.getTreeNodeForCoin(hex(q,"id")));break;
            case "block":{var t=TxPoWSearcher.getTxPoWBlock(new MiniNumber((String)q.get("amount")));response=t==null?null:t.getTxPoWID();break;}
            case "chain":{var t=TxPoWSearcher.searchChainForTxPoW(hex(q,"id"));response=t==null?null:t.getTxPoWID();break;}
            case "nodeblock":response=nodes.indexOf(TxPoWSearcher.searchChainForTxPoWBlock(hex(q,"id")));break;
            case "address":{var ids=new JSONArray();for(var t:TxPoWSearcher.searchTxPoWviaAddress(hex(q,"address")))ids.add(t.getTxPoWID());response=ids;break;}
            case "checkaddress":response=TxPoWSearcher.checkTxPoWForAddress(start.getTxPoW(),hex(q,"address"));break;
            case "checkrelevant":response=TxPoWSearcher.checkTxPoWRelevant(start.getTxPoW(),wallet);break;
            case "tokens":{var all=new JSONArray();for(var t:TxPoWSearcher.getAllTokens())all.add(token(t));response=all;break;}
            case "token":response=token(TxPoWSearcher.getToken(hex(q,"token")));break;
            default:throw new IllegalArgumentException(op);}
        }catch(CommandException error){response="COMMAND_ERROR:"+error.getMessage();}catch(Exception error){response="ERROR";}var output=new JSONObject();output.put("result",response);output.put("locks",lock.getReadHoldCount());while(lock.getReadHoldCount()>0)db.readLock(false);result.add(output);}return result;
    }
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));var output=System.out;String line;while((line=input.readLine())!=null)try{System.setOut(System.err);output.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace(System.err);output.println("\"ERROR\"");}finally{System.setOut(output);}}
}
