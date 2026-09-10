import java.io.*;
import java.security.*;
import java.security.cert.CertificateFactory;
import javax.net.ssl.*;
import org.minima.utils.RPCClient;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class RPCClientOracle {
    static SSLContext context(String ca)throws Exception{
        if(ca==null)return null;var store=KeyStore.getInstance(KeyStore.getDefaultType());store.load(null,null);
        try(var input=new FileInputStream(ca)){store.setCertificateEntry("fixture",CertificateFactory.getInstance("X.509").generateCertificate(input));}
        var trust=TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());trust.init(store);var context=SSLContext.getInstance("TLS");context.init(null,trust.getTrustManagers(),null);return context;
    }
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null)try{
        var request=(JSONObject)new JSONParser().parse(line);RPCClient.USER_AGENT=request.containsKey("agent")?(String)request.get("agent"):"Minima/1.0";
        String url=(String)request.get("url"),user=(String)request.get("user"),password=(String)request.get("password"),data=(String)request.get("data"),type=(String)request.get("type");var ssl=context((String)request.get("ca"));var output=new JSONObject();var log=new ByteArrayOutputStream();var previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
        try{String result;switch((String)request.get("method")){
            case "get":result=RPCClient.sendGET(url);break;case "basic":result=RPCClient.sendGETBasicAuth(url,user,password);break;case "getHTTPS":result=RPCClient.sendGETHTTPS(url);break;case "getSSL":result=RPCClient.sendGETSSL(url);break;case "basicSSL":result=RPCClient.sendGETBasicAuthSSL(url,user,password,ssl);break;case "put":result=RPCClient.sendPUT(url);break;case "post":result=RPCClient.sendPOST(url,data);break;case "postType":result=RPCClient.sendPOST(url,data,type);break;case "postHTTPS":result=RPCClient.sendPOSTHTTPS(url,data,type);break;case "auth":result=RPCClient.sendGETAuth(url,(String)request.get("token"));break;default:throw new IllegalArgumentException((String)request.get("method"));
        }output.put("result",result);}catch(Exception error){output.put("exception",error.toString());}finally{System.setOut(previous);}
        output.put("stdout",log.toString("UTF-8"));System.out.println(output.toJSONString());
    }catch(Exception error){error.printStackTrace(System.err);System.out.println("{\"fatal\":true}");}}
}
