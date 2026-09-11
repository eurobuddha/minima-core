import java.io.*;
import java.util.*;
import java.util.concurrent.Semaphore;
import org.minima.database.MinimaDB;
import org.minima.database.txpowtree.TxPoWTreeNode;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.system.brains.*;
import org.minima.system.params.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

/** Actual checker and worker, with source-generated serialized block fixtures. */
public class TimedCheckerErrorsOracle extends NIOClientOracle {
    static JSONArray events=new JSONArray();static int prints;static String[] failures;static boolean armed,stalled,workerHeld;static Semaphore releaseWorker=new Semaphore(0);
    static synchronized void record(String text){events.add(text);}
    static Throwable failure(String kind){return switch(kind){case "error"->new LinkageError("logger");case "oom"->new OutOfMemoryError("logger");case "io"->new IOException("logger");default->new IllegalStateException("logger");};}
    @SuppressWarnings("unchecked") static <T extends Throwable> void fail(Throwable e)throws T{throw (T)e;}
    static TxPoWTreeNode tip(TxPoW parent){var tip=new TxPoWTreeNode(parent);tip.getMMR().addEntry(MMRData.CreateMMRDataLeafNode(MiniNumber.ONE,MiniNumber.ONE));tip.getMMR().finalizeSet();MinimaDB.getDB().getTxPoWTree().setRoot(tip);return tip;}
    static JSONObject fixture()throws Exception{
        MinimaDB.createDB();GeneralParams.USE_SQL_COINDB=false;
        var parent=new TxPoW();parent.setBlockNumber(MiniNumber.ONE);parent.setTimeMilli(MiniNumber.ZERO);parent.setBlockDifficulty(Magic.MIN_TXPOW_WORK);for(int i=0;i<GlobalParams.MINIMA_CASCADE_LEVELS;i++)parent.setSuperParent(i,MiniData.ZERO_TXPOWID);parent.setHeaderBodyHash();parent.calculateTXPOWID();tip(parent);MinimaDB.getDB().getUserDB().setHashRate(new MiniNumber(1000000));
        var block=TxPoWGenerator.generateTxPoW(new Transaction(),new Witness());var out=new JSONObject();out.put("parent",ConsensusOracle.wire(parent));out.put("block",ConsensusOracle.wire(block));out.put("duplicate",ConsensusOracle.fixture(4,"RETURN TRUE",3,1024).split(" ")[1]);return out;
    }
    public static void main(String[] args)throws Exception{
        var output=System.out;System.setOut(new PrintStream(OutputStream.nullOutputStream()){
            public void println(String full){int index;boolean delay;synchronized(TimedCheckerErrorsOracle.class){if(!armed)return;String text=full.substring(full.indexOf(" : ")+3);if(text.startsWith("     "))return;record("log:"+text);index=prints++;delay=stalled&&!workerHeld&&Thread.currentThread().getName().startsWith("Thread-");if(delay)workerHeld=true;}if(delay){releaseWorker.acquireUninterruptibly();record("workerInterrupted:"+Thread.currentThread().isInterrupted());}if(index<failures.length&&!failures[index].equals("none"))fail(failure(failures[index]));}
        });
        if(args[0].equals("fixture")){var data=fixture();output.println(data.toJSONString());return;}
        String mode=args[0],mutation=args[1];failures=args[2].split(",");boolean blockLogs=Boolean.parseBoolean(args[3]);
        var data=(JSONObject)new JSONParser().parse(new BufferedReader(new InputStreamReader(System.in)).readLine());MinimaDB.createDB();GeneralParams.USE_SQL_COINDB=false;GeneralParams.BLOCK_LOGS=false;
        var parent=ConsensusOracle.txpow(data.getString("parent"));var tip=tip(parent);var block=ConsensusOracle.txpow(data.getString("block"));var transactions=new ArrayList<TxPoW>();var checker=new TimedChecker();
        boolean timed=!mode.equals("block")&&!mode.equals("only");stalled=mode.equals("stalled")||mode.equals("interrupted");
        if(timed&&!mode.equals("fresh")){boolean prime=mutation.equals("valid")||mode.equals("primed")||stalled;var chain=block.getTxHeader().mChainID;if(!prime)block.getTxHeader().mChainID=new MiniData("0xFE");if(checker.checkTxPoWBlock(tip,block,transactions)!=prime)throw new AssertionError("unexpected prime result");((Thread)get(checker,"mCheckerThread")).join();if(!((Boolean)get(checker,"mFinishedRunning")))throw new AssertionError("prime not finished");block.getTxHeader().mChainID=chain;}
        switch(mutation){
            case "chain":block.getTxHeader().mChainID=new MiniData("0xFF");break;
            case "height":block.setBlockNumber(new MiniNumber(99));break;
            case "parents":block.setSuperParent(0,new MiniData("0xFF"));break;
            case "time-before":block.setTimeMilli(new MiniNumber(-1));break;
            case "time-after":block.setTimeMilli(new MiniNumber(86400001));break;
            case "difficulty":block.setBlockDifficulty(new MiniData("0x01"));break;
            case "magic":block.getMagic().mCurrentMaxTxnPerBlock=new MiniNumber(7);break;
            case "count":for(int i=0;i<=block.getMagic().getMaxNumTxns().getAsInt();i++)block.addBlockTxPOW(MiniData.ZERO_TXPOWID);break;
            case "empty-mmr":set(tip,"mMMR",new MMR());break;
            case "root":block.setMMRRoot(new MiniData("0xFF"));break;
            case "duplicate":case "duplicate-null":transactions.add(ConsensusOracle.txpow(data.getString("duplicate")));if(mutation.equals("duplicate-null"))transactions.add(null);break;
            case "null-block":block=null;break;case "null-parent":tip=null;break;case "null-transactions":transactions=null;break;case "null-transaction":transactions.add(null);break;
        }
        armed=true;GeneralParams.BLOCK_LOGS=blockLogs;Thread.setDefaultUncaughtExceptionHandler((thread,error)->record("uncaught:"+thread.getName()+":"+error));long started=System.nanoTime();Object returned;
        if(mode.equals("interrupted"))Thread.currentThread().interrupt();
        try{returned=timed?checker.checkTxPoWBlock(tip,block,transactions):mode.equals("only")?TxPoWChecker.checkTxBlockOnly(tip,new TxBlock(block)):TxPoWChecker.checkTxPoWBlock(tip,block,transactions);}catch(Throwable e){returned="escaped:"+e;}
        long elapsed=(System.nanoTime()-started)/1000000;boolean pending=mode.equals("interrupted")?Thread.interrupted():false;if(stalled)releaseWorker.release();
        if(timed){var worker=(Thread)get(checker,"mCheckerThread");worker.join(5000);if(worker.isAlive())throw new AssertionError("worker not complete");}
        var out=new JSONObject();if(mode.equals("interrupted"))out.put("threadInterrupted",pending);out.put("events",events);out.put("returned",returned);if(timed){out.put("finished",get(checker,"mFinishedRunning"));out.put("valid",get(checker,"mValidBlock"));out.put("deadline",elapsed>=TimedChecker.MAX_CHECKTIME-1000);}
        output.println(out.toJSONString());
    }
}
