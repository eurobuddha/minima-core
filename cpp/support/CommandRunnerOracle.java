import java.io.*;
import java.util.*;
import org.minima.system.commands.*;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class CommandRunnerOracle {
    static class Mock extends Command {Mock(String name){super(name,"fixture");}public ArrayList<String> getValidParams(){return new ArrayList<>(Arrays.asList("data","value","fail","state","text","path","amount"));}public JSONObject runCommand()throws Exception {if(getBooleanParam("fail",false))throw new CommandException("fixture failed");var result=getJSONReply();result.put("caller",getMiniDAPPID());return result;}public Command getFunction(){return new Mock(getName());}}
    public static void main(String[] args)throws Exception{Locale.setDefault(Locale.ENGLISH);CommandRunner.ALL_COMMANDS[0]=new Mock("fixture");CommandRunner.ALL_COMMANDS[1]=new Mock("send");CommandRunner.ALL_COMMANDS[2]=new Mock("txnstate");var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{var data=(JSONObject)new JSONParser().parse(line);String op=(String)data.get("op"),text=(String)data.get("text");var runner=CommandRunner.getRunner();Object result;
        if(op.equals("split")){var split=runner.splitterQuotedPattern(text);if(split==null)result=null;else{var a=new JSONArray();Collections.addAll(a,split);result=a;}}
        else if(op.equals("direct")||op.equals("meta")){
            var command=runner.getCommandOnly(text);result=null;if(command!=null){
                if(op.equals("direct")){command.getParams().putAll((JSONObject)data.get("params"));result=command.runCommand();}
                else{var meta=new JSONObject();meta.put("help",command.getHelp());meta.put("fullhelp",command.getFullHelp());meta.put("valid",command.getValidParams());meta.put("fresh",command.getFunction().getJSONReply());result=meta;}
            }
        }
        else if(op.equals("allowed"))result=runner.isCommandAllowed(text);
        else if(op.equals("run"))result=runner.runMultiCommand((String)data.get("caller"),text);
        else if(op.equals("single"))result=runner.runSingleCommand(text);
        else{var command=op.equals("only")?runner.getCommandOnly(text):runner.getCommand(text);if(command==null)result=null;else{var object=new JSONObject();object.put("name",command.getName());object.put("params",command.getParams());object.put("complete",command.getCompleteCommand());if(command.getName().equals("missingcmd"))object.put("reply",command.runCommand());result=object;}}
        System.out.println(JSONValue.toJSONString(result));
    }catch(Exception e){System.out.println("\"ERROR\"");}}
}
