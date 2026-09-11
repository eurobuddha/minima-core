import java.io.*;
import java.util.Locale;
import org.minima.Minima;
import org.minima.system.Main;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

// Calls the pinned class without changing its command runner or command table.
// The singleton identity probe does not invoke Main's constructor or methods.
public class MinimaEntryOracle {
    public static void main(String[] args)throws Exception {
        Locale.setDefault(Locale.ENGLISH);
        if(args.length>0){
            Thread.setDefaultUncaughtExceptionHandler((thread,error)->System.out.println("uncaught:"+thread.getName()+":"+error.getClass().getName()));
            new Minima().mainStarter(args[0].equals("starter-null")?null:new String[]{args[0].equals("starter-help")?"-help":"-entry-unknown"});
            if(args[0].equals("starter-null"))System.out.println("RETURNED");return;
        }
        var reader=new BufferedReader(new InputStreamReader(System.in));String line;
        var instance=new Minima();
        while((line=reader.readLine())!=null){
            var request=(JSONObject)new JSONParser().parse(line);var output=new JSONObject();
            try{
                if("singleton".equals(request.get("op"))){
                    var unsafeField=sun.misc.Unsafe.class.getDeclaredField("theUnsafe");unsafeField.setAccessible(true);
                    var unsafe=(sun.misc.Unsafe)unsafeField.get(null);
                    var field=Main.class.getDeclaredField("mMainInstance");field.setAccessible(true);
                    Object previous=field.get(null),owner=unsafe.allocateInstance(Main.class);
                    try{field.set(null,owner);output.put("same",Minima.getMain()==owner);field.set(null,null);output.put("cleared",Minima.getMain()==null);}
                    finally{field.set(null,previous);}
                }else{
                    String text=(String)request.get("text");int arity=((Number)request.get("arity")).intValue();
                    output.put("result",arity==1?instance.runMinimaCMD(text):arity==2?instance.runMinimaCMD(text,Boolean.TRUE.equals(request.get("pretty"))):instance.runMinimaCMD(text,Boolean.TRUE.equals(request.get("pretty")),(String)request.get("caller")));
                }
            }catch(Throwable error){output.put("exception",error.toString());}
            System.out.println(output.toJSONString());
        }
    }
}
