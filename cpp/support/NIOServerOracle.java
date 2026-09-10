import java.io.*;
import java.net.*;
import java.nio.channels.*;
import java.util.*;
import java.util.concurrent.*;
import org.minima.system.Main;
import org.minima.system.network.NetworkManager;
import org.minima.system.network.minima.*;
import org.minima.objects.base.MiniData;
import org.minima.utils.messages.Message;

/** Actual pinned NIOServer/NIOClient, with only the application manager replaced. */
public class NIOServerOracle extends NIOClientOracle {
    static void require(boolean value,String why){if(!value)throw new IllegalStateException(why);}
    static class Collector extends NIOManager {
        BlockingQueue<Message> queue;
        NIOTraffic traffic;
        Collector(){super(null);}
        public NIOTraffic getTrafficListener(){return traffic;}
        public void PostMessage(Message message){queue.add(message);}
        Message next(String type)throws Exception{var msg=queue.poll(5,TimeUnit.SECONDS);require(msg!=null,"event timeout: "+type);require(msg.isMessageType(type),"unexpected event: "+msg);return msg;}
    }
    static Collector collector()throws Exception{
        var manager=(Collector)U.allocateInstance(Collector.class);manager.queue=new LinkedBlockingQueue<>();manager.traffic=new NIOTraffic();
        var network=(NetworkManager)U.allocateInstance(NetworkManager.class);set(network,"mNIOManager",manager);
        var main=(Main)U.allocateInstance(Main.class);set(main,"mNetwork",network);field(Main.class,"mMainInstance").set(null,main);return manager;
    }
    static Socket connect(int port,String host)throws Exception{var socket=new Socket(host,port);socket.setSoTimeout(5000);return socket;}
    static byte[] frame(byte[] payload)throws Exception{var bytes=new ByteArrayOutputStream();var output=new DataOutputStream(bytes);output.writeInt(payload.length);output.write(payload);return bytes.toByteArray();}
    static byte[] read(Socket socket,int size)throws Exception{byte[] bytes=socket.getInputStream().readNBytes(size);require(bytes.length==size,"read EOF");return bytes;}
    static void received(Collector queue,NIOClient client,byte[] payload)throws Exception{var msg=queue.next("NIO_NEWMSG");require(msg.getString("uid").equals(client.getUID()),"message UID");require(msg.getString("fullhost").equals(client.getFullAddress()),"message host");require(((MiniData)msg.getObject("data")).isEqual(new MiniData(payload)),"message data");}
    static void disconnected(Collector queue,NIOClient client,boolean reconnect)throws Exception{var msg=queue.next("NIO_DISCONNECTED");require(msg.getObject("client")==client,"disconnect identity");require(msg.getBoolean("reconnect")==reconnect,"reconnect flag");}
    public static void main(String[] args)throws Exception{
        var queue=collector();if(args.length==2){var server=new NIOServer(Integer.parseInt(args[1]),queue);boolean early=args[0].equals("pre-shutdown");if(early)server.shutdown();server.run();if(early){queue.next("NIO_SERVERSTARTED");require(queue.queue.isEmpty()&&server.getNetClientSize()==0,"pre-shutdown state");((Selector)get(server,"mSelector")).close();}require(!server.isRunning(),"failed startup running");System.out.println("returned");return;}var before=new NIOServer(0,queue);require(!before.isRunning()&&before.getNetClientSize()==0,"initial state");boolean failed=false;try{before.disconnect("missing");}catch(NullPointerException e){failed=true;}require(failed,"prestart selector exception");before.shutdown();
        var server=new NIOServer(0,queue);var worker=new Thread(server);worker.setDaemon(true);worker.start();
        List<AutoCloseable> resources=new ArrayList<>();
        try{
            queue.next("NIO_SERVERSTARTED");require(server.isRunning(),"running");System.out.println("started");
            var selector=(Selector)get(server,"mSelector");resources.add(selector);int port=0;
            for(var key:selector.keys())if(key.channel() instanceof ServerSocketChannel channel)port=((InetSocketAddress)channel.getLocalAddress()).getPort();require(port>0,"bound port");
            var ipv4=connect(port,"127.0.0.1");resources.add(ipv4);var c4=(NIOClient)queue.next("NIO_NEWCONNECT").getObject("client");require(c4.isIncoming()&&c4.getHost().equals("127.0.0.1"),"IPv4 metadata");require(server.getClient(c4.getUID())==c4,"map identity");System.out.println("incoming4");
            var ipv6=connect(port,"::1");resources.add(ipv6);var c6=(NIOClient)queue.next("NIO_NEWCONNECT").getObject("client");require(c6.isIncoming()&&c6.getHost().equals("0:0:0:0:0:0:0:1"),"IPv6 metadata");System.out.println("incoming6");
            var listener=new ServerSocket(0,1,InetAddress.getByName("127.0.0.1"));resources.add(listener);var channel=SocketChannel.open(new InetSocketAddress("127.0.0.1",listener.getLocalPort()));resources.add(channel);var peer=listener.accept();resources.add(peer);peer.setSoTimeout(5000);
            server.regsiterNewSocket(channel);var outgoing=(NIOClient)queue.next("NIO_NEWCONNECT").getObject("client");require(outgoing.isOutgoing()&&outgoing.getPort()==listener.getLocalPort(),"outgoing metadata");require(server.getAllNIOClients().size()==3,"client count");System.out.println("outgoing");
            long totalRead=0,totalWrite=0;
            for(int n:new int[]{0,1,8193,65536}){
                byte[] payload=new byte[n];for(int i=0;i<n;i++)payload[i]=(byte)(i*37+n);var bytes=frame(payload);
                for(int i=0;i<4;i++)ipv4.getOutputStream().write(bytes[i]);ipv4.getOutputStream().write(payload);received(queue,c4,payload);totalRead+=bytes.length;
                server.sendMessage(c6.getUID(),new MiniData(payload));require(Arrays.equals(read(ipv6,bytes.length),bytes),"direct send");totalWrite+=bytes.length;System.out.println("frame:"+n);
            }
            c4.setValidGreeting(true);outgoing.setValidGreeting(true);var broadcast=frame(new byte[]{4,5,6});server.sendMessageAll(new MiniData(new byte[]{4,5,6}));require(Arrays.equals(read(ipv4,broadcast.length),broadcast)&&Arrays.equals(read(peer,broadcast.length),broadcast),"broadcast");ipv6.setSoTimeout(100);boolean quiet=false;try{ipv6.getInputStream().read();}catch(SocketTimeoutException e){quiet=true;}require(quiet,"invalid greeting received broadcast");ipv6.setSoTimeout(5000);totalWrite+=2*broadcast.length;System.out.println("broadcast-filter");
            server.sendMessage("missing",null);server.disconnect("missing");server.disconnect(c6.getUID());disconnected(queue,c6,false);require(server.getClient(c6.getUID())==null&&server.getNetClientSize()==2,"explicit remove");System.out.println("explicit-disconnect");
            peer.close();disconnected(queue,outgoing,true);require(server.getNetClientSize()==1,"outgoing remove");System.out.println("outgoing-eof");
            ipv4.getOutputStream().write(new byte[]{0x10,0,0,1});totalRead+=4;disconnected(queue,c4,false);require(server.getNetClientSize()==0,"invalid frame remove");System.out.println("oversized-frame");
            var finalPeer=connect(port,"127.0.0.1");resources.add(finalPeer);var finalClient=(NIOClient)queue.next("NIO_NEWCONNECT").getObject("client");server.shutdown();worker.join(5000);require(!worker.isAlive()&&!server.isRunning()&&server.getNetClientSize()==1&&server.getClient(finalClient.getUID())==finalClient,"shutdown retained map");require(finalPeer.getInputStream().read()==-1,"shutdown EOF");require(queue.queue.isEmpty(),"unexpected shutdown event");require(queue.traffic.getTotalRead()==totalRead&&queue.traffic.getTotalWrite()==totalWrite,"traffic totals");System.out.println("shutdown-retains-map\ntraffic:"+totalRead+":"+totalWrite);
        }finally{server.shutdown();worker.join(5000);Collections.reverse(resources);for(var resource:resources)resource.close();}
    }
}
