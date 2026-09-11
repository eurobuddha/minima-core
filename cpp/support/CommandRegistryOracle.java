import org.minima.system.commands.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.*;
import java.util.concurrent.*;
public class CommandRegistryOracle {
    public static void main(String[] args)throws Exception {
        GeneralParams.RPC_PORT=19005;
        var barrier=new CyclicBarrier(8);Command[][] arrays=new Command[8][];Thread[] threads=new Thread[8];
        for(int i=0;i<8;i++){final int n=i;threads[i]=new Thread(()->{try{barrier.await();arrays[n]=CommandRunner.ALL_COMMANDS;}catch(Exception e){throw new RuntimeException(e);}});threads[i].start();}
        for(var thread:threads)thread.join();
        var out=new JSONObject();boolean same=true;for(var array:arrays)same&=array==CommandRunner.ALL_COMMANDS;out.put("sameRegistry",same);
        var rows=new JSONArray();
        for(Command prototype:CommandRunner.ALL_COMMANDS) {
            var row=new JSONObject();row.put("name",prototype.getName());row.put("help",prototype.getHelp());row.put("fullhelp",prototype.getFullHelp());
            var params=new JSONArray();params.addAll(prototype.getValidParams());row.put("params",params);
            Command a=CommandRunner.getRunner().getCommandOnly(prototype.getName()),b=CommandRunner.getRunner().getCommandOnly(prototype.getName());
            row.put("fresh",a!=b&&a!=prototype&&b!=prototype);row.put("freshparams",a.getParams());rows.add(row);
        }
        out.put("commands",rows);
        var original=CommandRunner.ALL_COMMANDS[0];CommandRunner.ALL_COMMANDS[0]=new org.minima.system.commands.base.hash();
        out.put("mutationVisible",CommandRunner.getRunner().getCommandOnly("quit")==null);
        CommandRunner.ALL_COMMANDS[0]=original;out.put("restored",CommandRunner.getRunner().getCommandOnly("quit")!=null);
        var errors=new JSONArray();
        for(String input:new String[]{"jnitest maxattempts:0","benchmark hashes:0","jniminingtest maxattempts:0"}) {
            try{CommandRunner.getRunner().runMultiCommand(input);errors.add("unexpected success");}catch(UnsatisfiedLinkError e){errors.add(e.toString());}
        }
        out.put("jniMissingLibrary",errors);System.out.println(out);
    }
}
