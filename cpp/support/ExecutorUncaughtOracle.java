import java.io.*;
import java.util.concurrent.*;
import org.minima.utils.json.*;
public class ExecutorUncaughtOracle {
    static JSONArray events=new JSONArray();static Thread original;static String kind;
    static synchronized void record(String value){events.add(value);}
    static Throwable failure(){return switch(kind){case "error"->new LinkageError("task");case "oom"->new OutOfMemoryError("task");case "io"->new IOException("task");default->new IllegalStateException("task");};}
    @SuppressWarnings("unchecked") static <T extends Throwable> void sneaky(Throwable e)throws T{throw (T)e;}
    static void await(CountDownLatch latch){try{if(!latch.await(5,TimeUnit.SECONDS))throw new AssertionError("timeout");}catch(InterruptedException e){throw new AssertionError(e);}}
    public static void main(String[] args)throws Exception{
        kind=args[0];boolean queued=Boolean.parseBoolean(args[1]);var first=new CountDownLatch(1);var releaseTask=new CountDownLatch(1);var second=new CountDownLatch(queued?1:0);var handling=new CountDownLatch(1);var releaseHandler=new CountDownLatch(1);var handled=new CountDownLatch(1);
        Thread.setDefaultUncaughtExceptionHandler((t,e)->record("STALE-HANDLER"));
        var pool=Executors.newFixedThreadPool(1);
        pool.execute(()->{original=Thread.currentThread();record("first");first.countDown();await(releaseTask);sneaky(failure());});await(first);
        if(queued)pool.execute(()->{record("replacement:"+(Thread.currentThread()!=original));second.countDown();});
        Thread.setDefaultUncaughtExceptionHandler((t,e)->{await(second);record("uncaught:"+t.getName()+":"+e.toString());handling.countDown();await(releaseHandler);record("handler-return");handled.countDown();if(kind.equals("defaultthrows"))throw new RuntimeException("default");});
        pool.shutdown();record("early-termination:"+pool.awaitTermination(0,TimeUnit.MILLISECONDS));releaseTask.countDown();await(handling);
        record("terminated-during-handler:"+pool.awaitTermination(5,TimeUnit.SECONDS));releaseHandler.countDown();await(handled);Thread.setDefaultUncaughtExceptionHandler(null);
        var result=new JSONObject();result.put("events",events);System.out.println(result.toJSONString());
    }
}
