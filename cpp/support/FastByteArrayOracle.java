import java.io.*;
import org.minima.utils.FastByteArrayStream;
import org.minima.utils.Crypto;
import org.minima.objects.base.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class FastByteArrayOracle {
    static class Stream extends FastByteArrayStream {Stream(int n){super(n);}int capacity(){return buf.length;}void own(int off,int len){writeData(buf,off,len);}}
    static int integer(JSONObject o,String k){return ((Number)o.get(k)).intValue();}
    public static void main(String[] args)throws Exception{var in=new BufferedReader(new InputStreamReader(System.in));String line;while((line=in.readLine())!=null)try{var data=(JSONObject)new JSONParser().parse(line);var stream=new Stream(integer(data,"initial"));String out="";for(Object entry:(JSONArray)data.get("ops")){var op=(JSONObject)entry;String command=(String)op.get("op"),result="OK";try{
        if(command.equals("reset"))stream.reset();else if(command.equals("close"))stream.close();else if(command.equals("byte"))stream.write(integer(op,"value"));else if(command.equals("own"))stream.own(integer(op,"offset"),integer(op,"length"));
        else if(command.equals("text"))result=new JSONObject(){{put("text",stream.toString());put("high",stream.toString(integer(op,"high")));}}.toJSONString();
        else if(command.equals("output")){var bytes=new ByteArrayOutputStream();stream.writeTo(bytes);result=new MiniData(Crypto.getInstance().hashData(bytes.toByteArray())).to0xString();}
        else{byte[] bytes=op.get("data")==null?null:new MiniData((String)op.get("data")).getBytes();if(command.equals("fast"))stream.writeData(bytes,integer(op,"offset"),integer(op,"length"));else stream.write(bytes,integer(op,"offset"),integer(op,"length"));}
    }catch(Exception e){result="ERROR";}out+=result+":"+stream.size()+":"+stream.capacity()+":"+new MiniData(Crypto.getInstance().hashData(stream.toByteArray())).to0xString()+"|";}System.out.println(out);
    }catch(Exception e){System.out.println("ERROR");}}
}
