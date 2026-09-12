import java.io.*;
import org.minima.objects.base.*;
import org.minima.utils.json.*;

// Only the subclasses are fixtures. Every super call enters pinned Java code.
public class BaseDispatchOracle {
    static int integer(JSONObject q,String key){return ((Number)q.get(key)).intValue();}
    static final class ByteValue extends MiniByte {
        final JSONObject q;final String name;final JSONArray calls;final int mask;
        ByteValue(JSONObject q,String name,JSONArray calls){super(integer(q,name+"Raw"));this.q=q;this.name=name;this.calls=calls;mask=integer(q,name+"Mask");}
        boolean intercept(int bit,String method){if((mask&bit)==0)return false;String event=name+"."+method;calls.add(event);if(event.equals(q.getString("fault")))throw new IllegalStateException(event);return true;}
        public int getValue(){return intercept(1,"getValue")?integer(q,name+"Value"):super.getValue();}
        public byte getByteValue(){return intercept(2,"getByteValue")?(byte)integer(q,name+"Value"):super.getByteValue();}
        public boolean isEqual(MiniByte other){return intercept(4,"isEqual")?q.getBoolean("answer"):super.isEqual(other);}
        public boolean isFalse(){return intercept(8,"isFalse")?q.getBoolean("answer"):super.isFalse();}
        public boolean isTrue(){return intercept(16,"isTrue")?q.getBoolean("answer"):super.isTrue();}
        public String toString(){return intercept(32,"toString")?q.getString("text"):super.toString();}
        boolean baseEqual(MiniByte other){return super.isEqual(other);}
        boolean baseFalse(){return super.isFalse();}
        boolean baseTrue(){return super.isTrue();}
        String baseString(){return super.toString();}
    }
    static final class StringValue extends MiniString {
        final JSONObject q;final JSONArray calls;final int mask;
        StringValue(JSONObject q,JSONArray calls){super(q.getString("raw"));this.q=q;this.calls=calls;mask=integer(q,"mask");}
        boolean intercept(int bit,String method){if((mask&bit)==0)return false;calls.add(method);if(method.equals(q.getString("fault")))throw new IllegalStateException(method);return true;}
        public String toString(){return intercept(1,"toString")?q.getString("text"):super.toString();}
        public byte[] getData(){return intercept(2,"getData")?new MiniData(q.getString("bytes")).getBytes():super.getData();}
        public boolean isEqual(String value){return intercept(4,"isEqual")?q.getBoolean("answer"):super.isEqual(value);}
        boolean baseEqual(String value){return super.isEqual(value);}
    }
    static JSONObject run(JSONObject q)throws Exception {
        var r=new JSONObject();var calls=new JSONArray();String action=q.getString("action");
        if(q.getString("op").equals("dispatchByte")){
            var left=new ByteValue(q,"left",calls);MiniByte base=left;MiniByte right=new ByteValue(q,"right",calls);
            try{Object result=switch(action){
                case "getValue"->base.getValue();case "getByteValue"->(int)base.getByteValue();
                case "isEqual"->base.isEqual(right);case "isFalse"->base.isFalse();case "isTrue"->base.isTrue();case "toString"->base.toString();
                case "baseEqual"->left.baseEqual(right);case "baseFalse"->left.baseFalse();case "baseTrue"->left.baseTrue();case "baseString"->left.baseString();
                default->throw new IllegalArgumentException(action);
            };r.put("value",result);}catch(Exception error){r.put("error",error.toString());}
            var bytes=new ByteArrayOutputStream();base.writeDataStream(new DataOutputStream(bytes));r.put("wire",new MiniData(bytes.toByteArray()).to0xString());
        }else{
            var value=new StringValue(q,calls);MiniString base=value;var bytes=new ByteArrayOutputStream();bytes.write(0xAA);
            try{switch(action){
                case "toString":r.put("value",base.toString());break;
                case "getData":r.put("value",new MiniData(base.getData()).to0xString());break;
                case "isEqual":r.put("value",base.isEqual(q.getString("text")));break;
                case "baseEqual":r.put("value",value.baseEqual(q.getString("text")));break;
                case "copy":{var copy=new MiniString(base);r.put("value",copy.toString());r.put("data",new MiniData(copy.getData()).to0xString());break;}
                case "write":base.writeDataStream(new DataOutputStream(bytes));break;
                default:throw new IllegalArgumentException(action);
            }}catch(Exception error){r.put("error",error.toString());}r.put("wire",new MiniData(bytes.toByteArray()).to0xString());
        }r.put("calls",calls);return r;
    }
}
