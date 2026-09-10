import java.net.*;
import java.nio.channels.*;
public class NIOSocketProfile {
    public static void main(String[] args)throws Exception{
        try(var server=ServerSocketChannel.open()){
            server.configureBlocking(false);
            server.socket().bind(new InetSocketAddress(InetAddress.getByName("0.0.0.0").getHostName(),0));
            int port=((InetSocketAddress)server.getLocalAddress()).getPort();
            System.out.println("bound="+server.getLocalAddress().getClass().getSimpleName()+":"+((InetSocketAddress)server.getLocalAddress()).getAddress().getHostAddress());
            for(String host:new String[]{"127.0.0.1","::1"}){
                try(var client=new Socket()){
                    client.connect(new InetSocketAddress(host,port),1000);
                    SocketChannel accepted=null;long end=System.nanoTime()+1000000000L;
                    while(accepted==null&&System.nanoTime()<end){accepted=server.accept();Thread.sleep(1);}
                    if(accepted==null)throw new Exception("accept timed out");
                    try(var peer=accepted){System.out.println(host+"=accepted:"+((InetSocketAddress)peer.getRemoteAddress()).getAddress().getHostAddress());}
                }catch(Exception e){System.out.println(host+"="+e.getClass().getSimpleName());}
            }
        }
    }
}
