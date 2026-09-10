import java.io.*;
import java.lang.reflect.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.system.network.rpc.Authorizer;
import org.minima.system.network.minima.NIOTraffic;
import org.minima.system.params.GeneralParams;
import org.minima.objects.base.MiniData;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class NetworkUtilityOracle {
    static long number(JSONObject o,String k){return ((Number)o.get(k)).longValue();}
    public static void main(String[] args)throws Exception{Locale.setDefault(Locale.ENGLISH);var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{var data=(JSONObject)new JSONParser().parse(line);String type=(String)data.get("type"),out="";
        if(type.equals("auth")){MinimaDB.createDB();var userdb=MinimaDB.getDB().getUserDB();userdb.getAllData().put("rpcusers_allusers",data.get("users"));GeneralParams.RPC_AUTHENTICATE=(boolean)data.get("authenticate");GeneralParams.RPC_AUTHSTYLE=(String)data.get("style");GeneralParams.RPC_PASSWORD=(String)data.get("password");out=Authorizer.checkAuchCredentials((String)data.get("header")).toJSONString();}
        else if(type.equals("base64")){out=new MiniData(Base64.getDecoder().decode((String)data.get("data"))).to0xString();}
        else{var traffic=new NIOTraffic();for(Object obj:(JSONArray)data.get("ops")){var op=(JSONObject)obj;String action=(String)op.get("op"),result="OK";try{switch(action){case "reset":traffic.reset();var field=NIOTraffic.class.getDeclaredField("mStartTime");field.setAccessible(true);field.setLong(traffic,number(op,"amount"));break;case "read":traffic.addToTotalRead(number(op,"amount"));break;case "write":traffic.addToTotalWrite(number(op,"amount"));break;case "readkey":traffic.addReadBytes((String)op.get("key"),number(op,"amount"));break;case "writekey":traffic.addWriteBytes((String)op.get("key"),number(op,"amount"));break;default:throw new Exception();}}catch(Exception e){result="ERROR";}out+=result+":"+traffic.getTotalRead()+":"+traffic.getTotalWrite()+":"+traffic.getBreakdown().toJSONString()+"|";}out+="start:"+traffic.getStartTime();}
        System.out.println(out);
    }catch(Exception e){System.out.println("ERROR");}}
}
