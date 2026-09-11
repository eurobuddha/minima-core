import java.util.concurrent.CountDownLatch;
public class MinimaShutdownHooksOracle {
    public static void main(String[] args)throws Exception {
        String mode=args[0];int count=Integer.parseInt(args[1]);
        var started=new CountDownLatch(count);
        Thread.setDefaultUncaughtExceptionHandler((thread,error)->System.out.println("uncaught:"+thread.getName()+":"+error));
        for(int i=0;i<count;i++){
            final int index=i;
            Runtime.getRuntime().addShutdownHook(new Thread(()->{
                started.countDown();
                try{started.await();}catch(InterruptedException e){throw new RuntimeException(e);}
                System.out.println("hook:"+index+":"+Thread.currentThread().getName());
                if(mode.equals("throw")&&index==0)throw new IllegalStateException("fixture hook");
                if(mode.equals("late")&&index==0){
                    try{Runtime.getRuntime().addShutdownHook(new Thread(()->{}));}
                    catch(IllegalStateException e){System.out.println("late:"+e.getClass().getName());}
                }
            }));
        }
        System.out.println("READY");
        if(mode.equals("signal")){Thread.sleep(60000);throw new AssertionError("signal not delivered");}
        if(mode.equals("halt"))Runtime.getRuntime().halt(7);
        if(mode.equals("exit"))System.exit(9);
    }
}
