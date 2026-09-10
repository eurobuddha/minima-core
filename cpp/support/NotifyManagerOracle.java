import java.io.*;
import java.net.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.UserDB;
import org.minima.system.Main;
import org.minima.system.commands.CommandRunner;
import org.minima.system.commands.network.webhooks;
import org.minima.system.network.webhooks.NotifyManager;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
import org.minima.utils.messages.*;
// Pinned NotifyManager and RPCClient execute unchanged. Only HttpURLConnection
// and Main receivers are controlled; no remote hook is contacted by this fixture.
public class NotifyManagerOracle extends NIOClientOracle {
    static JSONObject command;static JSONArray trace,logs;static UserDB[] users;static int dbIndex,sends;static Manager manager;static ArrayList<JSONObject> notifications;
    static boolean flag(String key){return command!=null&&Boolean.TRUE.equals(command.get(key));}
    static int num(String key,int fallback){return command.containsKey(key)?((Number)command.get(key)).intValue():fallback;}
    static void record(Object...values){trace.add(new JSONArray(Arrays.asList(values)));}
    static void mutate(String where){String action=command.getString(where,"");switch(action){case "clear":manager.clearHooks();break;case "add":manager.addHook((String)command.get("hook"));break;case "remove":manager.removeHook((String)command.get("hook"));break;case "stop":manager.shutDown();break;}}
    static class DB extends MinimaDB {
        public UserDB getUserDB(){record("user",dbIndex);if(flag("failDB"))throw new RuntimeException("fixture DB failure");return flag("nullDB")?null:users[dbIndex];}
    }
    static class Owner extends Main {
        Owner()throws Exception{super();}
        public NotifyManager getNotifyManager(){record("manager");return flag("nullManager")?null:manager;}
        public void PostNotifyEvent(String name,JSONObject data){if(name.equals("MINIMALOG")&&flag("logFailure"))throw new RuntimeException("fixture log failure");}
    }
    static class Manager extends NotifyManager {
        void process(Message msg)throws Exception{super.processMessage(msg);}
        Message next(){return getNextMessage();}
    }
    static class Connection extends HttpURLConnection {
        ByteArrayOutputStream body=new ByteArrayOutputStream();
        Connection(URL url){super(url);}
        public void disconnect(){}public boolean usingProxy(){return false;}public void connect(){}
        public OutputStream getOutputStream(){return body;}
        public int getResponseCode()throws IOException{record("post",url.toString(),body.toString("UTF-8"));if(++sends==1)mutate("postAction");if(flag("postFailure"))throw new IOException("fixture POST failure");return 200;}
        public InputStream getInputStream(){return new ByteArrayInputStream(new byte[0]);}
    }
    static void installHTTP() {
        URL.setURLStreamHandlerFactory(protocol->protocol.equals("http")?new URLStreamHandler(){protected URLConnection openConnection(URL url){return new Connection(url);}}:null);
    }
    static ArrayList<String> strings(JSONArray array){var list=new ArrayList<String>();for(Object v:array)list.add((String)v);return list;}
    static Object run(JSONObject input)throws Exception {
        command=null;trace=new JSONArray();logs=new JSONArray();users=new UserDB[]{new UserDB(),new UserDB()};dbIndex=0;users[0].setWebHooks(strings((JSONArray)input.get("hooks")));
        notifications=new ArrayList<>();for(Object v:(JSONArray)input.get("notifications"))notifications.add((JSONObject)v);
        field(MinimaDB.class,"mMinimaDB").set(null,new DB());field(Main.class,"mMainInstance").set(null,U.allocateInstance(Owner.class));
        manager=(Manager)U.allocateInstance(Manager.class);set(manager,"mLock",new Object());set(manager,"mMessages",new LinkedList<Message>());set(manager,"mRunning",true);set(manager,"mHooks",MinimaDB.getDB().getUserDB().getWebHooks());
        MessageListener listener=msg->{record("listener",msg.toString());var n=(JSONObject)msg.getObject("notify");if(command.containsKey("listenerEvent"))n.put("event",command.get("listenerEvent"));mutate("listenerAction");if(command.containsKey("replace"))msg.addObject("notify",notifications.get(num("replace",0)));if(flag("listenerFailure"))throw new RuntimeException("fixture listener failure");};
        var held=new ArrayList<Message>();var output=new JSONArray();
        for(Object raw:(JSONArray)input.get("ops")){
            command=(JSONObject)raw;trace=new JSONArray();logs=new JSONArray();sends=0;manager.WEBHOOKS_ERROR_LOGS=flag("logs");Object prior=users[dbIndex].getAllData().get("webhooks");int priorDB=dbIndex;var row=new JSONObject();
            // Main.getMinimaListener is a static field getter with no interception.
            Main.setMinimaListener(flag("listener")?listener:null);
            var previous=System.out;
            System.setOut(new PrintStream(new OutputStream(){ByteArrayOutputStream bytes=new ByteArrayOutputStream();public void write(int b){if(b!=10){bytes.write(b);return;}String line=bytes.toString(java.nio.charset.StandardCharsets.UTF_8);bytes.reset();int index=line.indexOf("] : ");if(index>=0){String text=line.substring(index+4);boolean notify=!text.startsWith("ERROR webhook : ");logs.add(new JSONArray(Arrays.asList(text,notify)));record("log",text,notify);}}},true,"UTF-8"));
            try{Object result=null;int index=num("event",0);JSONObject notification=index<0?null:notifications.get(index);switch((String)command.get("cmd")){
                case "command":result=CommandRunner.getRunner().runMultiCommand(command.getString("caller","0x00"),command.getString("text"));break;
                case "direct":{var action=new webhooks();action.getParams().putAll((JSONObject)command.get("params"));result=action.runCommand();break;}
                case "meta":{var copy=new webhooks().getFunction();var meta=new JSONObject();meta.put("name",copy.getName());meta.put("help",copy.getHelp());meta.put("full",copy.getFullHelp());meta.put("valid",new JSONArray(copy.getValidParams()));result=meta;break;}
                case "add":manager.addHook((String)command.get("hook"));break;case "remove":manager.removeHook((String)command.get("hook"));break;case "clear":manager.clearHooks();break;case "get":result=new JSONArray(manager.getAllWebHooks());break;
                case "queue":manager.PostEvent(notification);break;case "next":{var msg=manager.next();held.add(msg);manager.process(msg);break;}
                case "process":{var msg=new Message(NotifyManager.NOTIFY_POST).addObject("notify",notification);held.add(msg);manager.process(msg);break;}
                case "unknown":manager.process(new Message("UNKNOWN"));break;case "wrong":manager.process(new Message(NotifyManager.NOTIFY_POST).addObject("notify",Long.valueOf(1)));break;
                case "mutate":notification.put("event",command.get("value"));break;case "stop":manager.shutDown();break;case "rebind":dbIndex=num("db",0);break;case "setDB":users[dbIndex].setWebHooks(strings((JSONArray)command.get("hooks")));break;case "copy":{var copy=manager.getAllWebHooks();copy.clear();result=new JSONArray(copy);break;}default:throw new IllegalArgumentException((String)command.get("cmd"));
            }row.put("value",result);}catch(Exception e){row.put("exception",e.toString());}finally{System.setOut(previous);}
            row.put("errorlogs",manager.WEBHOOKS_ERROR_LOGS);row.put("trace",trace);row.put("logs",logs);row.put("running",manager.isRunning());try{row.put("hooks",new JSONArray(manager.getAllWebHooks()));}catch(Exception e){row.put("hooksError",e.toString());}
            var stored=new JSONArray();for(var db:users)stored.add(new JSONObject(db.getAllData()));row.put("db",stored);row.put("dbIndex",dbIndex);row.put("replaced",prior!=users[priorDB].getAllData().get("webhooks"));
            var pending=new JSONArray();for(var msg:(LinkedList<Message>)get(manager,"mMessages"))pending.add(msg==null?null:msg.toString());var retained=new JSONArray();for(var msg:held)retained.add(msg==null?null:msg.toString());var state=new JSONArray();for(var n:notifications)state.add(n==null?null:new JSONObject(n));row.put("pending",pending);row.put("retained",retained);row.put("notifications",state);output.add(row);
        }
        Main.setMinimaListener(null);manager.shutDown();var result=new JSONObject();result.put("ops",output);return result;
    }
    public static void main(String[] args)throws Exception{installHTTP();var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception e){e.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
