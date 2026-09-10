import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.commands.*;
import org.minima.system.commands.send.wallet.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class WalletWrappersOracle {
    static JSONObject config;static JSONArray calls,logs;static TxnDB db;
    static JSONArray rows(){var result=new JSONArray();for(var row:db.listTxns()){var item=new JSONObject();item.put("id",row.getID());item.put("wire",MiniData.getMiniDataVersion(row).to0xString());if(Boolean.TRUE.equals(config.get("materialize")))item.put("txn",row.getTransaction().toJSON());result.add(item);}return result;}
    static JSONObject child(String text){
        var call=new JSONObject();call.put("text",text);call.put("rows",rows());calls.add(call);
        int index=calls.size();
        if(config.containsKey("throwAt")&&((Long)config.get("throwAt")).intValue()==index)throw new RuntimeException("injected child failure");
        if(config.containsKey("resultAt")&&((Long)config.get("resultAt")).intValue()==index)return (JSONObject)config.get("result");
        if(Boolean.TRUE.equals(config.get("materialize"))){
            if(text.startsWith("txncreate ")||text.startsWith("txndelete "))CommandRunner.getRunner().runMultiCommand(text);
            if(text.startsWith("txnaddamount ")){
                String id=text.substring(16).split(" ")[0];
                for(Object value:(JSONArray)config.get("fundedOutputs")){
                    JSONObject output=(JSONObject)value;
                    CommandRunner.getRunner().runMultiCommand("txnoutput id:"+id+" address:"+output.get("address")+" amount:"+output.get("amount"));
                }
            }
        }
        if(Boolean.TRUE.equals(config.get("real"))||(config.get("execute") instanceof JSONArray execute && execute.contains(text.split(" ")[0])))return (JSONObject)CommandRunner.getRunner().runMultiCommand(text).get(0);
        var result=new JSONObject();result.put("status",true);var response=new JSONObject();response.put("marker","child"+index);response.put("txpow","fixture-pow");result.put("response",response);return result;
    }
    static class Consolidate extends consolidatefrom {public Command getFunction(){return new Consolidate();}public JSONObject runCommand(String text){return child(text);}}
    static class Send extends sendfrom {public Command getFunction(){return new Send();}public JSONObject runCommand(String text){return child(text);}}
    static class CreateToken extends createtokenfrom {public Command getFunction(){return new CreateToken();}public JSONObject runCommand(String text){return child(text);}}
    static class Raw extends rawfrom {public Command getFunction(){return new Raw();}public JSONObject runCommand(String text){return child(text);}}
    static class RawTxn extends rawtxnfrom {public Command getFunction(){return new RawTxn();}public JSONObject runCommand(String text){return child(text);}}
    static class Create extends createfrom {public Command getFunction(){return new Create();}public JSONObject runCommand(String text){return child(text);}}
    static class Construct extends constructfrom {public Command getFunction(){return new Construct();}public JSONObject runCommand(String text){return child(text);}}
    static class Post extends postfrom {public Command getFunction(){return new Post();}public JSONObject runCommand(String text){return child(text);}}
    static class Sign extends signfrom {public Command getFunction(){return new Sign();}public JSONObject runCommand(String text){return child(text);}}
    static Command command(String name){return switch(name){case "consolidatefrom"->new Consolidate();case "sendfrom"->new Send();case "createtokenfrom"->new CreateToken();case "rawfrom"->new Raw();case "rawtxnfrom"->new RawTxn();case "createfrom"->new Create();case "constructfrom"->new Construct();case "postfrom"->new Post();case "signfrom"->new Sign();default->throw new IllegalArgumentException(name);};}
    static Coin fixtureCoin(Object value){
        if(value==null)return null;var spec=(JSONObject)value;
        var coin=new Coin(new MiniData(spec.getString("id")),new MiniData(spec.getString("address")),new MiniNumber(spec.getString("amount")),new MiniData(spec.containsKey("token")?spec.getString("token"):"0x00"),true);
        coin.setBlockCreated(new MiniNumber(spec.containsKey("created")?spec.getString("created"):"0"));
        if(spec.containsKey("spent"))coin.setSpent((boolean)spec.get("spent"));
        if(spec.containsKey("scale"))coin.setToken(new Token(Coin.COINID_OUTPUT,new MiniNumber(spec.getString("scale")),MiniNumber.ONE,new MiniString("fixture"),new MiniString("RETURN TRUE")));
        return coin;
    }
    static void fixtureChain(){
        org.minima.system.params.GeneralParams.USE_SQL_COINDB=false;org.minima.system.params.GeneralParams.IS_MEGAMMR=Boolean.TRUE.equals(config.get("mega"));
        if(config.containsKey("chainCoins")){
            var pow=new TxPoW("0x10",config.containsKey("height")?((Long)config.get("height")).intValue():20,1);
            var node=new org.minima.database.txpowtree.TxPoWTreeNode(pow);
            for(Object value:(JSONArray)config.get("chainCoins"))node.addCoinToNode(fixtureCoin(value));
            MinimaDB.getDB().getTxPoWTree().setRoot(node);
        }
        if(config.containsKey("megaCoins"))for(Object value:(JSONArray)config.get("megaCoins")){
            var coin=fixtureCoin(value);MinimaDB.getDB().getMegaMMR().getAllCoins().put(coin.getCoinID().to0xString(),coin);
        }
        if(config.containsKey("mempool")){
            var pow=new TxPoW();
            for(Object value:(JSONArray)config.get("mempool")){var coin=fixtureCoin(value);pow.getTransaction().addInput(coin);pow.getWitness().addCoinProof(new CoinProof(coin,new org.minima.objects.mmr.MMRProof()));}
            var data=new org.minima.database.txpowdb.ram.RamData(pow);data.setInCascade(Boolean.TRUE.equals(config.get("poolCascade")));data.setOnMainChain(Boolean.TRUE.equals(config.get("poolOnChain")));
            MinimaDB.getDB().getTxPoWDB().getCompleteMemPool().put("fixture",data);
        }
    }
    static JSONObject run(JSONObject input)throws Exception {
        config=input;calls=new JSONArray();logs=new JSONArray();MinimaDB.createDB();db=new TxnDB();var field=MinimaDB.class.getDeclaredField("mTxnDB");field.setAccessible(true);field.set(MinimaDB.getDB(),Boolean.TRUE.equals(input.get("nullDB"))?null:db);
        fixtureChain();
        if(input.containsKey("existing"))db.createTransaction((String)input.get("existing"));
        var out=new JSONObject();
        try {
            String name=(String)input.get("name");var command=command(name);var params=(JSONObject)input.get("params");command.getParams().putAll(params);
            if("$ROW".equals(command.getParams().get("data")))command.getParams().put("data",MiniData.getMiniDataVersion(new TxnRow("imported",new Transaction(),new Witness())).to0xString());
            if(input.containsKey("consolidateHelper")){
                ArrayList<Coin> coins=null;if(input.get("consolidateHelper")!=null){coins=new ArrayList<>();for(Object value:(JSONArray)input.get("consolidateHelper"))coins.add(fixtureCoin(value));}
                MiniNumber burn=params.get("burn")==null?null:new MiniNumber(params.getString("burn"));MiniNumber keyuses=params.get("keyuses")==null?null:new MiniNumber(params.getString("keyuses"));
                out.put("value",((Consolidate)command).createConsolidate(coins,burn,params.getString("fromaddress"),params.getString("tokenid"),params.getString("script"),params.getString("privatekey"),keyuses));
            }
            else if(Boolean.TRUE.equals(input.get("meta"))){var meta=new JSONObject();meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("params",command.getValidParams());meta.put("fresh",command.getFunction().getJSONReply());out.put("value",meta);}
            else if(input.containsKey("runner")){
                Command saved=null;int index=-1;for(int i=0;i<CommandRunner.ALL_COMMANDS.length;i++)if(CommandRunner.ALL_COMMANDS[i].getName().equals(name)){index=i;break;}
                if(Boolean.TRUE.equals(input.get("recordRunner"))){saved=CommandRunner.ALL_COMMANDS[index];CommandRunner.ALL_COMMANDS[index]=command(name);}
                try{out.put("value",CommandRunner.getRunner().runMultiCommand("0x00",(String)input.get("runner")));}finally{if(saved!=null)CommandRunner.ALL_COMMANDS[index]=saved;}
            }
            else if(input.containsKey("helper")){
                String text=(String)input.get("helper");Object value=switch(name){case "consolidatefrom"->new consolidatefrom().runCommand(text);case "sendfrom"->new sendfrom().runCommand(text);case "createtokenfrom"->new createtokenfrom().runCommand(text);case "rawfrom"->new rawfrom().runCommand(text);case "rawtxnfrom"->new rawtxnfrom().runCommand(text);case "createfrom"->new createfrom().runCommand(text);case "constructfrom"->new constructfrom().runCommand(text);case "postfrom"->new postfrom().runCommand(text);default->new signfrom().runCommand(text);};out.put("value",value);
            }else out.put("value",command.runCommand());
        }catch(Exception error){out.put("exception",error.toString());}
        if(Boolean.TRUE.equals(input.get("real")) && out.get("value") instanceof JSONObject value && value.get("response") instanceof JSONObject response && response.get("data") instanceof String data){
            var bytes=new MiniData(data).getBytes();var stream=new ByteArrayInputStream(bytes);var row=TxnRow.ReadFromStream(new DataInputStream(stream));
            var observation=new JSONObject();observation.put("consumed",bytes.length-stream.available());observation.put("wire",MiniData.getMiniDataVersion(row).to0xString());
            var transaction=row.getTransaction();transaction.calculateTransactionID();var valid=new JSONArray();
            for(var signature:row.getWitness().getAllSignatures()){var key=new org.minima.objects.keys.TreeKey();key.setPublicKey(signature.getRootPublicKey());valid.add(key.verify(transaction.getTransactionID(),signature));}
            observation.put("signatures",valid);out.put("export",observation);
        }
        out.put("calls",calls);out.put("rows",rows());return out;
    }
    public static void main(String[] args)throws Exception {
        var reader=new BufferedReader(new InputStreamReader(System.in));var stdout=System.out;String line;
        while((line=reader.readLine())!=null){var bytes=new ByteArrayOutputStream();JSONObject result;try{System.setOut(new PrintStream(bytes,true,"UTF-8"));result=run((JSONObject)new JSONParser().parse(line));}finally{System.setOut(stdout);}
            for(String item:bytes.toString("UTF-8").split("\n")){int at=item.indexOf("] : ");if(at>=0&&!item.substring(at+4).startsWith("     "))logs.add(item.substring(at+4));}result.put("logs",logs);stdout.println(result.toJSONString());
        }
    }
}
