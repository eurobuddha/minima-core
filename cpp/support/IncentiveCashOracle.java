import java.io.*;
import java.net.*;
import org.minima.database.MinimaDB;
import org.minima.system.params.GeneralParams;
import org.minima.objects.base.MiniData;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Run unmodified incentivecash/RPCClient with the HTTPS destination routed to loopback. */
public class IncentiveCashOracle extends WalletCommandsOracle {
    public static void main(String[] args)throws Exception {
        URL.setURLStreamHandlerFactory(protocol -> protocol.equals("https")?new URLStreamHandler(){
            protected URLConnection openConnection(URL url)throws IOException {
                String original=url.toExternalForm(),origin="https://incentivecash.minima.global";
                if(!original.startsWith(origin+"/"))throw new IOException("unexpected incentive destination");
                var call=new JSONObject();call.put("url",original);
                var prefs=new File(GeneralParams.DATA_FOLDER,"databases/userprefs.db");
                call.put("prefs",prefs.isFile()?new MiniData(java.nio.file.Files.readAllBytes(prefs.toPath())).to0xString():null);
                try{call.put("user",new JSONParser().parse(MinimaDB.getDB().getUserDB().getAllData().toJSONString()));}catch(Exception error){throw new IOException(error);}
                incentiveCalls.add(call);
                if(Boolean.TRUE.equals(config.get("httpFail")))throw new IOException("fixture HTTP failure");
                return new URL(config.getString("route")+original.substring(origin.length())).openConnection();
            }
        }:null);
        WalletCommandsOracle.main(args);
    }
}
