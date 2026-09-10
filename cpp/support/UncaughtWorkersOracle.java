import java.io.*;
import java.util.*;
import org.minima.database.MinimaDB;
import org.minima.utils.*;
import org.minima.utils.json.*;
import org.minima.utils.messages.*;

public class UncaughtWorkersOracle extends NIOClientOracle {
    static java.util.concurrent.CountDownLatch started=new java.util.concurrent.CountDownLatch(1);
    static String mode;static JSONArray events=new JSONArray();static Worker worker;
    static void record(String value){events.add(value);}
    static class DB extends MinimaDB {
        public void safeReleaseReadWriteLock(){record("cleanup");if(mode.equals("cleanup")||mode.equals("logandcleanup"))throw new IllegalStateException("cleanup");if(!mode.equals("catchlog"))worker.stopMessageProcessor();}
    }
    static class Worker extends MessageProcessor {
        Worker(){super("worker");}
        protected void processMessage(Message msg)throws Exception{
            record("process");switch(mode){
                case "exception":case "catchlog":case "logandcleanup":throw new RuntimeException("handler");
                case "store":throw new org.h2.mvstore.MVStoreException(1,"handler");
                case "timeout":throw new java.util.concurrent.TimeoutException("handler");
                case "error":throw new LinkageError("handler");
                case "oom":throw new OutOfMemoryError("handler");
                case "handlerthrows":throw new RuntimeException("handler");
            }
        }
    }
    public static void main(String[] args)throws Exception{
        mode=args[0];var saved=System.out;
        System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String full){String value=full.substring(full.indexOf(" : ")+3);if(value.startsWith("     "))return;record("log:"+value);if(value.endsWith("START"))started.countDown();
                if(((mode.equals("catchlog")||mode.equals("logandcleanup")||mode.equals("handlerthrows"))&&value.startsWith("MESSAGE PROCESSING"))||(mode.equals("startlog")&&value.endsWith("START"))||(mode.equals("endlog")&&value.endsWith("SHUTDOWN")))throw new IllegalArgumentException("logger");
            }
        });
        Thread.setDefaultUncaughtExceptionHandler((t,e)->{record("uncaught:"+t.getName()+":"+e.toString());if(mode.equals("handlerthrows"))throw new RuntimeException("default");});
        if(mode.equals("timer")){
            TimerProcessor.createTimerProcessor();var timer=TimerProcessor.getTimerProcessor();timer.PostMessage(new TimerMessage(-100,"ORPHAN"));((Thread)get(timer,"mMainThread")).join(5000);if(((Thread)get(timer,"mMainThread")).isAlive())throw new AssertionError("timer alive");record("pending:"+timer.getSize());record("running:"+get(timer,"mRunning"));TimerProcessor.stopTimerProcessor();
        }else{
            field(MinimaDB.class,"mMinimaDB").set(null,U.allocateInstance(DB.class));MessageProcessor.setTrace(mode.endsWith("log")&&(mode.startsWith("start")||mode.startsWith("end")),"");
            worker=new Worker();if(mode.equals("endlog"))started.await();if(!mode.equals("startlog"))worker.PostMessage("TEST");Thread thread=(Thread)get(worker,"mMainThread");thread.join(5000);if(thread.isAlive()){worker.stopMessageProcessor();throw new AssertionError("worker alive");}
            record("running:"+worker.isRunning());record("shutdown:"+worker.isShutdownComplete());record("last:"+(worker.getLastMessage()==null?"null":worker.getLastMessage().getMessageType()));worker.stopMessageProcessor();
        }
        Thread.setDefaultUncaughtExceptionHandler(null);System.setOut(saved);var result=new JSONObject();result.put("events",events);System.out.println(result.toJSONString());
    }
}
