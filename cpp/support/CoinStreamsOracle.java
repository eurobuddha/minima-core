import java.io.*;
import java.util.*;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.system.brains.TxPoWSearcher;
import org.minima.system.params.GeneralParams;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.utils.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Actual source readers, converters, retained owners and logger failures. */
public class CoinStreamsOracle {
    static String error(Throwable e){return e instanceof NullPointerException?"java.lang.NullPointerException":e.toString();}
    static Object snap(Streamable value){if(value==null)return null;try{return ObjectOracle.result(value);}catch(Throwable e){return "snapshot:"+error(e);}}
    static MMRProof proof(int count){var p=new MMRProof(new MiniNumber(123));for(int i=0;i<count;i++)p.addProofChunk(i%2==0,MMRData.CreateMMRDataLeafNode(new MiniString("branch"+i),new MiniNumber(i+1)));return p;}
    static JSONArray states(ArrayList<StateVariable> list){var a=new JSONArray();for(var s:list)a.add(s.toJSON());return a;}
    static JSONObject fixtures()throws Exception{var out=new JSONObject();var coins=new JSONArray();var proofs=new JSONArray();var mmrs=new JSONArray();for(int i:new int[]{0,1,7,31}){var c=ObjectOracle.coin(i);coins.add(ObjectOracle.wire(c));proofs.add(ObjectOracle.wire(new CoinProof(c,proof(i%4))));}for(int i:new int[]{0,1,3})mmrs.add(ObjectOracle.wire(proof(i)));out.put("coin",coins);out.put("proof",proofs);out.put("mmr",mmrs);return out;}
    static JSONArray snapshot(Coin c,Coin other,Coin held,CoinProof cp,CoinProof alias,Witness witness,MMRProof p,MMRProof heldProof,ArrayList<StateVariable> oldState){var out=new JSONArray();for(Streamable item:new Streamable[]{c,other,held,cp,alias,witness.getAllCoinProofs().get(0),p,heldProof})out.add(snap(item));out.add(states(oldState));return out;}
    static JSONObject run(JSONObject request)throws Exception{
        if(request.getString("action").equals("fixtures"))return fixtures();
        var c=ObjectOracle.coin(7);var oldState=c.getState();var other=ObjectOracle.coin(1);other.setState(oldState);
        var p=proof(2);var cp=new CoinProof(c,p);var alias=cp;var witness=new Witness();witness.addCoinProof(cp);var held=cp.getCoin();var heldProof=cp.getMMRProof();
        if(Boolean.TRUE.equals(request.get("badHash")))c.resetCoinID(new MiniData(new byte[65]));
        if(Boolean.TRUE.equals(request.get("nullState")))c.setState(null);
        var bytes=BaseConverter.decode16(request.getString("wire","0x"));var stream=new DataInputStream(new ByteArrayInputStream(bytes));
        var logs=new JSONArray();int[] prints={0};int failAt=((Number)request.get("failAt")).intValue();String kind=request.getString("kind");
        var original=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String full){int at=full.indexOf("] : ");if(at<0)return;String body=full.substring(at+4);if(body.startsWith("     "))return;logs.add(body);if(++prints[0]==failAt){if(kind.equals("error"))throw new LinkageError("logger");throw new IllegalStateException("logger");}}
        });
        var out=new JSONObject();Object result="ok";
        try{
            switch(request.getString("action")){
            case "readCoin":c.readDataStream(stream);break;
            case "readProof":cp.readDataStream(stream);break;
            case "readMMR":p.readDataStream(stream);break;
            case "convertCoin":result=snap(Coin.convertMiniDataVersion(new MiniData(bytes)));break;
            case "convertProof":result=snap(CoinProof.convertMiniDataVersion(new MiniData(bytes)));break;
            case "convertMMR":result=snap(MMRProof.convertMiniDataVersion(new MiniData(bytes)));break;
            case "floating":{
                var node=new TxPoWTreeNode(new TxPoW());boolean previous=GeneralParams.USE_SQL_COINDB;GeneralParams.USE_SQL_COINDB=false;
                try{node.addCoinToNode(c);var found=TxPoWSearcher.getFloatingCoin(node,c.getAmount(),c.getAddress(),c.getTokenID());var value=new JSONObject();value.put("present",found!=null);value.put("value",snap(found));result=value;}finally{GeneralParams.USE_SQL_COINDB=previous;}break;
            }
            case "copy":result=snap(c.deepCopy());break;
            case "rename":result=snap(c.getSameCoinWithCoinID(new MiniData("0xEE")));break;
            case "mutate":break;
            default:throw new IllegalArgumentException("action");
            }
        }catch(Throwable e){result="escaped:"+error(e);}
        finally{System.setOut(original);}
        out.put("result",result);out.put("logs",logs);out.put("consumed",bytes.length-stream.available());
        out.put("beforeMutation",snapshot(c,other,held,cp,alias,witness,p,heldProof,oldState));
        if(Boolean.TRUE.equals(request.get("after"))){
            oldState.add(new StateVariable(255,"[held list]"));held.setSpent(!held.getSpent());
            p.addProofChunk(false,MMRData.CreateMMRDataLeafNode(new MiniString("after"),new MiniNumber(9)));
            cp.getCoin().setBlockCreated(new MiniNumber(999));cp.getMMRProof().addProofChunk(true,MMRData.CreateMMRDataLeafNode(new MiniString("wrapper"),new MiniNumber(10)));
        }
        out.put("afterMutation",snapshot(c,other,held,cp,alias,witness,p,heldProof,oldState));return out;
    }
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)System.out.println(run((JSONObject)new JSONParser().parse(line)));}
}
