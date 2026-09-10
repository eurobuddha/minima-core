import java.io.*;
import java.nio.file.*;
import java.util.*;
import org.minima.system.network.p2p2.P2P2DB;
import org.minima.utils.JsonDB;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
import org.minima.objects.base.MiniData;
public class P2P2DBOracle {
    static String wire(JsonDB db)throws Exception{var bytes=new ByteArrayOutputStream();db.writeDataStream(new DataOutputStream(bytes));return new MiniData(bytes.toByteArray()).to0xString();}
    static Object run(JSONObject request)throws Exception{
        var db=new P2P2DB();JsonDB base=db;var alias=db.getAllKnownPeers();var directory=Files.createTempDirectory("minima-p2p2-fixture-");var file=directory.resolve("peers.db");Files.write(directory.resolve("blocked"),new byte[0]);var states=new JSONArray();
        try{
            for(Object item:(JSONArray)request.get("ops")){
                var op=(JSONObject)item;var name=(String)op.get("op");var value=op.get("value");Object result="OK";
                try{
                    switch(name){
                        case "add":result=db.addPeerToAllKnown((String)value);break;
                        case "append":alias.add((String)value);break;
                        case "clear":alias.clear();break;
                        case "first":db.setFirstStartUp((boolean)value);break;
                        case "put":db.getAllData().put((String)op.get("key"),value);break;
                        case "read":db.readDataStream(new DataInputStream(new ByteArrayInputStream(new MiniData((String)value).getBytes())));break;
                        case "file":Files.write(file,new MiniData((String)value).getBytes());break;
                        case "save":base.saveDB(file.toFile());break;
                        case "saveFail":base.saveDB(directory.resolve("blocked/file").toFile());break;
                        case "load":base.loadDB(file.toFile());break;
                        case "loadMissing":base.loadDB(directory.resolve("missing").toFile());break;
                        default:throw new IllegalArgumentException(name);
                    }
                }catch(Exception e){result="ERROR";}
                var state=new JSONObject();state.put("result",result);var peers=new JSONArray();peers.addAll(db.getAllKnownPeers());state.put("peers",peers);state.put("alias",alias==db.getAllKnownPeers());state.put("data",db.getAllData());
                try{state.put("first",db.isFirstStartUp());}catch(Exception e){state.put("first","ERROR");}
                try{state.put("wire",wire(db));}catch(Exception e){state.put("wire","ERROR");}
                state.put("file",Files.exists(file)?new MiniData(Files.readAllBytes(file)).to0xString():null);states.add(new JSONParser().parse(state.toString()));
            }
            return states;
        }finally{try(var paths=Files.walk(directory)){for(var path:paths.sorted(Comparator.reverseOrder()).toList())Files.delete(path);}}
    }
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception e){System.out.println("\"ERROR\"");}}
}
