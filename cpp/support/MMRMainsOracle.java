import java.io.*;
import org.minima.objects.mmr.*;
import org.minima.objects.base.*;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.JSONObject;
public class MMRMainsOracle {
    public static void main(String[] args) throws Exception {
        String operation=args[0],mode=args[1];String[] input=mode.equals("null")?null:mode.equals("args")?new String[]{"ignored"}:new String[0];
        var output=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(output,true,"UTF-8"));
        String error="none",message="",before="",after="";MMR tree=new MMR();
        try {
            if(operation.equals("MMR"))MMR.main(input);
            else if(operation.equals("MegaMMR")){GeneralParams.MEGAMMR_MEGAPRUNE=Boolean.parseBoolean(args[2]);MegaMMR.main(input);}
            else {
                if(mode.equals("entry")){MMRData data=new MMRData(new MiniData("0x01"),new MiniNumber(args[5]));data.setUnspendable(Boolean.parseBoolean(args[6]));tree.setEntry(Integer.parseInt(args[2]),new MMREntryNumber(new java.math.BigDecimal(new java.math.BigInteger(args[3]),Integer.parseInt(args[4]))),data);}
                else if(mode.equals("many"))for(int i=0;i<Integer.parseInt(args[2]);++i)tree.addEntry(new MMRData(new MiniData("0x01"),new MiniNumber(i)));
                before=MiniData.getMiniDataVersion(tree).to0xString();
                if(operation.equals("printinfo"))MMR.printinfo(mode.equals("null")?null:tree);else MMR.printmmrtree(mode.equals("null")?null:tree);
                after=MiniData.getMiniDataVersion(tree).to0xString();
            }
        }catch(Throwable e){error=e.getClass().getName();message=e.getMessage()==null?"":e.getMessage();}
        finally{System.setOut(previous);}
        JSONObject result=new JSONObject();result.put("stdout",output.toString("UTF-8"));result.put("error",error);result.put("message",message);result.put("before",before);result.put("after",after);System.out.println(result);
    }
}
