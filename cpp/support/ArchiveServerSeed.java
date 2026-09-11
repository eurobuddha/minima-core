import org.minima.utils.mysql.MySQLConnect;
import org.minima.objects.TxBlock;
import org.minima.utils.json.*;
// Build/validation only: actual pinned Java writes the database read by both servers.
public class ArchiveServerSeed {
    public static void main(String[] args)throws Exception{
        Class.forName("com.mysql.cj.jdbc.Driver");
        var mysql=new MySQLConnect(args[0],args[1],args[2],args[3]);mysql.init();mysql.wipeAll();var blocks=new JSONArray();
        for(int i=0;i<3;i++){var block=new TxBlock(ArchiveManagerOracle.txpow(i,0));if(!mysql.saveBlock(block))throw new AssertionError("Block write failed");blocks.add(ArchiveManagerOracle.wire(block));}
        var spec=new JSONObject();spec.put("count",3);var cascade=ArchiveManagerOracle.cascade(spec);mysql.saveCascade(cascade);mysql.shutdown();
        var result=new JSONObject();result.put("blocks",blocks);result.put("cascade",ArchiveManagerOracle.wire(cascade));System.out.println(result.toJSONString());
    }
}
