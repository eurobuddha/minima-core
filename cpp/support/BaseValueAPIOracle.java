import java.io.*;
import java.math.*;
import org.minima.objects.base.*;
import org.minima.utils.json.*;
public class BaseValueAPIOracle {
    static JSONObject decimal(BigDecimal value){var r=new JSONObject();r.put("coefficient",value.unscaledValue().toString());r.put("scale",value.scale());r.put("text",value.toString());return r;}
    static JSONObject helpers(JSONObject q)throws Exception {
        var r=new JSONObject();String op=q.getString("op");
        if(op.equals("helperWrite")){
            var bytes=new ByteArrayOutputStream();bytes.write(new MiniData(q.getString("prefix")).getBytes());var out=new DataOutputStream(bytes);
            switch(q.getString("kind")){
                case "byte":MiniByte.WriteToStream(out,q.getBoolean("value"));break;
                case "number":MiniNumber.WriteToStream(out,((Number)q.get("value")).intValue());break;
                case "string":MiniString.WriteToStream(out,q.getString("value"));break;
                case "data":MiniData.WriteToStream(out,new MiniData(q.getString("value")).getBytes());break;
            }r.put("wire",new MiniData(bytes.toByteArray()).to0xString());
        }else if(op.equals("helperRead")){
            byte[] data=new MiniData(q.getString("wire")).getBytes();var bytes=new ByteArrayInputStream(data);bytes.skip(((Number)q.get("offset")).intValue());var in=new DataInputStream(bytes);
            try{String value=switch(q.getString("kind")){
                case "byte"->MiniByte.ReadFromStream(in).toString();
                case "number"->MiniNumber.ReadFromStream(in).toString();
                case "string"->MiniString.ReadFromStream(in).toString();
                case "hash"->MiniData.ReadHashFromStream(in).to0xString();
                case "mmr"->org.minima.objects.mmr.MMREntryNumber.ReadFromStream(in).toString();
                default->q.containsKey("expected")?MiniData.ReadFromStream(in,((Number)q.get("expected")).intValue()).to0xString():MiniData.ReadFromStream(in).to0xString();
            };r.put("value",value);}catch(Exception e){r.put("error",e.toString());}r.put("offset",data.length-bytes.available());
        }else if(op.equals("helperVersion")){
            final int[] calls={0};String fault=q.getString("fault");var captured=new ByteArrayOutputStream();var previous=System.out;
            org.minima.utils.Streamable object=new org.minima.utils.Streamable(){
                public void readDataStream(DataInputStream in){}
                public void writeDataStream(DataOutputStream out)throws IOException{calls[0]++;out.write(new byte[]{(byte)0xAA,0x55});if(fault.equals("io"))throw new IOException("fixture IO");if(fault.equals("runtime"))throw new IllegalStateException("fixture runtime");out.write(new MiniData(q.getString("bytes")).getBytes());}
            };
            try{System.setOut(new PrintStream(captured));var value=MiniData.getMiniDataVersion(fault.equals("null")?null:object);r.put("value",value==null?null:value.to0xString());}
            catch(Exception e){r.put("errorClass",e.getClass().getName());}finally{System.setOut(previous);}
            r.put("calls",calls[0]);r.put("logged",captured.toString().contains("java.io.IOException: fixture IO"));
        }else if(op.equals("helperByteConstants")){
            byte old=MiniByte.FALSE.getByteValue();try{MiniByte.FALSE.readDataStream(new DataInputStream(new ByteArrayInputStream(new byte[]{((Number)q.get("falseValue")).byteValue()})));var value=new MiniByte(((Number)q.get("value")).intValue());r.put("false",value.isFalse());r.put("true",value.isTrue());r.put("falseValue",MiniByte.FALSE.getValue());}
            finally{MiniByte.FALSE.readDataStream(new DataInputStream(new ByteArrayInputStream(new byte[]{old})));}
        }else if(op.equals("helperMMRDecimal")){
            var value=new org.minima.objects.mmr.MMREntryNumber(new BigDecimal(new BigInteger(q.getString("coefficient")),((Number)q.get("scale")).intValue()));var before=value.getBigDecimal();
            value.readDataStream(new DataInputStream(new ByteArrayInputStream(new MiniData(q.getString("wire")).getBytes())));r.put("before",decimal(before));r.put("after",decimal(value.getBigDecimal()));
        }
        return r;
    }
    static JSONObject run(JSONObject q){var r=new JSONObject();try{
        if(q.getString("op").startsWith("dispatch"))return BaseDispatchOracle.run(q);
        if(q.getString("op").startsWith("helper"))return helpers(q);
        if(q.getString("op").equals("limits")){
            r.put("maximum",decimal(MiniNumber.MAX_MININUMBER));r.put("minimum",decimal(MiniNumber.MIN_MININUMBER));r.put("maximumNumber",decimal(MiniNumber.MAXIMUM.getAsBigDecimal()));
        }else if(q.getString("op").equals("data")){
            var value=new MiniData(q.getString("value"));var other=new MiniData(q.getString("other"));BigDecimal before=null;
            if(q.getBoolean("warm"))before=value.getDataValueDecimal();
            if(q.get("replacement")!=null){var bytes=new ByteArrayOutputStream();new MiniData(q.getString("replacement")).writeDataStream(new DataOutputStream(bytes));value.readDataStream(new DataInputStream(new ByteArrayInputStream(bytes.toByteArray())));}
            r.put("before",before==null?null:decimal(before));r.put("decimal",decimal(value.getDataValueDecimal()));r.put("bytes",value.to0xString());
            r.put("compare",value.compare(other));r.put("lessEqual",value.isLessEqual(other));r.put("moreEqual",value.isMoreEqual(other));r.put("equalBytes",value.isEqual(other));
            r.put("limited",value.to0xString(((Number)q.get("length")).intValue()));
        }else{
            var value=q.getString("op").equals("decimal")?new MiniNumber(new BigDecimal(new BigInteger(q.getString("value")),((Number)q.get("scale")).intValue())):q.getString("op").equals("bigint")?new MiniNumber(new BigInteger(q.getString("value"))):new MiniNumber(q.getString("value"));var before=value.getAsBigDecimal();
            if(q.get("wire")!=null)value.readDataStream(new DataInputStream(new ByteArrayInputStream(new MiniData(q.getString("wire")).getBytes())));
            r.put("before",decimal(before));r.put("decimal",decimal(value.getAsBigDecimal()));var bytes=new ByteArrayOutputStream();value.writeDataStream(new DataOutputStream(bytes));r.put("wire",new MiniData(bytes.toByteArray()).to0xString());
            r.put("compareToZero",value.compareTo(MiniNumber.ZERO));r.put("decimalCompareToZero",value.getAsBigDecimal().compareTo(BigDecimal.ZERO));
        }
    }catch(Exception error){r.put("error",error.toString());}return r;}
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)System.out.println(run((JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line)));}
}
