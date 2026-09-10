import java.io.*;
import java.lang.reflect.*;
import java.util.*;
import org.minima.objects.base.MiniNumber;
import org.minima.system.params.*;
import org.minima.system.network.p2p.params.P2PParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

public class ParamConfigurerOracle {
    static final Class<?> keyClass;
    static final Object[] keys;
    static final Field params;
    static final Map<Field,Object> defaults=new LinkedHashMap<>();
    static {
        try {
            keyClass=Class.forName("org.minima.system.params.ParamConfigurer$ParamKeys");keys=keyClass.getEnumConstants();
            params=ParamConfigurer.class.getDeclaredField("paramKeysToArg");params.setAccessible(true);
            for(Class<?> type:new Class<?>[]{GeneralParams.class,GlobalParams.class,TestParams.class})for(Field field:type.getFields())if(Modifier.isStatic(field.getModifiers()))defaults.put(field,field.get(null));
            var field=P2PParams.class.getField("LOG_LEVEL");defaults.put(field,field.get(null));
        }catch(Exception error){throw new RuntimeException(error);}
    }
    static Object metadata()throws Exception {
        var result=new JSONObject();var list=new JSONArray();
        var key=keyClass.getDeclaredField("key");key.setAccessible(true);var help=keyClass.getDeclaredField("helpMsg");help.setAccessible(true);
        for(Object object:keys){var row=new JSONObject();row.put("name",object.toString());row.put("key",key.get(object));row.put("help",help.get(object));row.put("hash",object.hashCode());list.add(row);}
        result.put("keys",list);return result;
    }
    static String[] args(Object value){return value==null?null:(String[])((JSONArray)value).toArray(new String[0]);}
    static JSONObject fields(Class<?> type)throws Exception {var result=new JSONObject();for(Field field:type.getFields())if(Modifier.isStatic(field.getModifiers())){var value=field.get(null);result.put(field.getName(),value instanceof MiniNumber?value.toString():value);}return result;}
    static void set(Class<?> type,String name,Object value)throws Exception {var field=type.getField(name);if(field.getType()==MiniNumber.class)field.set(null,new MiniNumber((String)value));else if(field.getType()==int.class)field.setInt(null,((Number)value).intValue());else if(field.getType()==long.class)field.setLong(null,((Number)value).longValue());else field.set(null,value);}
    static Object run(JSONObject request)throws Exception {
        if("metadata".equals(request.get("kind")))return metadata();
        for(var item:defaults.entrySet())item.getKey().set(null,item.getValue());
        Locale.setDefault(Locale.forLanguageTag((String)request.getOrDefault("language","en")));
        var config=new ParamConfigurer();var output=new JSONArray();var previous=System.out;
        for(Object raw:(JSONArray)request.get("ops")) {
            var operation=(JSONObject)raw;var row=new JSONObject();var bytes=new ByteArrayOutputStream();System.setOut(new PrintStream(bytes,true,"UTF-8"));
            try {
                Object value=operation.get("value");
                switch((String)operation.get("op")) {
                    case "args":row.put("same",config.usingProgramArgs(args(value))==config);break;
                    case "conf":row.put("same",config.usingConfFile(args(value))==config);break;
                    case "env":{Map<String,String> env=null;if(value!=null){env=new LinkedHashMap<>();for(Object item:(JSONArray)value){var pair=(JSONArray)item;env.put((String)pair.get(0),(String)pair.get(1));}}row.put("same",config.usingEnvVariables(env)==config);break;}
                    case "configure":row.put("same",config.configure()==config);break;
                    case "checkArgs":row.put("result",ParamConfigurer.checkParams(args(value)));break;
                    case "checkText":row.put("result",ParamConfigurer.checkParams((String)value));break;
                    case "setTest":set(TestParams.class,(String)operation.get("field"),value);break;
                    case "setGlobal":set(GlobalParams.class,(String)operation.get("field"),value);break;
                    case "applyTest":TestParams.setTestParams();break;
                    case "reset":GeneralParams.resetDefaults();break;
                    default:throw new IllegalArgumentException((String)operation.get("op"));
                }
            }catch(Exception error){row.put("exception",error.toString());}
            finally{System.setOut(previous);}
            row.put("stdout",bytes.toString("UTF-8"));row.put("general",fields(GeneralParams.class));row.put("global",fields(GlobalParams.class));row.put("test",fields(TestParams.class));row.put("daemon",config.isDaemon());row.put("shutdownhook",config.isShutDownHook());row.put("level",P2PParams.LOG_LEVEL.toString());
            var map=(Map<?,?>)params.get(config);var entries=new JSONArray();for(var entry:map.entrySet()){var pair=new JSONArray();pair.add(entry.getKey().toString());pair.add(entry.getValue());entries.add(pair);}row.put("params",entries);output.add(row);
        }
        return output;
    }
    public static void main(String[] ignored)throws Exception {var reader=new BufferedReader(new InputStreamReader(System.in));String line;while((line=reader.readLine())!=null) {try {System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception error){error.printStackTrace();System.out.println("{\"fatal\":true}");}}}
}
