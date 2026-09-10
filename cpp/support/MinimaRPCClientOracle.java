import java.io.*;
import java.util.HexFormat;
import org.minima.utils.MinimaRPCClient;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

// Invokes the unchanged source main, preserving nullable UTF-16 arguments.
public class MinimaRPCClientOracle {
    public static void main(String[] ignored)throws Exception {
        var source=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=source.readLine())!=null) {
            var request=(JSONObject)new JSONParser().parse(line);
            var supplied=(JSONArray)request.get("args");
            String[] args=supplied==null?null:(String[])supplied.toArray(new String[0]);
            var output=new JSONObject();var out=new ByteArrayOutputStream();var err=new ByteArrayOutputStream();
            var priorOut=System.out;var priorErr=System.err;var priorIn=System.in;
            var events=new JSONArray();
            System.setIn(new FilterInputStream(new ByteArrayInputStream(HexFormat.of().parseHex((String)request.getOrDefault("input","657869740a")))) {
                boolean failed=false;
                public int read(byte[] bytes,int start,int length)throws IOException {
                    if(!failed&&Boolean.TRUE.equals(request.get("readFail"))){failed=true;throw new IOException("fixture read");}
                    return super.read(bytes,start,length);
                }
                public void close()throws IOException {events.add("close");if(Boolean.TRUE.equals(request.get("closeFail")))throw new IOException("fixture close");super.close();}
            });
            System.setOut(new PrintStream(out,true,"UTF-8"));System.setErr(new PrintStream(err,true,"UTF-8"));
            try {MinimaRPCClient.main(args);output.put("status",0);}
            catch(Exception error){output.put("exception",error.toString());}
            finally {System.setOut(priorOut);System.setErr(priorErr);System.setIn(priorIn);}
            output.put("stdout",out.toString("UTF-8"));output.put("stderr",err.toString("UTF-8"));output.put("events",events);
            priorOut.println(output.toJSONString());
        }
    }
}
