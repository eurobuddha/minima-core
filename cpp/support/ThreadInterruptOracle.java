import java.io.*;
import java.util.concurrent.*;
import org.minima.database.MinimaDB;
import org.minima.system.commands.txn.txnlock;
import org.minima.system.commands.sendpoll.SendPollManager;
import org.minima.utils.json.*;
import org.minima.utils.messages.*;

public class ThreadInterruptOracle extends NIOClientOracle {
    static int cleanup;
    static class DB extends MinimaDB {
        public void safeReleaseReadWriteLock(){++cleanup;}
    }
    static String sleep(long delay){
        try {Thread.sleep(delay);return "returned";}
        catch(Exception e){return e.getClass().getName();}
    }
    static class Worker extends MessageProcessor {
        final String mode;
        final CountDownLatch entered=new CountDownLatch(1);
        String error="none";boolean pending,returned;
        Worker(String value){super("interrupt-worker");mode=value;}
        protected void processMessage(Message message)throws Exception{
            entered.countDown();
            try {
                if(mode.equals("hard_lock"))new txnlock().getLock(0);
                else if(mode.equals("hard_unlock")){
                    var command=new txnlock();command.getParams().put("action","unlock");
                    command.getParams().put("unlockdelay",Long.toString(Long.MAX_VALUE));command.runCommand();
                }else Thread.sleep(mode.equals("soft_sleep")?500:Long.MAX_VALUE);
                returned=true;
            }catch(InterruptedException e){error=e.getClass().getName();pending=Thread.currentThread().isInterrupted();throw e;}
        }
    }
    public static void main(String[] args)throws Exception{
        var saved=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()));
        String mode=args[0];var result=new JSONObject();result.put("mode",mode);
        if(mode.equals("flags")){
            var rows=new JSONArray();
            for(long delay:new long[]{-1,Long.MIN_VALUE,0,1,Long.MAX_VALUE}){
                Thread.currentThread().interrupt();Thread.currentThread().interrupt();
                var row=new JSONObject();row.put("delay",Long.toString(delay));row.put("error",sleep(delay));
                row.put("pending",Thread.currentThread().isInterrupted());row.put("consume",Thread.interrupted());row.put("again",Thread.interrupted());rows.add(row);
            }
            result.put("rows",rows);result.put("zero",sleep(0));result.put("positive",sleep(2));
        }else if(mode.equals("external")){
            var entered=new CountDownLatch(1);var done=new CountDownLatch(1);String[] outcome={"none"};boolean[] pending={true};
            var thread=new Thread(()->{entered.countDown();outcome[0]=sleep(Long.MAX_VALUE);pending[0]=Thread.currentThread().isInterrupted();done.countDown();});
            thread.start();entered.await();result.put("blocked",!done.await(25,TimeUnit.MILLISECONDS));thread.interrupt();thread.join(3000);
            if(thread.isAlive())throw new AssertionError("sleep did not interrupt");
            result.put("error",outcome[0]);result.put("pending",pending[0]);
        }else if(mode.equals("lock_flags")){
            txnlock.lockFunction(false);Thread.currentThread().interrupt();result.put("acquired",new txnlock().getLock(-1));result.put("pending_after_acquire",Thread.currentThread().isInterrupted());
            var command=new txnlock();command.getParams().put("action","unlock");command.getParams().put("unlockdelay","0");command.runCommand();
            result.put("locked",txnlock.mLocked);result.put("pending_after_unlock",Thread.interrupted());
        }else if(mode.startsWith("timer_")){
            TimerProcessor.createTimerProcessor();var timers=TimerProcessor.getTimerProcessor();var called=new CountDownLatch(1);int[] calls={0};boolean[] before={true},after={false};
            System.setOut(new PrintStream(OutputStream.nullOutputStream()){
                public void println(String text){if(!text.contains("Timer Message NOT run")&&!text.contains("Timer Message is NULL"))return;calls[0]++;before[0]=Thread.currentThread().isInterrupted();if(mode.equals("timer_self"))Thread.currentThread().interrupt();else timers.stop();after[0]=Thread.currentThread().isInterrupted();called.countDown();}
            });
            var target=(Worker)U.allocateInstance(Worker.class);
            if(mode.equals("timer_null")){synchronized(get(timers,"mMessagesLock")){((java.util.ArrayList<TimerMessage>)get(timers,"mTimerMessages")).add(null);}}
            else{var message=new TimerMessage(-1,"TIMER_PROBE");message.setProcessor(target);timers.PostMessage(message);}
            if(!called.await(3,TimeUnit.SECONDS))throw new AssertionError("timer callback timeout");var thread=(Thread)get(timers,"mMainThread");thread.join(3000);if(thread.isAlive())throw new AssertionError("timer still alive");result.put("calls",calls[0]);result.put("before",before[0]);result.put("after",after[0]);result.put("pending",thread.isInterrupted());result.put("running",get(timers,"mRunning"));result.put("remaining",timers.getSize());
        }else if(mode.equals("hard_poll")){
            field(MinimaDB.class,"mMinimaDB").set(null,U.allocateInstance(DB.class));TimerProcessor.createTimerProcessor();var timers=TimerProcessor.getTimerProcessor();
            var worker=new SendPollManager();worker.addSendCommand("hash data:interrupt-fixture");worker.PostMessage(SendPollManager.SENDPOLL_FUNCTION);
            long deadline=System.nanoTime()+TimeUnit.SECONDS.toNanos(3);
            while(!worker.listCommands().isEmpty()&&System.nanoTime()<deadline)Thread.sleep(1);
            worker.waitToShutDown(0);var thread=(Thread)get(worker,"mMainThread");thread.join(3000);if(thread.isAlive())throw new AssertionError("poll worker alive");
            timers.stop();((Thread)get(timers,"mMainThread")).join(3000);
            result.put("remaining",worker.listCommands().size());result.put("timers",timers.getSize());result.put("cleanup",cleanup);
            result.put("running",worker.isRunning());result.put("shutdown",worker.isShutdownComplete());
        }else{
            field(MinimaDB.class,"mMinimaDB").set(null,U.allocateInstance(DB.class));txnlock.lockFunction(true);
            var worker=new Worker(mode);worker.PostMessage("TEST");if(!worker.entered.await(3,TimeUnit.SECONDS))throw new AssertionError("not entered");
            if(mode.equals("soft_sleep"))worker.stopMessageProcessor();
            else {if(mode.equals("interrupted_waiter"))Thread.currentThread().interrupt();worker.waitToShutDown(0);}
            var thread=(Thread)get(worker,"mMainThread");thread.join(3000);if(thread.isAlive())throw new AssertionError("worker not stopped");
            result.put("error",worker.error);result.put("pending",worker.pending);result.put("returned",worker.returned);
            result.put("cleanup",cleanup);result.put("locked",txnlock.mLocked);result.put("running",worker.isRunning());result.put("shutdown",worker.isShutdownComplete());
            result.put("waiter_pending",Thread.interrupted());txnlock.lockFunction(false);
        }
        System.setOut(saved);System.out.println(result.toJSONString());
    }
}
