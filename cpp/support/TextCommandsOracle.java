import java.io.*;
import org.minima.system.commands.*;
import org.minima.utils.json.*;
public class TextCommandsOracle {
    public static void main(String[] args)throws Exception{
        var reader=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=reader.readLine())!=null){var q=(JSONObject)JSONValue.parseWithException(line);var out=new JSONObject();
            Command cmd=switch(q.getString("kind")){case "tutorial"->new tutorial();case "base/tutorial"->new org.minima.system.commands.base.tutorial();case "whitepaper"->new whitepaper();default->throw new IllegalArgumentException();};
            cmd.getParams().putAll((JSONObject)q.get("params"));String op=q.getString("op");
            if(op.equals("meta")){out.put("name",cmd.getName());out.put("help",cmd.getHelp());out.put("fullhelp",cmd.getFullHelp());out.put("params",cmd.getValidParams());out.put("fresh",cmd.getFunction().getJSONReply());}
            else if(op.equals("runner"))out.put("value",CommandRunner.getRunner().runMultiCommand(q.getString("caller"),q.getString("text")));
            else if(op.equals("constant"))out.put("value",whitepaper.WP);
            else{var reply=cmd.runCommand();if(op.equals("alias"))cmd.getParams().put("later","changed");out.put("value",reply);}
            System.out.println(out.toJSONString());
        }
    }
}
