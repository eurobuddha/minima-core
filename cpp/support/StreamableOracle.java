import java.io.*;
import org.minima.utils.*;
import org.minima.utils.json.*;
import org.minima.objects.base.*;
import org.minima.objects.mmr.*;
import org.minima.objects.keys.*;
import org.minima.utils.sphincs.SPHINCSSignature;
import org.minima.utils.sphincs.HORST.HORSTSignature;
import org.minima.utils.sphincs.FORS.FORSSignature;
import org.minima.system.network.minima.NIOManager;
public class StreamableOracle {
    static class Failing implements Streamable {
        byte[] values={1,2,3};int fail;
        Failing(int n){fail=n;}
        public void writeDataStream(DataOutputStream out)throws IOException{for(int i=0;i<values.length;i++){if(i==fail)throw new IOException("fixture write");out.writeByte(values[i]);}if(fail==3)throw new IOException("fixture write");}
        public void readDataStream(DataInputStream in)throws IOException{for(int i=0;i<values.length;i++)values[i]=in.readByte();}
    }
    static Streamable bad(String kind){var d=new MMRData(new MiniData(new byte[65]),new MiniNumber(7));var p=new MMRProof(new MiniNumber(7));p.addProofChunk(true,d);var s=new SignatureProof(new MiniData("0xA1"),new MiniData("0xB2"),p);if(kind.equals("mmrdata"))return d;if(kind.equals("entry"))return new MMREntry(7,new MMREntryNumber(4),d);if(kind.equals("chunk"))return p.new MMRProofChunk(true,d);if(kind.equals("signatureproof"))return s;var result=new Signature();result.addSignatureProof(s);return result;}
    static Streamable create(JSONObject q){if(q.getString("op").equals("badwrite"))return bad(q.getString("kind"));return switch(q.getString("kind")){case "data"->new MiniData();case "number"->new MiniNumber();case "string"->new MiniString("");case "byte"->new MiniByte();case "proof"->new MMRProof();case "mmrdata"->new MMRData(new MiniData("0x"),MiniNumber.ZERO);case "entrynumber"->new MMREntryNumber();case "entry"->Boolean.TRUE.equals(q.get("nullData"))?new MMREntry(7,new MMREntryNumber(4),(MMRData)null):new MMREntry(7,new MMREntryNumber(4));case "chunk"->new MMRProof().new MMRProofChunk(false,new MMRData(new MiniData("0x"),MiniNumber.ZERO));case "signatureproof"->new SignatureProof(new MiniData("0x"),new MiniData("0x"),new MMRProof());case "signature"->new Signature();case "horst"->new HORSTSignature();case "fors"->new FORSSignature();case "sphincs"->new SPHINCSSignature();case "failing"->new Failing(((Number)q.get("fail")).intValue());default->throw new IllegalArgumentException();};}
    static byte[] data(JSONObject q,String name){return new MiniData(q.getString(name)).getBytes();}
    static String hex(byte[] bytes){return new MiniData(bytes).to0xString();}
    static JSONObject state(Streamable value){var out=new JSONObject();var buffer=new ByteArrayOutputStream();buffer.write(0xaa);buffer.write(0x55);try{value.writeDataStream(new DataOutputStream(buffer));}catch(Exception e){out.put("writeError",e.getClass().getName());}out.put("appended",hex(buffer.toByteArray()));return out;}
    static JSONObject run(JSONObject q)throws Exception{
        var result=new JSONObject();Streamable value=create(q);var initial=data(q,"initial");if(!q.getString("op").equals("badwrite")&&!Boolean.TRUE.equals(q.get("empty")))value.readDataStream(new DataInputStream(new ByteArrayInputStream(initial)));if(value instanceof MMRData d)d.setUnspendable(true);Signature alias=value instanceof Signature?(Signature)value:null;var oldList=alias==null?null:alias.getAllSignatureProofs();String op=q.getString("op");if(Boolean.TRUE.equals(q.get("cache")))((MiniData)value).getDataValue();
        if(op.equals("read")){byte[] bytes=q.containsKey("selfInput")?((MiniData)value).getBytes():data(q,"wire");var buffer=new ByteArrayInputStream(bytes);int start=((Number)q.get("offset")).intValue();buffer.skip(start);try{var in=new DataInputStream(buffer);if(q.containsKey("hashRead"))((MiniData)value).readHashFromStream(in);else if(q.containsKey("expectedSize"))((MiniData)value).readDataStream(in,((Number)q.get("expectedSize")).intValue());else value.readDataStream(in);}catch(Exception e){result.put("readError",e.getClass().getName());}result.put("offset",bytes.length-buffer.available());}
        else if(op.equals("callers")){
            result.put("data",MiniData.getMiniDataVersion(value).to0xString());result.put("hash",Crypto.getInstance().hashObject(value).to0xString());result.put("pairHash",Crypto.getInstance().hashObjects(value,value).to0xString());result.put("nio",NIOManager.createNIOMessage(new MiniByte(13),value).to0xString());
            File file=new File(q.getString("file"));try{MiniFile.writeObjectToFile(file,value);result.put("file",hex(MiniFile.readCompleteFile(file)));MiniFile.writeObjectToFile(file,value,true);result.put("appendedFile",hex(MiniFile.readCompleteFile(file)));MiniFile.writeDataToFile(file,data(q,"wire"));MiniFile.loadObject(file,value);}finally{file.delete();}
        }
        if(value instanceof MMREntry e)result.put("empty",e.isEmpty());if(value instanceof MMRData d)result.put("unspendable",d.isUnspendable());if(alias!=null){result.put("oldListSize",oldList.size());result.put("sameList",oldList==alias.getAllSignatureProofs());result.put("aliasCount",alias.getAllSignatureProofs().size());}
        if(value instanceof MiniData d){result.put("numeric",d.getDataValue().toString());result.put("less128",d.isLess(new MiniData("0x80")));result.put("shiftLeft",d.shiftl(1).to0xString());result.put("shiftRight",d.shiftr(1).to0xString());}
        result.put("state",state(value));return result;
    }
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null){JSONObject result;try{result=run((JSONObject)JSONValue.parseWithException(line));}catch(Exception e){result=new JSONObject();result.put("error",e.getClass().getName());result.put("message",e.getMessage());}System.out.println(result.toJSONString());}}
}
