import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.utils.mysql.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.database.cascade.Cascade;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class ArchiveServerOracle extends SqlDBOracle {
    static class CleanMain extends org.minima.system.Main {
        boolean nested;
        public void PostNotifyEvent(String event,JSONObject data){if(!nested){nested=true;ArchiveServer.SystemClean();}}
    }
    static class Listener extends ArchiveServer {
        volatile boolean finished;
        Listener()throws Exception{super(0,"fixture:3306","fixture","student","synthetic");}
        public void run(){try{super.run();}finally{finished=true;}}
    }
    static JSONObject runArchive(JSONObject request)throws Exception{
        connectionID=preparedID=calls=0;events=new JSONArray();config=new JSONObject();config.put("rows",new JSONArray());config.put("columns",new JSONArray());
        var out=new JSONObject();var captured=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(captured,true,"UTF-8"));ArchiveServer server=null;Socket client=null,accepted=null;
        long before=System.currentTimeMillis();
        try{
            String op=request.getString("op");NIOClientOracle.field(ArchiveServer.class,"mLastClean").setLong(null,0);
            if(op.equals("clean")){
                long last=((Number)request.get("last")).longValue();NIOClientOracle.field(ArchiveServer.class,"mLastClean").setLong(null,last);
                var mainField=NIOClientOracle.field(org.minima.system.Main.class,"mMainInstance");var old=mainField.get(null);
                try{if(Boolean.TRUE.equals(request.get("reentrant")))mainField.set(null,NIOClientOracle.U.allocateInstance(CleanMain.class));ArchiveServer.SystemClean();}finally{mainField.set(null,old);}
                out.put("changed",NIOClientOracle.field(ArchiveServer.class,"mLastClean").getLong(null)!=last);
            }else if(op.equals("constructor")){
                config=request;server=new Listener();
                long end=System.currentTimeMillis()+5000;while(NIOClientOracle.get(server,"mServerSocket")==null&&System.currentTimeMillis()<end)Thread.sleep(1);
                // The log occurs after socket assignment; join the real listener before capture.
                while(!captured.toString("UTF-8").contains("Server started on port")&&System.currentTimeMillis()<end)Thread.sleep(1);
                out.put("port",server.getPort());out.put("listening",NIOClientOracle.get(server,"mServerSocket")!=null);out.put("cascade",ArchiveManagerOracle.wire(server.mCascade));server.shutdown();
                while(!((Listener)server).finished&&System.currentTimeMillis()<end)Thread.sleep(1);if(!((Listener)server).finished)throw new IllegalStateException("Listener did not stop");
            }else{
                server=(ArchiveServer)NIOClientOracle.U.allocateInstance(ArchiveServer.class);server.mMySQL=new MySQLConnect("fixture:3306","fixture","student","synthetic");server.mMySQL.init();
                if(Boolean.TRUE.equals(request.get("cascade"))){var spec=new JSONObject();spec.put("count",3);server.mCascade=ArchiveManagerOracle.cascade(spec);}
                config=request;events=new JSONArray();calls=0;
                if(op.equals("retry")){
                    if(Boolean.TRUE.equals(request.get("interrupt")))Thread.currentThread().interrupt();
                    var handler=server.new ArchiveHandler(null);var first=request.get("first");out.put("blocks",ArchiveManagerOracle.wires(handler.reconnectLoadTxBlocks(first==null?null:new MiniNumber((String)first))));
                }else{
                    if(!Boolean.TRUE.equals(request.get("nullSocket"))){
                        try(var listener=new ServerSocket(0,1,InetAddress.getLoopbackAddress())){client=new Socket(InetAddress.getLoopbackAddress(),listener.getLocalPort());accepted=listener.accept();}
                        client.setSoTimeout(50);client.getOutputStream().write(new MiniData(request.getString("frame")).getBytes());client.shutdownOutput();
                        if(Boolean.TRUE.equals(request.get("closed")))accepted.close();
                    }
                    final boolean nullRange=Boolean.TRUE.equals(request.get("nullRange"));
                    ArchiveServer.ArchiveHandler handler=nullRange?server.new ArchiveHandler(accepted){public synchronized ArrayList<TxBlock> reconnectLoadTxBlocks(MiniNumber first){return null;}}:server.new ArchiveHandler(accepted);
                    handler.run();out.put("closed",accepted==null?null:accepted.isClosed());var bytes=new ByteArrayOutputStream();
                    if(client!=null)try{int value;while((value=client.getInputStream().read())!=-1)bytes.write(value);}catch(SocketTimeoutException expected){}
                    out.put("reply",new MiniData(bytes.toByteArray()).to0xString());
                }
            }
        }catch(Exception error){out.put("exception",error.toString());}
        finally{if(request.containsKey("interrupt"))out.put("threadInterrupted",Thread.interrupted());if(accepted!=null)accepted.close();if(client!=null)client.close();System.setOut(previous);}
        var logs=new JSONArray();for(String line:captured.toString("UTF-8").split("\n")){int at=line.indexOf("] : ");if(at>=0&&!line.substring(at+4).startsWith("     "))logs.add(line.substring(at+4));}
        out.put("logs",logs);out.put("events",events);out.put("milliseconds",System.currentTimeMillis()-before);return out;
    }
    public static void main(String[] args)throws Exception{installDriver("jdbc:mysql:");var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)System.out.println(runArchive((JSONObject)new JSONParser().parse(line)).toJSONString());}
}
