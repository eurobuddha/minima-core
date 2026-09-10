import java.io.*;
import java.sql.*;
import java.lang.reflect.Proxy;
import org.minima.database.wallet.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class WalletRowsOracle {
    static int integer(JSONObject o,String k){var v=o.get(k);return v==null?0:((Number)v).intValue();}
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{var data=(JSONObject)new JSONParser().parse(line);String type=(String)data.get("type");boolean sql=(boolean)data.get("sql");var fields=(JSONObject)data.get("fields");int fail=integer(data,"fail");var log=new StringBuilder();int[] calls={0};var row=(ResultSet)Proxy.newProxyInstance(WalletRowsOracle.class.getClassLoader(),new Class[]{ResultSet.class},(proxy,method,values)->{String key=(String)values[0];log.append(method.getName()).append(":").append(key).append(",");if(++calls[0]==fail)throw new SQLException("fixture failure");return method.getName().equals("getInt")?integer(fields,key):(String)fields.get(key);});String out;
        try{JSONObject getters=new JSONObject(),json;
            if(type.equals("seed")){var r=sql?new SeedRow(row):new SeedRow((String)fields.get("phrase"),(String)fields.get("seed"));getters.put("phrase",r.getPhrase());getters.put("seed",r.getSeed());json=r.toJSON();}
            else if(type.equals("key")){var r=sql?new KeyRow(row):new KeyRow(integer(fields,"size"),integer(fields,"depth"),integer(fields,"uses"),integer(fields,"maxuses"),(String)fields.get("modifier"),(String)fields.get("privatekey"),(String)fields.get("publickey"));getters.put("size",r.getSize());getters.put("depth",r.getDepth());getters.put("uses",r.getUses());getters.put("maxuses",r.getMaxUses());getters.put("modifier",r.getModifier());getters.put("privatekey",r.getPrivateKey());getters.put("publickey",r.getPublicKey());json=r.toJSON();}
            else{var r=sql?new ScriptRow(row):new ScriptRow((String)fields.get("script"),(String)fields.get("address"),integer(fields,"simple")!=0,integer(fields,"defaultaddress")!=0,(String)fields.get("publickey"),integer(fields,"track")!=0);getters.put("script",r.getScript());getters.put("address",r.getAddress());getters.put("simple",r.isSimple());getters.put("default",r.isDefault());getters.put("publickey",r.getPublicKey());getters.put("track",r.isTrack());json=r.toJSON();}
            out=getters.toJSONString()+"|"+json.toJSONString();
        }catch(SQLException e){out="SQL_ERROR";}catch(Exception e){out="ERROR";}System.out.println(out+"|"+log);
    }catch(Exception e){System.out.println("ERROR");}}
}
