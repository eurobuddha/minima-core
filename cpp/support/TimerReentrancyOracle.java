import java.io.*;
import java.util.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;
public class TimerReentrancyOracle extends NIOClientOracle {
    static class Target extends MessageProcessor {Target(){super("unused");}protected void processMessage(Message message){}}
    static class Probe extends TimerMessage {
        Runnable action;Probe(String name,long deadline,Runnable callback)throws Exception{super(0,name);set(this,"mTimer",deadline);action=callback;}
        public String toString(){if(action!=null)action.run();return getMessageType();}
    }
    static JSONArray names(ArrayList<TimerMessage> values){var out=new JSONArray();for(var msg:values)out.add(msg==null?null:msg.getMessageType());return out;}
    public static void main(String[] args)throws Exception{
        String mode=args[0],action=args[1];int count=Integer.parseInt(args[2]);var timer=(TimerProcessor)U.allocateInstance(TimerProcessor.class);set(timer,"mRunning",true);set(timer,"mMessagesLock",new Object());set(timer,"mTimerMessages",new ArrayList<TimerMessage>());
        var target=(Target)U.allocateInstance(Target.class);var events=new JSONArray();var logs=new JSONArray();boolean[] fired={false};var added=new Probe("C",Long.MAX_VALUE,null);added.setProcessor(target);
        Runnable callback=()->{
            if(fired[0])return;fired[0]=true;events.add("size:"+timer.getSize());
            if(action.equals("append"))timer.PostMessage(added);
            else if(action.equals("throw"))throw new IllegalStateException("callback");
            else if(action.equals("print"))timer.printAllMessages();
            else if(action.equals("run")){Thread.currentThread().interrupt();timer.run();events.add("nested:"+timer.getSize());if(mode.equals("run"))Thread.currentThread().interrupt();}
            else if(action.equals("nullpost"))timer.PostMessage(null);
            events.add("after:"+timer.getSize());
        };
        var output=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String text){text=text.substring(text.indexOf(" : ")+3);if(text.startsWith("     "))return;logs.add(text);if(action.equals("log_append")&&!fired[0]){fired[0]=true;events.add("size:"+timer.getSize());timer.PostMessage(added);events.add("after:"+timer.getSize());}}
        });
        for(int i=0;i<count;i++){var probe=new Probe(i==0?"A":"B",i==0?0:Long.MAX_VALUE,action.equals("log_append")?null:callback);probe.setProcessor(target);timer.PostMessage(probe);}
        var held=(ArrayList<TimerMessage>)get(timer,"mTimerMessages");if(action.equals("nullentry"))held.add(null);String error="none";
        if(mode.equals("run"))Thread.currentThread().interrupt();try{if(mode.equals("run"))timer.run();else timer.printAllMessages();}catch(Exception e){error=e.getClass().getName();}
        var result=new JSONObject();result.put("error",error);result.put("pending",Thread.interrupted());result.put("running",get(timer,"mRunning"));result.put("events",events);result.put("logs",logs);var current=(ArrayList<TimerMessage>)get(timer,"mTimerMessages");result.put("remaining",names(current));result.put("original",names(held));result.put("sameList",current==held);output.println(result);
    }
}
