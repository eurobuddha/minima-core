import java.io.*;
import java.nio.charset.StandardCharsets;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.system.params.GlobalParams;
import org.minima.utils.*;
import org.minima.utils.json.*;
public class TxHeaderOracle {
 static int integer(JSONObject q,String k,int fallback){return q.containsKey(k)?((Number)q.get(k)).intValue():fallback;}
 static String category(Exception e){return e.getClass().getName();}
 static Streamable field(TxHeader h,int i){return switch(i){case 0->h.mNonce;case 1->h.mChainID;case 2->h.mTimeMilli;case 3->h.mBlockNumber;case 4->h.mBlockDifficulty;case 5->h.mMMRRoot;case 6->h.mMMRTotal;case 7->h.mMagic;case 8->h.mCustomHash;default->h.mTxBodyHash;};}
 static void set(TxHeader h,int i,Streamable v){switch(i){case 0:h.mNonce=(MiniNumber)v;break;case 1:h.mChainID=(MiniData)v;break;case 2:h.mTimeMilli=(MiniNumber)v;break;case 3:h.mBlockNumber=(MiniNumber)v;break;case 4:h.mBlockDifficulty=(MiniData)v;break;case 5:h.mMMRRoot=(MiniData)v;break;case 6:h.mMMRTotal=(MiniNumber)v;break;case 7:h.mMagic=(Magic)v;break;case 8:h.mCustomHash=(MiniData)v;break;default:h.mTxBodyHash=(MiniData)v;}}
 static byte[] encode(Streamable p)throws IOException{var out=new ByteArrayOutputStream();p.writeDataStream(new DataOutputStream(out));return out.toByteArray();}
 static Object wire(Streamable p){if(p==null)return null;try{return new MiniData(encode(p)).to0xString();}catch(Exception e){return category(e);}}
 static JSONArray parents(MiniData[] p){var a=new JSONArray();if(p!=null)for(var value:p)a.add(value==null?null:value.to0xString());return a;}
 static JSONObject snapshot(TxHeader h){var r=new JSONObject();var f=new JSONArray();var aliases=new JSONArray();for(int i=0;i<10;i++)f.add(wire(field(h,i)));r.put("fields",f);r.put("parents",h.mSuperParents==null?null:parents(h.mSuperParents));if(h.mSuperParents!=null)for(var a:h.mSuperParents)for(var b:h.mSuperParents)aliases.add(a==b);r.put("parentAliases",aliases);return r;}
 static class FaultNumber extends MiniNumber{Runnable action;public void writeDataStream(DataOutputStream out)throws IOException{out.writeByte(0xAB);if(action!=null)action.run();throw new IOException("fixture");}}
 static class FaultData extends MiniData{Runnable action;public void writeDataStream(DataOutputStream out)throws IOException{out.writeByte(0xAB);if(action!=null)action.run();throw new IOException("fixture");}}
 static class FaultMagic extends Magic{Runnable action;public void writeDataStream(DataOutputStream out)throws IOException{out.writeByte(0xAB);if(action!=null)action.run();throw new IOException("fixture");}}
 static class HashFault extends MiniData{Runnable action;public void writeHashToStream(DataOutputStream out)throws IOException{out.writeByte(0xBC);if(action!=null)action.run();throw new IOException("fixture");}}
 static JSONObject run(JSONObject q)throws Exception{
  int levels=GlobalParams.MINIMA_CASCADE_LEVELS;var main=TxHeader.MAIN_NET;var test=TxHeader.TEST_NET;byte[] mainBytes=encode(main),zero=encode(MiniNumber.ZERO),empty=encode(MiniData.ZERO_TXPOWID),max=encode(Crypto.MAX_HASH);
  try{return execute(q);}finally{GlobalParams.MINIMA_CASCADE_LEVELS=levels;TxHeader.MAIN_NET=main;TxHeader.TEST_NET=test;main.readDataStream(new DataInputStream(new ByteArrayInputStream(mainBytes)));MiniNumber.ZERO.readDataStream(new DataInputStream(new ByteArrayInputStream(zero)));MiniData.ZERO_TXPOWID.readDataStream(new DataInputStream(new ByteArrayInputStream(empty)));Crypto.MAX_HASH.readDataStream(new DataInputStream(new ByteArrayInputStream(max)));}
 }
 static JSONObject execute(JSONObject q)throws Exception{
  GlobalParams.MINIMA_CASCADE_LEVELS=integer(q,"levels",4);var r=new JSONObject();String op=q.getString("op");
  if(op.equals("convert")){
   var original=System.out;var captured=new ByteArrayOutputStream();System.setOut(new PrintStream(captured,true,"UTF-8"));try{var converted=TxHeader.convertMiniDataVersion(q.getBoolean("nullInput")?null:new MiniData(q.getString("wire")));r.put("converted",converted!=null);if(converted!=null)r.put("wire",new MiniData(encode(converted)).to0xString());}catch(Exception e){r.put("error",category(e));}finally{System.setOut(original);}var logs=new JSONArray();for(String line:captured.toString("UTF-8").split("\n")){int pos=line.indexOf("] : java.");if(pos>=0){String type=line.substring(pos+4);int end=type.indexOf(": ");logs.add(end<0?type:type.substring(0,end));}}r.put("logs",logs);return r;
  }
  if(op.equals("construct")){try{var h=new TxHeader();r.put("length",h.mSuperParents.length);}catch(Exception e){r.put("error",category(e));}return r;}
  if(op.equals("defaults")){
   var a=new TxHeader();var b=new TxHeader();var chain=a.mChainID;TxHeader.MAIN_NET=new MiniData("0x77");var c=new TxHeader();var checks=new JSONArray();for(boolean v:new boolean[]{a.mChainID==b.mChainID,a.mChainID!=c.mChainID,c.mChainID==TxHeader.MAIN_NET,a.mBlockDifficulty==Crypto.MAX_HASH,a.mMMRTotal==MiniNumber.ZERO,a.mCustomHash==MiniData.ZERO_TXPOWID,a.mNonce!=b.mNonce,a.mNonce!=a.mBlockNumber,a.mMMRRoot!=a.mTxBodyHash,a.mMagic!=b.mMagic,a.mSuperParents!=b.mSuperParents,a.mSuperParents[0]!=a.mSuperParents[1]})checks.add(v);r.put("checks",checks);
   chain.readDataStream(new DataInputStream(new ByteArrayInputStream(encode(new MiniData("0x66")))));a.mBlockDifficulty.readDataStream(new DataInputStream(new ByteArrayInputStream(encode(new MiniData("0x55")))));a.mMMRTotal.readDataStream(new DataInputStream(new ByteArrayInputStream(encode(new MiniNumber(44)))));a.mCustomHash.readDataStream(new DataInputStream(new ByteArrayInputStream(encode(new MiniData("0x33")))));var values=new JSONArray();values.add(b.mChainID.to0xString());values.add(c.mChainID.to0xString());values.add(Crypto.MAX_HASH.to0xString());values.add(MiniNumber.ZERO.toString());values.add(MiniData.ZERO_TXPOWID.to0xString());r.put("values",values);return r;
  }
  var h=new TxHeader();h.readDataStream(new DataInputStream(new ByteArrayInputStream(new MiniData(q.getString("initial")).getBytes())));
  if(q.containsKey("arrayLength")){int n=integer(q,"arrayLength",4);if(n<0)h.mSuperParents=null;else{h.mSuperParents=new MiniData[n];for(int i=0;i<n;i++)h.mSuperParents[i]=new MiniData("0x33");}}
  if(q.containsKey("nullParent"))h.mSuperParents[integer(q,"nullParent",0)]=null;
  if(q.containsKey("nullField"))set(h,integer(q,"nullField",0),null);
  GlobalParams.MINIMA_CASCADE_LEVELS=integer(q,"runLevels",4);
  if(op.equals("read")){
   Streamable[] old=new Streamable[10];for(int i=0;i<10;i++)old[i]=field(h,i);var oldParents=h.mSuperParents;var alias=h;byte[] raw=new MiniData(q.getString("wire")).getBytes();var input=new ByteArrayInputStream(raw);input.skip(integer(q,"offset",0));try{h.readDataStream(new DataInputStream(input));}catch(Exception e){r.put("error",category(e));}
   r.put("offset",raw.length-input.available());r.put("state",snapshot(h));r.put("alias",snapshot(alias));var same=new JSONArray();for(int i=0;i<10;i++)same.add(old[i]==field(h,i));r.put("sameFields",same);r.put("sameArray",oldParents==h.mSuperParents);r.put("oldParents",oldParents==null?null:parents(oldParents));return r;
  }
  if(op.equals("alias")){var alias=h;var old=h.getBodyHash();var oldArray=h.mSuperParents;h.mTxBodyHash=new MiniData("0x99");oldArray[0]=oldArray[1];h.mSuperParents=new MiniData[0];var checks=new JSONArray();for(boolean v:new boolean[]{alias.getBodyHash()==h.getBodyHash(),old!=h.getBodyHash(),oldArray!=alias.mSuperParents,oldArray[0]==oldArray[1]})checks.add(v);r.put("checks",checks);r.put("oldBody",old.to0xString());return r;}
  if(op.equals("write")){
   int index=integer(q,"fault",-1);if(index>=0){Runnable action=()->{if(q.getBoolean("replace")){if(index>=10)h.mSuperParents[index-10]=new MiniData("0x99");else if(index==7)set(h,index,new Magic());else if(index==0||index==2||index==3||index==6)set(h,index,new MiniNumber(99));else set(h,index,new MiniData("0x99"));}};Streamable fault;
    if(index==0||index==2||index==3||index==6){var f=new FaultNumber();f.action=action;fault=f;}else if(index==7){var f=new FaultMagic();f.action=action;fault=f;}else if(index==1||index==4){var f=new FaultData();f.action=action;fault=f;}else{var f=new HashFault();f.action=action;fault=f;}if(index>=10)h.mSuperParents[index-10]=(MiniData)fault;else set(h,index,fault);
   }
   var output=new ByteArrayOutputStream();output.write(0xAA);output.write(0x55);try{h.writeDataStream(new DataOutputStream(output));}catch(Exception e){r.put("error",category(e));}r.put("wire",new MiniData(output.toByteArray()).to0xString());if(index>=0&&q.getBoolean("replace"))r.put("replaced",index>=10?h.mSuperParents[index-10].to0xString():wire(field(h,index)));return r;
  }
  if(op.equals("json")){try{r.put("json",h.toJSON());}catch(Exception e){r.put("error",category(e));}return r;}
  throw new IllegalArgumentException("fixture operation");
 }
 public static void main(String[] args)throws Exception{var in=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;while((line=in.readLine())!=null)System.out.println(run((JSONObject)new org.minima.utils.json.parser.JSONParser().parse(line)));}
}
