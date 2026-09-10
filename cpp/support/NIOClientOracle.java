import java.io.*;
import java.net.*;
import java.nio.*;
import java.nio.channels.*;
import java.nio.channels.spi.*;
import java.util.*;
import java.lang.reflect.*;
import java.security.MessageDigest;
import sun.misc.Unsafe;
import org.minima.system.Main;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.*;
import org.minima.objects.base.MiniData;
import org.minima.utils.FastByteArrayStream;
import org.minima.utils.messages.Message;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class NIOClientOracle {
    static final Unsafe U;
    static {try{Field f=Unsafe.class.getDeclaredField("theUnsafe");f.setAccessible(true);U=(Unsafe)f.get(null);}catch(Exception e){throw new RuntimeException(e);}}
    static Field field(Class<?> type,String name)throws Exception{while(type!=null){try{Field f=type.getDeclaredField(name);f.setAccessible(true);return f;}catch(NoSuchFieldException e){type=type.getSuperclass();}}throw new NoSuchFieldException(name);}
    static Object get(Object value,String name)throws Exception{return field(value.getClass(),name).get(value);}
    static void set(Object value,String name,Object data)throws Exception{field(value.getClass(),name).set(value,data);}
    static byte[] bytes(Object value){return new MiniData((String)value).getBytes();}
    static String digest(byte[] value)throws Exception{return new MiniData(MessageDigest.getInstance("SHA-256").digest(value)).to0xString();}
    static Object data(byte[] value)throws Exception{if(value==null)return null;var out=new JSONObject();out.put("size",value.length);out.put("hash",digest(value));return out;}
    static Object buffer(ByteBuffer buffer)throws Exception{if(buffer==null)return null;var out=new JSONObject();out.put("position",buffer.position());out.put("limit",buffer.limit());out.put("hash",digest(buffer.array()));return out;}
    static class Channel extends SocketChannel {
        ArrayList<Byte> input=new ArrayList<>();ByteArrayOutputStream output=new ByteArrayOutputStream();int rcap=8192,wcap=8192,reads=0,writes=0,closes=0;boolean eof=false,readError=false,writeError=false;
        Channel(){super(SelectorProvider.provider());}
        void feed(byte[] data){for(byte b:data)input.add(b);}
        public int read(ByteBuffer target)throws IOException{reads++;if(!isOpen())throw new ClosedChannelException();if(readError)throw new IOException("Read failure");if(!target.hasRemaining())return 0;if(input.isEmpty()&&eof)return -1;int count=Math.min(Math.min(input.size(),target.remaining()),rcap);for(int i=0;i<count;i++)target.put(input.remove(0));return count;}
        public int write(ByteBuffer source)throws IOException{writes++;if(!isOpen())throw new ClosedChannelException();if(writeError)throw new IOException("Write failure");int count=Math.min(source.remaining(),wcap);for(int i=0;i<count;i++)output.write(source.get());return count;}
        protected void implCloseSelectableChannel(){closes++;}
        protected void implConfigureBlocking(boolean block){}
        public SocketChannel bind(SocketAddress address){throw new UnsupportedOperationException();}
        public <T> SocketChannel setOption(SocketOption<T> name,T value){throw new UnsupportedOperationException();}
        public <T> T getOption(SocketOption<T> name){throw new UnsupportedOperationException();}
        public Set<SocketOption<?>> supportedOptions(){return Set.of();}
        public SocketChannel shutdownInput(){throw new UnsupportedOperationException();}
        public SocketChannel shutdownOutput(){throw new UnsupportedOperationException();}
        public Socket socket(){throw new UnsupportedOperationException();}
        public boolean isConnected(){return isOpen();}
        public boolean isConnectionPending(){return false;}
        public boolean connect(SocketAddress remote){throw new UnsupportedOperationException();}
        public boolean finishConnect(){throw new UnsupportedOperationException();}
        public SocketAddress getRemoteAddress(){throw new UnsupportedOperationException();}
        public SocketAddress getLocalAddress(){throw new UnsupportedOperationException();}
        public long read(ByteBuffer[] dst,int offset,int length){throw new UnsupportedOperationException();}
        public long write(ByteBuffer[] src,int offset,int length){throw new UnsupportedOperationException();}
    }
    static class Select extends Selector {
        int wakes=0;boolean wakeError=false;
        public Selector wakeup(){if(wakeError)throw new IllegalStateException();wakes++;return this;}
        public boolean isOpen(){return true;}public SelectorProvider provider(){return SelectorProvider.provider();}public Set<SelectionKey> keys(){throw new UnsupportedOperationException();}public Set<SelectionKey> selectedKeys(){throw new UnsupportedOperationException();}public int selectNow(){throw new UnsupportedOperationException();}public int select(long timeout){throw new UnsupportedOperationException();}public int select(){throw new UnsupportedOperationException();}public void close(){}
    }
    static class Key extends SelectionKey {
        Channel channel;Select selector=new Select();boolean valid=true,cancelError=false,interestError=false;int interest=OP_READ,cancels=0;
        Key(Channel ch){channel=ch;}public SelectableChannel channel(){return channel;}public Selector selector(){return selector;}public boolean isValid(){return valid;}public void cancel(){cancels++;if(cancelError)throw new IllegalStateException();valid=false;}public int interestOps(){if(!valid)throw new CancelledKeyException();return interest;}public SelectionKey interestOps(int value){if(!valid)throw new CancelledKeyException();if(interestError)throw new IllegalStateException();interest=value;return this;}public int readyOps(){return 0;}
    }
    // Skip Main/NetworkManager/NIOManager startup only; NIOClient constructors
    // and every read/write operation below execute the unmodified pinned class.
    static class Manager extends NIOManager {
        NIOTraffic traffic;ArrayList<Message> posted;boolean postError=false,nullTraffic=false;
        Manager(){super(null);}
        public NIOTraffic getTrafficListener(){return nullTraffic?null:traffic;}
        public void PostMessage(Message message){if(postError)throw new IllegalStateException();posted.add(message);}
    }
    static Manager install()throws Exception{var manager=(Manager)U.allocateInstance(Manager.class);manager.traffic=new NIOTraffic();manager.posted=new ArrayList<>();var network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(network,"mNIOManager",manager);var main=(Main)U.allocateInstance(Main.class);set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);return manager;}
    static Object snapshot(NIOClient client,Channel channel,Key key,Manager manager,NIOClientInfo info)throws Exception{
        var out=new JSONObject();out.put("json",client.toJSON());out.put("full",client.getFullAddress());out.put("fullMinima",client.getFullMinimaAddress());out.put("outgoing",client.isOutgoing());out.put("last",client.getLastReadTime());out.put("attempts",client.getConnectAttempts());out.put("p2p",client.hasReceivedP2PGreeting());out.put("disconnected",client.hasMaximaDiscxonnected());try{out.put("maximaClient",client.isMaximaClient());}catch(Exception e){out.put("maximaClient","ERROR");}try{out.put("maximaMLS",client.isMaximaMLS());}catch(Exception e){out.put("maximaMLS","ERROR");}out.put("extra",client.getExtraData());
        out.put("in",buffer((ByteBuffer)get(client,"mBufferIn")));out.put("out",buffer((ByteBuffer)get(client,"mBufferOut")));
        for(String name:new String[]{"mReadCurrentPosition","mReadCurrentLimit","mWritePosition","mWriteLimit","mWriteStart"})out.put(name,get(client,name));
        var stream=(FastByteArrayStream)get(client,"mReadByteArray");out.put("stream",stream==null?null:data(stream.toByteArray()));out.put("temp",data((byte[])get(client,"mReadByteArrayTemp")));out.put("writeData",data((byte[])get(client,"mWriteData")));
        var messages=(ArrayList<?>)get(client,"mMessages");if(messages==null)out.put("queue",null);else{var queue=new JSONArray();for(Object m:messages)queue.add(m==null?null:data(((MiniData)m).getBytes()));out.put("queue",queue);}
        var posted=new JSONArray();for(Message m:manager.posted){var msg=new JSONObject();msg.put("type",m.getMessageType());msg.put("uid",m.getString("uid"));msg.put("host",m.getString("fullhost"));msg.put("data",data(((MiniData)m.getObject("data")).getBytes()));posted.add(msg);}out.put("posted",posted);out.put("totalRead",manager.traffic.getTotalRead());out.put("totalWrite",manager.traffic.getTotalWrite());
        out.put("validKey",key.valid);out.put("interest",key.interest);out.put("wakes",key.selector.wakes);out.put("cancels",key.cancels);out.put("channelOpen",channel.isOpen());out.put("reads",channel.reads);out.put("writes",channel.writes);out.put("closes",channel.closes);out.put("input",channel.input.size());out.put("output",data(channel.output.toByteArray()));
        out.put("info",info==null?null:info.toJSON());if(info!=null){try{out.put("infoTime",info.getTimeConnected());}catch(Exception e){out.put("infoTime","ERROR");}try{out.put("infoExtra",info.getExtraData());}catch(Exception e){out.put("infoExtra","ERROR");}out.put("infoValid",info.ismValidGreeting());}return out;
    }
    static Object run(JSONObject request)throws Exception{
        if(request.containsKey("date"))return new Date(((Number)request.get("date")).longValue()).toString();
        var manager=install();var channel=new Channel();var key=new Key(channel);long clock=1700000000000L;
        if(Boolean.FALSE.equals(request.get("runtime")))field(Main.class,"mMainInstance").set(null,null);
        if(Boolean.TRUE.equals(request.get("nullManager")))set(Main.getInstance().getNetworkManager(),"mNIOManager",null);
        boolean connecting="connecting".equals(request.get("ctor"));var client=connecting?new NIOClient((String)request.get("host"),((Number)request.get("port")).intValue()):new NIOClient(Boolean.TRUE.equals(request.get("incoming")),(String)request.get("host"),((Number)request.get("port")).intValue(),channel,key);
        boolean uid=client.getUID().matches("[0-9A-V]{1,13}");set(client,"mUID","fixture");if(!connecting){set(client,"mTimeConnected",clock);set(client,"mLastMessageRead",clock);}NIOClientInfo info=null;
        var states=new JSONArray();states.add(snapshot(client,channel,key,manager,info));
        for(Object raw:(JSONArray)request.get("ops")){var op=(JSONObject)raw;String name=(String)op.get("op"),field=(String)op.get("field");Object value=op.get("value"),result="OK";long started=System.currentTimeMillis();try{switch(name){
        case "largeSend":client.sendData(new MiniData(new byte[((Number)value).intValue()]));break;case "feed":channel.feed(bytes(value));break;case "read":client.handleRead();break;case "write":client.handleWrite();break;case "send":client.sendData(value==null?null:new MiniData((String)value));break;case "disconnect":client.disconnect();break;
        case "set":switch(field){
        case "host":client.overrideHost((String)value);break;case "port":client.setPort(((Number)value).intValue());break;case "minimaport":client.setMinimaPort(((Number)value).intValue());break;case "welcome":client.setWelcomeMessage((String)value);break;case "maxima":client.setMaximaIdent((String)value);break;case "mls":client.setMaximaMLS((String)value);break;case "valid":client.setValidGreeting((Boolean)value);break;case "sent":client.setSentGreeting((Boolean)value);break;case "attempts":client.setConnectAttempts(((Number)value).intValue());break;case "extra":client.setExtraData(value);break;default:throw new IllegalArgumentException(field);}break;
        case "increment":client.incrementConnectAttempts();break;case "p2p":client.setReceivedP2PGreeting();break;case "maximaDisconnected":client.setMaximaDisconnected();break;
        case "clock":clock=((Number)value).longValue();break;case "connectedTime":set(client,"mTimeConnected",((Number)value).longValue());break;
        case "info":info=new NIOClientInfo(client,(Boolean)value);break;case "explicitInfo":info=new NIOClientInfo("manual",null,-1,true);break;case "nullInfo":info=new NIOClientInfo(null,true);break;case "infoExtra":info.setExtrasData(value);break;
        case "control":switch(field){case "rcap":channel.rcap=((Number)value).intValue();break;case "wcap":channel.wcap=((Number)value).intValue();break;case "eof":channel.eof=(Boolean)value;break;case "readError":channel.readError=(Boolean)value;break;case "writeError":channel.writeError=(Boolean)value;break;case "keyValid":key.valid=(Boolean)value;break;case "cancelError":key.cancelError=(Boolean)value;break;case "interestError":key.interestError=(Boolean)value;break;case "wakeError":key.selector.wakeError=(Boolean)value;break;case "postError":manager.postError=(Boolean)value;break;case "nullTraffic":manager.nullTraffic=(Boolean)value;break;default:throw new IllegalArgumentException(field);}break;
        default:throw new IllegalArgumentException(name);
        }}catch(Exception e){result="ERROR";}long ended=System.currentTimeMillis();if(client.getLastReadTime()>=started&&client.getLastReadTime()<=ended)set(client,"mLastMessageRead",clock);
        var item=new JSONObject();item.put("result",result);item.put("state",snapshot(client,channel,key,manager,info));states.add(new JSONParser().parse(item.toJSONString()));}
        var out=new JSONObject();out.put("uid",uid);out.put("states",states);return out;
    }
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{System.out.println(JSONValue.toJSONString(run((JSONObject)new JSONParser().parse(line))));}catch(Exception e){System.out.println("\"ERROR\"");}}
}
