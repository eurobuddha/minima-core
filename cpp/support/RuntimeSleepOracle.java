import java.io.*;
import org.minima.database.MinimaDB;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.system.network.minima.*;
import org.minima.system.commands.txn.txnlock;
import org.minima.utils.messages.*;
import org.minima.utils.json.*;

public class RuntimeSleepOracle extends NIOClientOracle {
    public static void main(String[] args)throws Exception{
        String mode=args[0];var out=new JSONObject();out.put("mode",mode);var saved=System.out;
        System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String text){System.err.println(text);if(mode.equals("restart_wait")&&text.contains("restart in 5 seconds"))Thread.currentThread().interrupt();}
        });
        GeneralParams.P2P_ENABLED=false;GeneralParams.P2P2_ENABLED=false;GeneralParams.RPC_ENABLED=false;GeneralParams.RPC_AUTHENTICATE=false;
        GeneralParams.NOCONNECT=true;GeneralParams.CONNECT_LIST="";GeneralParams.IS_HOST_SET=true;GeneralParams.MINIMA_HOST="127.0.0.1";GeneralParams.MINIMA_PORT=0;
        MinimaDB.createDB();var main=(Main)U.allocateInstance(Main.class);MainNetworkOracle.stack(main);field(Main.class,"mMainInstance").set(null,main);
        var fixture=new MainNetworkOracle(new JSONObject(),main);var nio=fixture.nio;var network=fixture.network;
        set(network,"mP2PManager",nio);set(network,"mP2P2Manager",nio);
        var miner=(MessageProcessor)U.allocateInstance(org.minima.system.brains.TxPoWMiner.class);MainNetworkOracle.stack(miner);set(main,"mTxPoWMiner",miner);
        var poll=(MessageProcessor)U.allocateInstance(org.minima.system.commands.sendpoll.SendPollManager.class);MainNetworkOracle.stack(poll);set(main,"mSendPoll",poll);
        TimerProcessor.createTimerProcessor();var timer=TimerProcessor.getTimerProcessor();timer.stop();((Thread)get(timer,"mMainThread")).join();
        if(mode.equals("restart_pause")){nio.stopMessageProcessor();nio.run();}
        Thread completed=null;
        Thread caller=Thread.currentThread();caller.interrupt();
        if(mode.equals("shutdown_wait")||mode.equals("restart_wait")){
            completed=new Thread(()->{try{while(caller.isInterrupted())Thread.sleep(1);nio.stopMessageProcessor();nio.run();}catch(Exception e){throw new RuntimeException(e);}});completed.start();
        }
        String error="none";
        try{
            if(mode.equals("shutdown_wait")){var method=Main.class.getDeclaredMethod("shutdownGenProcs");method.setAccessible(true);method.invoke(main);}
            else if(mode.equals("restart_wait")||mode.equals("restart_pause"))main.restartNIO();
            else if(mode.equals("netrestart")){var method=Main.class.getDeclaredMethod("processMessage",Message.class);method.setAccessible(true);method.invoke(main,new Message(Main.MAIN_NETRESTART));}
            else if(mode.equals("rpc")){var command=new org.minima.system.commands.network.rpc();command.getParams().put("enable","true");command.getParams().put("password","synthetic");command.runCommand();}
            else throw new IllegalArgumentException(mode);
        }catch(Exception e){Throwable cause=e instanceof java.lang.reflect.InvocationTargetException?e.getCause():e;error=cause.getClass().getName();}
        boolean pending=Thread.interrupted();if(completed!=null)completed.join();
        out.put("error",error);out.put("pending",pending);out.put("replaced",main.getNetworkManager()!=network);
        out.put("enabled",GeneralParams.RPC_ENABLED);out.put("authenticate",GeneralParams.RPC_AUTHENTICATE);
        out.put("locked",!MinimaDB.getDB().getRWLockInfo().contains("Read locks = 0]"));
        var current=main.getNetworkManager();if(current!=network){current.shutdownNetwork();for(var worker:new MessageProcessor[]{current.getNIOManager(),current.getP2PManager(),current.getP2P2Manager()})((Thread)get(worker,"mMainThread")).join(3000);}
        Main.ClearMainInstance();System.setOut(saved);System.out.println(out.toJSONString());
    }
}
