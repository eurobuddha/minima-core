import java.io.*;
import java.sql.*;
import java.nio.charset.StandardCharsets;
import org.minima.database.archive.ArchiveManager;
import org.minima.database.txpowdb.onchain.TxPoWOnChainDB;
import org.minima.database.txpowdb.sql.TxPoWSqlDB;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.utils.json.*;

// Calls the pinned mains unchanged. Every user.home is a driver-owned fixture.
// Read mode independently opens the real saved H2 file and decodes its bytes.
public class DatabaseMainsOracle {
    static JSONArray read(String name,String home)throws Exception {
        boolean archive=name.equals("ArchiveManager"),onchain=name.equals("TxPoWOnChainDB");
        String file=new File(home,archive?"testfolder/sqlsync":"testsql").getAbsolutePath();
        var output=new JSONArray();org.h2.Driver.load();
        try(var connection=DriverManager.getConnection("jdbc:h2:"+file+";MODE=MySQL;DB_CLOSE_ON_EXIT=FALSE","SA","");var statement=connection.createStatement();var rows=statement.executeQuery("SELECT * FROM "+(archive?"syncblock":onchain?"onchaintxpow":"txpow")+" ORDER BY id")) {
            while(rows.next()) {
                var row=new JSONObject();row.put("id",rows.getLong("id"));row.put("txpowid",rows.getString("txpowid"));row.put("timemilli",rows.getLong("timemilli"));
                if(archive||onchain)row.put("block",rows.getLong("block"));
                if(onchain)row.put("blockid",rows.getString("blockid"));
                else {
                    byte[] bytes=rows.getBytes(archive?"syncdata":"txpowdata");var data=new MiniData(bytes);row.put("wire",data.to0xString());TxPoW txpow;
                    if(archive){var block=TxBlock.convertMiniDataVersion(data);txpow=block.getTxPoW();row.put("wireagain",MiniData.getMiniDataVersion(block).to0xString());}
                    else {txpow=TxPoW.convertMiniDataVersion(data);row.put("wireagain",MiniData.getMiniDataVersion(txpow).to0xString());for(String col:new String[]{"isblock","istransaction","isrelevant"})row.put(col,rows.getInt(col));row.put("parentid",rows.getString("parentid"));}
                    row.put("txpow",txpow.toJSON());
                    if(archive){txpow.setSuperParent(0,new MiniData());txpow.calculateTXPOWID();row.put("before_parent_id",txpow.getTxPoWID());}
                }
                output.add(row);
            }
        }
        return output;
    }
    public static void main(String[] args)throws Exception {
        if(args[0].equals("read")){System.out.println(read(args[1],args[3]));return;}
        System.setProperty("user.home",args[3]);
        PrintStream original=System.out;var buffer=new ByteArrayOutputStream();System.setOut(new PrintStream(buffer,true,StandardCharsets.UTF_8){public void println(String text){if(text.startsWith("Minima @ "))text=text.substring(text.indexOf(" : ")+3);super.println(text);}});
        String error="none";
        try {String[] input=args[2].equals("null")?null:args[2].equals("args")?new String[]{"unused","value"}:new String[0];switch(args[1]){case "ArchiveManager":ArchiveManager.main(input);break;case "TxPoWOnChainDB":TxPoWOnChainDB.main(input);break;case "TxPoWSqlDB":TxPoWSqlDB.main(input);break;default:throw new AssertionError("unknown main");}}
        catch(Throwable e){error=e.toString();}
        System.out.flush();System.setOut(original);var result=new JSONObject();result.put("stdout",buffer.toString(StandardCharsets.UTF_8));result.put("error",error);System.out.println(result);
    }
}
