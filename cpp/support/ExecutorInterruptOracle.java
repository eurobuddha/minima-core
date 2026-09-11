import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.locks.ReentrantLock;
import org.minima.system.Main;
import org.minima.system.network.minima.*;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;

public class ExecutorInterruptOracle extends NIOClientOracle {
    static void require(boolean value,String text){if(!value)throw new AssertionError(text);}
    static void await(CountDownLatch latch)throws Exception{require(latch.await(5,TimeUnit.SECONDS),"fixture latch timeout");}
    static JSONObject result(ExecutorService pool,long millis){
        Object value=null;String error="none";
        try{value=pool.awaitTermination(millis,TimeUnit.MILLISECONDS);}catch(InterruptedException e){error=e.toString();}
        var out=new JSONObject();out.put("value",value);out.put("error",error);out.put("pending",Thread.interrupted());return out;
    }
    static class HeldTask {
        final CountDownLatch started=new CountDownLatch(1),release=new CountDownLatch(1);
        void start(ExecutorService pool)throws Exception{pool.execute(()->{started.countDown();try{release.await();}catch(InterruptedException e){throw new AssertionError(e);}});await(started);}
    }
    static class Waiter {
        final CompletableFuture<JSONObject> result=new CompletableFuture<>();Thread thread;volatile boolean atFinal;
        void start(ExecutorService pool,boolean reuse){thread=new Thread(()->{try{if(reuse)for(int i=0;i<3;i++)require(!pool.awaitTermination(1,TimeUnit.MILLISECONDS),"unexpected early termination");atFinal=true;result.complete(result(pool,Long.MAX_VALUE));}catch(Throwable e){result.completeExceptionally(e);}});thread.start();}
        void awaitRegistration()throws Exception{long end=System.nanoTime()+5_000_000_000L;while(!(atFinal&&thread.getState()==Thread.State.TIMED_WAITING)){require(!result.isDone(),"waiter returned early");require(System.nanoTime()<end,"wait registration timeout");Thread.onSpinWait();}}
        JSONObject finish()throws Exception{return result.get(5,TimeUnit.SECONDS);}
    }
    public static void main(String[] args)throws Exception{
        String mode=args[0];var out=new JSONObject();var output=System.out;
        // Preserve runtime diagnostics without mixing them into the JSON record.
        System.setOut(new java.io.PrintStream(java.io.OutputStream.nullOutputStream()){
            public void println(String text){System.err.println(text);}
        });
        if(mode.equals("matrix")){
            String state=args[1];long millis=Long.parseLong(args[2]);boolean pending=Boolean.parseBoolean(args[3]);var pool=Executors.newFixedThreadPool(1);var task=new HeldTask();
            if(state.equals("held"))task.start(pool);if(!state.equals("open"))pool.shutdown();if(pending)Thread.currentThread().interrupt();out=result(pool,millis);
            if(state.equals("held"))task.release.countDown();pool.shutdown();require(pool.awaitTermination(5,TimeUnit.SECONDS),"matrix cleanup timeout");
        }else if(mode.startsWith("nio_")){
            boolean held=mode.endsWith("held"),hard=mode.startsWith("nio_hard");var main=(Main)U.allocateInstance(Main.class);field(Main.class,"mMainInstance").set(null,main);
            var manager=(NIOManager)U.allocateInstance(NIOManager.class);set(manager,"mMessages",new LinkedList<Message>());set(manager,"mLock",new Object());set(manager,"mRunning",true);
            var pool=Executors.newFixedThreadPool(1);set(manager,"THREAD_POOL",pool);var server=new NIOServer(0,manager);set(manager,"mNIOServer",server);var task=new HeldTask();if(held)task.start(pool);Thread.currentThread().interrupt();String error="none";
            try{if(hard)manager.hardShutDown();else{var method=NIOManager.class.getDeclaredMethod("processMessage",Message.class);method.setAccessible(true);method.invoke(manager,new Message(NIOManager.NIO_SHUTDOWN));}}
            catch(Exception e){Throwable cause=e instanceof java.lang.reflect.InvocationTargetException?e.getCause():e;error=cause.toString();}
            out.put("error",error);out.put("pending",Thread.interrupted());out.put("poolShutdown",pool.isShutdown());out.put("serverShutdown",get(server,"mShutDown"));out.put("running",manager.isRunning());
            if(held)task.release.countDown();pool.shutdown();require(pool.awaitTermination(5,TimeUnit.SECONDS),"NIO cleanup timeout");manager.stopMessageProcessor();Main.ClearMainInstance();
        }else{
            var pool=(ThreadPoolExecutor)Executors.newFixedThreadPool(1);var waiters=new ArrayList<Waiter>();int count=mode.equals("multiple")?4:1;
            for(int i=0;i<count;i++){var waiter=new Waiter();waiter.start(pool,mode.equals("reuse"));waiter.awaitRegistration();waiters.add(waiter);}
            out.put("blocked",true);
            if(mode.equals("signal_first")){
                var lock=(ReentrantLock)field(ThreadPoolExecutor.class,"mainLock").get(pool);lock.lock();try{pool.shutdown();waiters.get(0).thread.interrupt();}finally{lock.unlock();}
            }else if(mode.equals("complete"))pool.shutdown();
            else{
                if(mode.equals("reinterrupt")){var lock=(ReentrantLock)field(ThreadPoolExecutor.class,"mainLock").get(pool);lock.lock();try{var thread=waiters.get(0).thread;thread.interrupt();long end=System.nanoTime()+5_000_000_000L;while(thread.isInterrupted()){require(System.nanoTime()<end,"first interrupt not consumed before reacquisition");Thread.onSpinWait();}thread.interrupt();}finally{lock.unlock();}}
                else waiters.get(0).thread.interrupt();
                waiters.get(0).finish();out.put("beforeShutdown",!pool.isShutdown());if(count>1){boolean blocked=true;for(int i=1;i<count;i++)blocked&=!waiters.get(i).result.isDone();out.put("othersBlocked",blocked);}pool.shutdown();
            }
            var rows=new JSONArray();for(var waiter:waiters){rows.add(waiter.finish());waiter.thread.join();}out.put("results",rows);require(pool.awaitTermination(5,TimeUnit.SECONDS),"cleanup timeout");
        }
        output.println(out.toJSONString());
    }
}
