import java.io.*;
import java.util.*;
import javax.crypto.Cipher;
import org.minima.system.commands.Command;
import org.minima.system.commands.CommandRunner;
import org.minima.system.commands.backup.decryptbackup;
import org.minima.system.params.GeneralParams;
import org.minima.objects.base.MiniData;
import org.minima.utils.encrypt.GenerateKey;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class DecryptBackupOracle {
    static JSONObject run(JSONObject request)throws Exception{
        var out=new JSONObject();
        try{
            String op=(String)request.get("op");
            if("make".equals(op)){
                byte[] salt=new MiniData((String)request.get("salt")).getBytes(),iv=new MiniData((String)request.get("iv")).getBytes();
                byte[] key=GenerateKey.secretKey((String)request.get("password"),salt).getEncoded();
                var bytes=new ByteArrayOutputStream();var stream=new DataOutputStream(bytes);new MiniData(salt).writeDataStream(stream);new MiniData(iv).writeDataStream(stream);
                stream.write(GenerateKey.getCipherSYM(Cipher.ENCRYPT_MODE,iv,key).doFinal(new MiniData((String)request.get("compressed")).getBytes()));
                out.put("data",new MiniData(bytes.toByteArray()).to0xString());return out;
            }
            GeneralParams.BASE_FILE_FOLDER=(String)request.get("folder");
            Command command=new decryptbackup();
            if("meta".equals(op)){
                out.put("name",command.getName());out.put("help",command.getHelp());out.put("fullHelp",command.getFullHelp());
                var params=new JSONArray();params.addAll(command.getValidParams());out.put("params",params);
                command.getParams().put("file","fixture");out.put("freshParams",command.getFunction().getParams());
            }else if("runner".equals(op))out.put("result",CommandRunner.getRunner().runMultiCommand((String)request.get("caller"),(String)request.get("command")));
            else{command.getParams().putAll((JSONObject)request.get("params"));out.put("result",command.runCommand());}
        }catch(Throwable error){out.put("exception",error.toString());}return out;
    }
    public static void main(String[] ignored)throws Exception{
        Locale.setDefault(Locale.ENGLISH);var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null)System.out.println(run((JSONObject)new JSONParser().parse(line)).toJSONString());
    }
}
