import java.io.*;
import java.util.*;
import java.lang.reflect.*;
import java.nio.charset.StandardCharsets;
import org.minima.utils.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
import org.minima.objects.base.*;
import org.minima.database.userprefs.UserDB;
public class PrefsOracle {
    static Object argument(Object arg,Class<?> type){if(type==MiniNumber.class)return new MiniNumber((String)arg);if(type==MiniData.class)return new MiniData((String)arg);if(type==ArrayList.class){var out=new ArrayList<String>();for(var obj:(JSONArray)arg)out.add((String)obj);return out;}return arg;}
    static String render(Object value){if(value instanceof MiniData)return JSONValue.toJSONString(value.toString());if(value instanceof MiniNumber)return JSONValue.toJSONString(value.toString());return JSONValue.toJSONString(value);}
    static String read(UserDB db,String text) throws Exception{var bytes=new ByteArrayOutputStream();new MiniString(text).writeDataStream(new DataOutputStream(bytes));db.readDataStream(new DataInputStream(new ByteArrayInputStream(bytes.toByteArray())));return "null";}
    public static void main(String[] args) throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{
        var request=(JSONObject)new JSONParser().parse(line);Locale.setDefault(Locale.forLanguageTag((String)request.get("locale")));var db=new UserDB();db.setString("keep","original");String out;
        if(request.containsKey("date")){System.out.println(MiniUtil.DATEFORMAT.format(new Date(Long.parseLong((String)request.get("date")))));continue;}
        try{out=read(db,(String)request.get("initial"));}catch(Exception e){out="ERROR";}
        for(Object obj:(JSONArray)request.get("commands")){
            var command=(JSONArray)obj;String name=(String)command.get(0);int count=command.size()-1;String value="ERROR";
            try{if(name.equals("wire")){var bytes=new ByteArrayOutputStream();db.writeDataStream(new DataOutputStream(bytes));value=JSONValue.toJSONString(BaseConverter.encode16(bytes.toByteArray()));}
                else if(name.equals("alias")){var array=new JSONArray();db.setJSONArray("a",array);array.add("first");var original=db.getJSONArray("a");db.setJSONArray("b",original);original.add(null);db.getJSONArray("b").add("last");value=render(db.getAllData());}
                else{for(Method m:UserDB.class.getMethods())if(m.getName().equals(name)&&m.getParameterCount()==count){var values=new Object[count];for(int i=0;i<count;i++)values[i]=argument(command.get(i+1),m.getParameterTypes()[i]);value=render(m.invoke(db,values));break;}}
            }catch(Exception e){value="ERROR";}
            out+="|"+value;
        }
        out+="|"+render(db.getAllData());System.out.println(out);
    }catch(Exception e){System.out.println("ERROR");}}
}
