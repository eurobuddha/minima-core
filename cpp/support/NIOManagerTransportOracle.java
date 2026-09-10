import java.io.*;
import org.minima.objects.Greeting;
import org.minima.objects.base.*;
import org.minima.system.network.minima.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.Streamable;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class NIOManagerTransportOracle {
    static Object run(JSONObject request)throws Exception{
        var manager=NIOClientOracle.install();
        NIOClientOracle.set(manager,"mNIOServer",new NIOServer(0,manager));
        var out=new JSONObject();
        try{
            String mode=(String)request.get("mode");
            if(mode.equals("ping")){
                Greeting greeting=NIOManager.sendPingMessage((String)request.get("host"),((Number)request.get("port")).intValue(),true);
                out.put("result",greeting==null?null:MiniData.getMiniDataVersion(greeting).to0xString());
            }else{
                var type=new MiniByte(((Number)request.get("type")).intValue());
                out.put("name",NIOMessage.convertMessageType(type));
                String kind=(String)request.get("kind");Streamable value;
                switch(kind){
                    case "data":value=new MiniData((String)request.get("value"));break;
                    case "string":value=new MiniString((String)request.get("value"));break;
                    case "greeting":var greeting=new Greeting();greeting.setTopBlock(new MiniNumber(123));greeting.getExtraData().put("welcome","fixture");greeting.getChain().add(new MiniData("0xAB"));value=greeting;break;
                    case "null":value=null;break;
                    case "failure":case "oom":value=new Streamable(){
                        public void readDataStream(DataInputStream in)throws IOException{throw new UnsupportedOperationException();}
                        public void writeDataStream(DataOutputStream stream)throws IOException{out.put("serialized",true);if(kind.equals("oom"))throw new OutOfMemoryError("Fixture");throw new IOException("Fixture failure");}
                    };break;
                    default:throw new IllegalArgumentException(kind);
                }
                if(mode.equals("create"))out.put("result",NIOManager.createNIOMessage(type,value).to0xString());
                else{NIOManager.sendNetworkMessage((String)request.get("uid"),type,value);out.put("result","OK");}
            }
        }catch(IOException error){out.put("result","IO:"+error.getMessage());}
        catch(Exception error){out.put("result","ERROR");}
        out.put("traffic",manager.traffic.getBreakdown());out.put("read",manager.traffic.getTotalRead());out.put("write",manager.traffic.getTotalWrite());return out;
    }
    public static void main(String[] args)throws Exception{GeneralParams.NETWORKING_LOGS=false;var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception e){System.out.println("\"ERROR\"");}}
}
