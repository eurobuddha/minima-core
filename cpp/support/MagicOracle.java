import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.utils.Streamable;
import org.minima.utils.json.*;
public class MagicOracle {
    static int integer(JSONObject q,String k){return ((Number)q.get(k)).intValue();}
    static Streamable get(Magic m,int i){return switch(i){
        case 0->m.getMaxTxPoWSize();
        case 1->m.getMaxKISSOps();
        case 2->m.getMaxNumTxns();
        case 3->m.getMinTxPowWork();
        case 4->m.mDesiredMaxTxPoWSize;
        case 5->m.mDesiredMaxKISSVMOps;
        case 6->m.mDesiredMaxTxnPerBlock;
        case 7->m.mDesiredMinTxPoWWork;
        default->throw new IllegalArgumentException("field index");};}
    static void set(Magic m,int i,Streamable p){switch(i){
        case 0:m.mCurrentMaxTxPoWSize=(MiniNumber)p;break;
        case 1:m.mCurrentMaxKISSVMOps=(MiniNumber)p;break;
        case 2:m.mCurrentMaxTxnPerBlock=(MiniNumber)p;break;
        case 3:m.mCurrentMinTxPoWWork=(MiniData)p;break;
        case 4:m.setDesiredMaxTxPoWSize((MiniNumber)p);break;
        case 5:m.setDesiredKISSVM((MiniNumber)p);break;
        case 6:m.setDesiredMaxTxns((MiniNumber)p);break;
        case 7:m.mDesiredMinTxPoWWork=(MiniData)p;break;
        default:throw new IllegalArgumentException("field index");}}
    static Streamable item(int i,Object v){return v==null?null:i==3||i==7?new MiniData((String)v):new MiniNumber((String)v);}
    static String text(Streamable p){return p==null?null:p instanceof MiniNumber?p.toString():((MiniData)p).to0xString();}
    static JSONArray fields(Magic m){var a=new JSONArray();for(int i=0;i<8;i++)a.add(text(get(m,i)));return a;}
    static void init(Magic m,JSONObject q){if(q.containsKey("values")){var v=(JSONArray)q.get("values");for(int i=0;i<8;i++)set(m,i,item(i,v.get(i)));}}
    static class FaultNumber extends MiniNumber{Runnable action;public void writeDataStream(DataOutputStream out)throws IOException{out.writeByte(0xAB);if(action!=null)action.run();throw new IOException("fixture failure");}}
    static class FaultData extends MiniData{Runnable action;public void writeDataStream(DataOutputStream out)throws IOException{out.writeByte(0xAB);if(action!=null)action.run();throw new IOException("fixture failure");}}
    static JSONArray run(JSONObject q){
        Magic owner=new Magic();init(owner,q);Magic alias=owner;var containing=new TxPoW();containing.setMagic(owner);
        var held=new ArrayList<Streamable>();for(int i=0;i<8;i++)held.add(get(owner,i));var output=new JSONArray();
        for(Object raw:(JSONArray)q.get("ops")){
            var op=(JSONObject)raw;String name=op.getString("op");var result=new JSONObject();
            try{
                switch(name){
                    case "read":case "mutate":{
                        byte[] wire=new MiniData(op.getString("wire")).getBytes();int offset=op.containsKey("offset")?integer(op,"offset"):0;
                        var input=new ByteArrayInputStream(wire);input.skip(offset);var stream=new DataInputStream(input);
                        try{Streamable target=name.equals("read")?owner:held.get(integer(op,"index"));target.readDataStream(stream);}catch(Exception e){result.put("error",e.getClass().getName());}result.put("offset",wire.length-input.available());break;
                    }
                    case "write":{var bytes=new ByteArrayOutputStream();bytes.write(0xAA);bytes.write(0x55);try{Streamable target=owner;target.writeDataStream(new DataOutputStream(bytes));}catch(Exception e){result.put("error",e.getClass().getName());}result.put("wire",new MiniData(bytes.toByteArray()).to0xString());break;}
                    case "json":result.put("value",owner.toJSON());break;
                    case "same":{var other=new Magic();init(other,op);result.put("value",Boolean.TRUE.equals(op.get("nullOther"))?owner.checkSame(null):owner.checkSame(other));break;}
                    case "next":{var next=owner.calculateNewCurrent();result.put("value",fields(next));result.put("wire",MiniData.getMiniDataVersion(next).to0xString());if(Boolean.TRUE.equals(op.get("install")))owner=next;break;}
                    case "set":set(owner,integer(op,"index"),item(integer(op,"index"),op.get("value")));break;
                    case "setHeld":set(owner,integer(op,"index"),held.get(integer(op,"from")));break;
                    case "cache":result.put("value",((MiniData)held.get(integer(op,"index"))).getDataValue().toString());break;
                    case "replace":owner=new Magic();break;
                    case "release":owner=null;break;
                    case "defaults":{var fresh=new Magic();var same=new JSONArray();for(int i=0;i<4;i++){same.add(get(alias,i)==get(alias,i+4));same.add(get(alias,i)==get(fresh,i));}result.put("value",same);result.put("fresh",fields(fresh));break;}
                    case "fault":{
                        int index=integer(op,"index");Magic m=owner;Runnable action=op.containsKey("replaceField")?()->set(m,index,item(index,index==3||index==7?"0x99":"99")):null;
                        if(index==3||index==7){var f=new FaultData();f.action=action;set(owner,index,f);}else{var f=new FaultNumber();f.action=action;set(owner,index,f);}break;
                    }
                    default:throw new IllegalArgumentException("unknown operation");
                }
            }catch(Exception e){result.put("error",e.getClass().getName());}
            result.put("state",owner==null?null:fields(owner));result.put("alias",fields(alias));result.put("containing",fields(containing.getMagic()));
            var old=new JSONArray();var same=new JSONArray();for(int i=0;i<8;i++){old.add(text(held.get(i)));same.add(owner!=null&&get(owner,i)==held.get(i));}result.put("held",old);result.put("same",same);output.add(result);
        }
        return output;
    }
    public static void main(String[] args)throws Exception{var reader=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;while((line=reader.readLine())!=null){var q=(JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line);System.out.println(run(q));}}
}
