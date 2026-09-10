import java.io.*;
import java.math.BigInteger;
import org.minima.utils.Maths;
import org.minima.utils.Stack;
import org.minima.utils.TimedRequest;
import org.minima.objects.base.MiniData;
import org.minima.system.params.GlobalParams;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class SmallUtilitiesOracle {
    static Object run(JSONObject request) {
        String op=(String)request.get("op");
        if(op.equals("timed")) {
            TimedRequest timed=new TimedRequest();JSONArray out=new JSONArray();boolean clock=true;
            for(Object item:(JSONArray)request.get("ops")) {
                JSONObject action=(JSONObject)item;Object result=null;
                try {
                    String name=(String)action.get("op");
                    MiniData id=action.get("id")==null?null:new MiniData((String)action.get("id"));
                    if(name.equals("add")) {
                        long before=System.currentTimeMillis();var unit=timed.new RequestUnit(id);long after=System.currentTimeMillis();clock &= before<=unit.mTimeMilli&&unit.mTimeMilli<=after;
                        if(action.containsKey("time"))unit.mTimeMilli=Long.parseLong((String)action.get("time"));
                        timed.mPreviousRequests.add(unit);
                    } else if(name.equals("send"))result=timed.sendNow(id);
                    else if(name.equals("nullunit"))timed.mPreviousRequests.add(null);
                    else if(name.equals("nulllist"))timed.mPreviousRequests=null;
                    else if(name.equals("reset"))timed.mPreviousRequests=new java.util.ArrayList<>();
                    else if(name.equals("clear"))timed.mPreviousRequests.clear();
                    else if(name.equals("change"))timed.mPreviousRequests.get(((Number)action.get("index")).intValue()).mDataID=id;
                    else throw new IllegalArgumentException();
                }catch(Exception error){result="ERROR";}
                JSONObject state=new JSONObject();state.put("result",result);state.put("size",timed.mPreviousRequests==null?-1:timed.mPreviousRequests.size());out.add(state);
            }
            JSONObject result=new JSONObject();result.put("clock",clock);result.put("results",out);return result;
        }
        if(op.equals("versions"))return Maths.compareVersions((String)request.get("a"),(String)request.get("b"));
        if(op.equals("super")){GlobalParams.MINIMA_CASCADE_LEVELS=((Number)request.get("levels")).intValue();return Maths.getSuperLevel(new MiniData((String)request.get("a")),new MiniData((String)request.get("b")));}
        if(op.equals("logbig"))return Long.toUnsignedString(Double.doubleToLongBits(Maths.log2BI(new BigInteger((String)request.get("a")))));
        if(op.equals("log"))return Long.toUnsignedString(Double.doubleToLongBits(Maths.log2(Double.longBitsToDouble(Long.parseUnsignedLong((String)request.get("a"))))));
        Stack stack=new Stack();JSONArray values=(JSONArray)request.get("values"),out=new JSONArray();
        for(Object item:(JSONArray)request.get("ops")) {
            JSONObject action=(JSONObject)item;Object result=null;
            try {
                switch((String)action.get("op")) {
                    case "push":stack.push(values.get(((Number)action.get("index")).intValue()));break;
                    case "pop":result=stack.pop();break;
                    case "peek":result=stack.peek();break;
                    case "mutate":((JSONObject)values.get(((Number)action.get("index")).intValue())).put("text",action.get("text"));break;
                    default:throw new IllegalArgumentException();
                }
            }catch(Exception error){result="ERROR";}
            JSONObject state=new JSONObject();state.put("result",result);state.put("empty",stack.isEmpty());out.add(state);
        }
        return out;
    }
    public static void main(String[] args)throws Exception {
        BufferedReader input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}
        catch(NumberFormatException error){System.out.println(JSONValue.toJSONString("NUMBER_ERROR:"+error.getMessage()));}
        catch(Exception error){System.out.println("\"ERROR\"");}
    }
}
