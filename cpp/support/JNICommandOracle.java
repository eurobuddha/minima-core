import org.minima.system.commands.Command;
import org.minima.objects.base.MiniData;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class JNICommandOracle {
    static JSONObject request,events=new JSONObject();
    public static void hello() {
        events.put("hello",true);
        if("hello_error".equals(request.get("mode")))throw new IllegalStateException("fixture hello error");
    }
    public static byte[] hash(byte[] nonce,int attempts,byte[] difficulty,byte[] header) {
        events.put("nonce",new MiniData(nonce).to0xString());events.put("attempts",attempts);
        events.put("difficulty",new MiniData(difficulty).to0xString());events.put("header",new MiniData(header).to0xString());
        if("hash_error".equals(request.get("mode")))throw new IllegalStateException("fixture hash error");
        if(request.containsKey("mutate")) {
            java.util.Arrays.fill(difficulty,((Number)request.get("mutate")).byteValue());
            events.put("mutateddifficulty",new MiniData(difficulty).to0xString());
        }
        if(request.containsKey("result"))return request.get("result")==null?null:new MiniData((String)request.get("result")).getBytes();
        return nonce;
    }
    public static void main(String[] args)throws Exception {
        request=(JSONObject)new JSONParser().parse(args[0]);
        String name=(String)request.get("name");String mode=(String)request.get("mode");
        if(!"unloaded".equals(mode))System.load(args[1]);
        Class<?> type=Class.forName("org.minima.system.commands.base."+name);
        Command command=(Command)type.getConstructor().newInstance();command.getParams().putAll((JSONObject)request.get("params"));
        JSONObject out=new JSONObject();
        try {
            if("meta".equals(mode)) {
                JSONObject meta=new JSONObject();meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());
                JSONArray params=new JSONArray();params.addAll(command.getValidParams());meta.put("validparams",params);
                Command fresh=command.getFunction();meta.put("freshname",fresh.getName());meta.put("freshparams",fresh.getParams());meta.put("distinct",fresh!=command);
                var format=type.getDeclaredMethod("outputByteArray",byte[].class);format.setAccessible(true);byte[] bytes=new byte[256];for(int i=0;i<256;i++)bytes[i]=(byte)i;
                meta.put("byteformat",format.invoke(command,(Object)bytes));out.put("value",meta);
            }else out.put("value",command.runCommand());
        }catch(Throwable e){out.put("error",e.getClass().getName());out.put("message",e.getMessage());}
        out.put("events",events);System.out.println(out);
    }
}
