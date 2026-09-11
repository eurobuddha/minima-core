import java.io.*;
import java.math.*;
import org.minima.objects.base.*;
import org.minima.utils.json.*;
public class BaseValueAPIOracle {
    static JSONObject decimal(BigDecimal value){var r=new JSONObject();r.put("coefficient",value.unscaledValue().toString());r.put("scale",value.scale());r.put("text",value.toString());return r;}
    static JSONObject run(JSONObject q){var r=new JSONObject();try{
        if(q.getString("op").equals("data")){
            var value=new MiniData(q.getString("value"));var other=new MiniData(q.getString("other"));BigDecimal before=null;
            if(q.getBoolean("warm"))before=value.getDataValueDecimal();
            if(q.get("replacement")!=null){var bytes=new ByteArrayOutputStream();new MiniData(q.getString("replacement")).writeDataStream(new DataOutputStream(bytes));value.readDataStream(new DataInputStream(new ByteArrayInputStream(bytes.toByteArray())));}
            r.put("before",before==null?null:decimal(before));r.put("decimal",decimal(value.getDataValueDecimal()));r.put("bytes",value.to0xString());
            r.put("compare",value.compare(other));r.put("lessEqual",value.isLessEqual(other));r.put("moreEqual",value.isMoreEqual(other));r.put("equalBytes",value.isEqual(other));
            r.put("limited",value.to0xString(((Number)q.get("length")).intValue()));
        }else{
            var value=q.getString("op").equals("bigint")?new MiniNumber(new BigInteger(q.getString("value"))):new MiniNumber(q.getString("value"));var before=value.getAsBigDecimal();
            if(q.get("wire")!=null)value.readDataStream(new DataInputStream(new ByteArrayInputStream(new MiniData(q.getString("wire")).getBytes())));
            r.put("before",decimal(before));r.put("decimal",decimal(value.getAsBigDecimal()));var bytes=new ByteArrayOutputStream();value.writeDataStream(new DataOutputStream(bytes));r.put("wire",new MiniData(bytes.toByteArray()).to0xString());
            r.put("compareToZero",value.compareTo(MiniNumber.ZERO));r.put("decimalCompareToZero",value.getAsBigDecimal().compareTo(BigDecimal.ZERO));
        }
    }catch(Exception error){r.put("error",error.toString());}return r;}
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)System.out.println(run((JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line)));}
}
