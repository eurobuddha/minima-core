import java.io.*;
import java.util.*;
import org.minima.system.Main;
import org.minima.system.commands.*;
import org.minima.system.commands.send.sendpoll;
import org.minima.system.commands.sendpoll.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Real command/queue bodies; constructor bypass prevents background node work. */
public class SendPollCommandOracle {
    static sun.misc.Unsafe unsafe()throws Exception {var f=sun.misc.Unsafe.class.getDeclaredField("theUnsafe");f.setAccessible(true);return (sun.misc.Unsafe)f.get(null);}
    static void set(Class<?> type,Object target,String name,Object value)throws Exception {var f=type.getDeclaredField(name);f.setAccessible(true);f.set(target,value);}
    static class Poll extends SendPollManager {
        JSONArray timers;
        @Override public void PostTimerMessage(TimerMessage message){try{var f=TimerMessage.class.getDeclaredField("mDelay");f.setAccessible(true);var item=new JSONObject();item.put("type",message.getMessageType());item.put("delay",f.getLong(message));timers.add(item);}catch(Exception e){throw new RuntimeException(e);}}
        void poll()throws Exception {processMessage(new Message(SENDPOLL_FUNCTION));}
    }
    static JSONArray snapshot(List<SendPollMessage> messages){var result=new JSONArray();for(var message:messages)result.add(message.toJSON());return result;}
    static JSONObject run(JSONObject input)throws Exception {
        var manager=(Poll)unsafe().allocateInstance(Poll.class);manager.timers=new JSONArray();
        set(SendPollManager.class,manager,"mSyncObject",new Object());set(SendPollManager.class,manager,"mSendCommands",new ArrayList<SendPollMessage>());
        set(MessageProcessor.class,manager,"mRunning",true);set(MessageStack.class,manager,"mLock",new Object());
        var owner=(Main)unsafe().allocateInstance(Main.class);set(Main.class,owner,"mSendPoll",manager);set(Main.class,null,"mMainInstance",owner);
        ArrayList<SendPollMessage> held=new ArrayList<>();var outputs=new JSONArray();
        for(Object entry:(JSONArray)input.get("ops")){
            var op=(JSONObject)entry;var out=new JSONObject();String kind=(String)op.get("op");
            try {
                Object value=null;
                if(kind.equals("main")){set(Main.class,null,"mMainInstance",Boolean.TRUE.equals(op.get("missing"))?null:owner);set(Main.class,owner,"mSendPoll",Boolean.TRUE.equals(op.get("nullManager"))?null:manager);}
                else if(kind.equals("hold"))held=manager.listCommands();
                else if(kind.equals("stop"))manager.stopMessageProcessor();
                else if(kind.equals("poll"))manager.poll();
                else if(kind.equals("run"))value=CommandRunner.getRunner().runMultiCommand(op.containsKey("caller")?(String)op.get("caller"):"0x00",(String)op.get("text"));
                else if(kind.equals("meta")){
                    var command=new sendpoll();var result=new JSONObject();result.put("help",command.getFullHelp());result.put("valid",command.getValidParams());result.put("reply",command.getJSONReply());result.put("complete",command.getCompleteCommand());var fresh=command.getFunction();result.put("fresh",fresh!=command&&fresh.getParams()!=command.getParams());value=result;
                }else {
                    var command=new sendpoll();if(op.containsKey("text"))command.setCompleteCommand((String)op.get("text"));
                    var params=op.containsKey("params")?(JSONObject)op.get("params"):new JSONObject();
                    if(op.containsKey("removeIndex")){params=(JSONObject)params.clone();params.put("action","remove");params.put("uid",held.get(((Long)op.get("removeIndex")).intValue()).getUID());}
                    command.getParams().putAll(params);value=command.runCommand();
                }
                out.put("value",value);
            }catch(Exception e){out.put("exception",e.toString());}
            out.put("queue",snapshot(manager.listCommands()));out.put("held",snapshot(held));out.put("timers",new JSONArray(manager.timers));outputs.add(out);
        }
        var result=new JSONObject();result.put("ops",outputs);return result;
    }
    public static void main(String[] args)throws Exception {
        var reader=new BufferedReader(new InputStreamReader(System.in));String line;var stdout=System.out;
        while((line=reader.readLine())!=null){JSONObject result;try{System.setOut(new PrintStream(OutputStream.nullOutputStream()));result=run((JSONObject)new JSONParser().parse(line));}finally{System.setOut(stdout);}stdout.println(result.toJSONString());}
    }
}
