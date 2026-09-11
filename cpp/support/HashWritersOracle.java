import java.io.*;
import java.nio.charset.StandardCharsets;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.utils.Streamable;
import org.minima.utils.json.*;
public class HashWritersOracle {
    static int integer(JSONObject q,String key,int fallback){return q.containsKey(key)?((Number)q.get(key)).intValue():fallback;}
    static class Ordinary extends MiniData {
        String mode;JSONArray trace;Runnable action;
        Ordinary(byte[] bytes,String m,JSONArray t){super(bytes);mode=m;trace=t;}
        public void writeDataStream(DataOutputStream out)throws IOException{
            trace.add("ordinary");out.writeByte(0xA1);if(action!=null)action.run();
            if(mode.startsWith("super"))super.writeDataStream(out);
            trace.add("ordinary-after");if(mode.endsWith("io"))throw new IOException("ordinary fixture");
        }
    }
    static class Hash extends Ordinary {
        Hash(byte[] bytes,String m,JSONArray t){super(bytes,m,t);}
        public void writeHashToStream(DataOutputStream out)throws IOException{
            trace.add("hash");out.writeByte(0xB2);if(action!=null)action.run();
            if(mode.startsWith("super"))super.writeHashToStream(out);
            trace.add("hash-after");if(mode.endsWith("io"))throw new IOException("hash fixture");
        }
    }
    static JSONObject run(JSONObject q)throws Exception{
        int oldHash=MiniData.MINIMA_MAX_HASH_LENGTH,oldData=MiniData.MINIMA_MAX_MINIDATA_LENGTH;
        try{
            MiniData.MINIMA_MAX_HASH_LENGTH=integer(q,"hashLimit",64);MiniData.MINIMA_MAX_MINIDATA_LENGTH=integer(q,"dataLimit",512*1024*1024);
            byte[] bytes=new byte[integer(q,"length",1)];java.util.Arrays.fill(bytes,(byte)0x44);var trace=new JSONArray();String kind=q.getString("kind"),mode=q.getString("mode"),context=q.getString("context"),method=q.getString("method");
            MiniData child=kind.equals("ordinary")?new Ordinary(bytes,mode,trace):kind.equals("hash")?new Hash(bytes,mode,trace):new MiniData(bytes);
            Streamable owner;java.util.function.Supplier<String> current=null;
            if(context.equals("transaction")){
                var txn=new Transaction();txn.setLinkHash(child);owner=txn;current=()->txn.getLinkHash().to0xString();
                if(child instanceof Ordinary derived&&q.getBoolean("replace"))derived.action=()->txn.setLinkHash(new MiniData("0x99"));
            }else if(context.equals("prng")||context.equals("id")){
                var body=new TxBody();body.mPRNG=new MiniData("0x11");body.mTxnDifficulty=new MiniData("0x22");owner=body;
                if(context.equals("prng"))body.mPRNG=child;else{body.mTxPowIDList.add(new MiniData("0x33"));body.mTxPowIDList.add(child);}
                current=()->(context.equals("prng")?body.mPRNG:body.mTxPowIDList.get(1)).to0xString();
                if(child instanceof Ordinary derived&&q.getBoolean("replace"))derived.action=()->{var replacement=new MiniData("0x99");if(context.equals("prng"))body.mPRNG=replacement;else body.mTxPowIDList.set(1,replacement);};
            }else if(context.equals("magic")){
                var magic=new Magic();magic.mCurrentMinTxPoWWork=child;owner=magic;current=()->magic.mCurrentMinTxPoWWork.to0xString();
                if(child instanceof Ordinary derived&&q.getBoolean("replace"))derived.action=()->magic.mCurrentMinTxPoWWork=new MiniData("0x99");
            }else owner=child;
            child=null;var result=new JSONObject();var output=new ByteArrayOutputStream();output.write(0xAA);output.write(0x55);var stream=new DataOutputStream(output);
            try{
                if(method.equals("read")||method.equals("readHash")){
                    var input=new ByteArrayInputStream(new MiniData(q.getString("wire")).getBytes());int length=input.available();var data=(MiniData)owner;
                    try{if(method.equals("readHash"))data.readHashFromStream(new DataInputStream(input));else data.readDataStream(new DataInputStream(input));}catch(Exception e){result.put("error",e.toString());}
                    result.put("offset",length-input.available());result.put("value",data.to0xString());
                }else if(!context.equals("direct"))owner.writeDataStream(stream);
                else if(method.equals("hash"))((MiniData)owner).writeHashToStream(stream);
                else if(method.equals("hashSerial")||method.equals("serial")){
                    var staging=new ByteArrayOutputStream();var target=new DataOutputStream(staging);
                    if(method.equals("hashSerial"))((MiniData)owner).writeHashToStream(target);else owner.writeDataStream(target);stream.write(staging.toByteArray());
                }else owner.writeDataStream(stream);
            }catch(Exception e){result.put("error",e.toString());}
            result.put("wire",new MiniData(output.toByteArray()).to0xString());result.put("trace",trace);if(current!=null)result.put("current",current.get());return result;
        }finally{MiniData.MINIMA_MAX_HASH_LENGTH=oldHash;MiniData.MINIMA_MAX_MINIDATA_LENGTH=oldData;}
    }
    public static void main(String[] args)throws Exception{var reader=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;while((line=reader.readLine())!=null)System.out.println(run((JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line)));}
}
