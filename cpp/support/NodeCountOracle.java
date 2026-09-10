import java.io.*;
import java.net.*;
import java.nio.file.*;
import java.util.*;
import org.minima.system.commands.*;
import org.minima.system.network.p2p.params.P2PParams;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class NodeCountOracle {
    static JSONObject run(JSONObject request,String root)throws Exception{
        var folder=Path.of(root);if(Files.exists(folder))try(var paths=Files.walk(folder)){for(var path:paths.sorted(Comparator.reverseOrder()).toList())Files.delete(path);}Files.createDirectories(folder);GeneralParams.BASE_FILE_FOLDER=root;
        var manager=NIOClientOracle.install();var out=new JSONObject();var captured=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(captured,true,"UTF-8"));
        try{
            String mode=request.getString("mode");
            if(mode.equals("double")){double value=Double.parseDouble(request.getString("text"));out.put("bits",Long.toUnsignedString(Double.doubleToRawLongBits(value)));out.put("text",Double.toString(value));}
            else{
                if(request.containsKey("directory"))Files.createDirectories(folder.resolve(request.getString("directory")));
                if(request.containsKey("initial"))Files.write(folder.resolve("scan.csv"),new org.minima.objects.base.MiniData(request.getString("initial")).getBytes());
                P2PParams.DEFAULT_NODE_LIST=request.get("defaults")==null?null:new ArrayList<>();
                if(P2PParams.DEFAULT_NODE_LIST!=null)for(Object item:(JSONArray)request.get("defaults")){if(item==null){P2PParams.DEFAULT_NODE_LIST.add(null);continue;}var spec=(JSONObject)item;String host=(String)spec.get("host");int port=((Number)spec.get("port")).intValue();P2PParams.DEFAULT_NODE_LIST.add(Boolean.TRUE.equals(spec.get("unresolved"))?InetSocketAddress.createUnresolved(host,port):new InetSocketAddress(host,port));}
                if(mode.equals("run"))out.put("value",CommandRunner.getRunner().runMultiCommand(request.getString("caller"),request.getString("text")));
                else{var command=new org.minima.system.commands.network.nodecount();command.getParams().putAll((JSONObject)request.get("params"));
                    if(mode.equals("meta")){var meta=new JSONObject();meta.put("name",command.getName());meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("valid",command.getValidParams());meta.put("fresh",command.getFunction().getJSONReply());out.put("value",meta);}
                    else out.put("value",command.runCommand());
                }
            }
        }catch(Exception e){out.put("exception",e.toString());}finally{System.setOut(previous);}
        var logs=new JSONArray();for(String line:captured.toString("UTF-8").split("\\R")){int at=line.indexOf("] : ");if(at>=0)logs.add(line.substring(at+4));}out.put("logs",logs);out.put("traffic",manager.traffic.getBreakdown());out.put("read",manager.traffic.getTotalRead());out.put("write",manager.traffic.getTotalWrite());
        var files=new JSONObject();var dirs=new JSONArray();try(var paths=Files.walk(folder)){for(var path:paths.filter(p->!p.equals(folder)).toList()){if(Files.isRegularFile(path))files.put(folder.relativize(path).toString(),new org.minima.objects.base.MiniData(Files.readAllBytes(path)).to0xString());else if(Files.isDirectory(path))dirs.add(folder.relativize(path).toString());}}out.put("files",files);out.put("directories",dirs);return out;
    }
    public static void main(String[] args)throws Exception{GeneralParams.NETWORKING_LOGS=false;var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)System.out.println(run((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}
}
