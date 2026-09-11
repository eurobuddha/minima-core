import java.io.*;
import java.lang.reflect.*;
import java.nio.charset.StandardCharsets;
import java.util.zip.ZipEntry;
import org.minima.objects.base.MiniData;
import org.minima.utils.ZipExtractor;
import org.minima.utils.json.*;

public class ZipExtractorOracle {
    static class Counted extends InputStream {
        byte[] bytes; int position,closes,chunk,fail; boolean failClose;
        JSONArray reads=new JSONArray();
        Counted(JSONObject request) {
            bytes=new MiniData((String)request.get("bytes")).getBytes();
            chunk=request.containsKey("chunk")?((Number)request.get("chunk")).intValue():4096;
            fail=request.containsKey("failAt")?((Number)request.get("failAt")).intValue():-1;
            failClose=Boolean.TRUE.equals(request.get("failClose"));
        }
        public int read() throws IOException {byte[] b=new byte[1];return read(b,0,1)==-1?-1:b[0]&255;}
        public int read(byte[] out,int offset,int length) throws IOException {
            JSONArray event=new JSONArray();event.add(length);
            if(fail>=0&&position>=fail){event.add("throw");reads.add(event);throw new IOException("fixture read failure");}
            int count=Math.min(Math.min(length,chunk),bytes.length-position);
            if(fail>=0)count=Math.min(count,fail-position);
            System.arraycopy(bytes,position,out,offset,count);position+=count;
            event.add(count==0?-1:count);reads.add(event);return count==0?-1:count;
        }
        public void close() throws IOException {closes++;if(failClose)throw new IOException("fixture close failure");}
    }
    public static void main(String[] args) throws Exception {
        Method helper=ZipExtractor.class.getDeclaredMethod("newFile",File.class,ZipEntry.class);helper.setAccessible(true);
        BufferedReader input=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=input.readLine())!=null) {
            JSONObject request=(JSONObject)JSONValue.parseWithException(line),result=new JSONObject();
            JSONArray logs=new JSONArray();Counted counted=new Counted(request);
            File destination=request.get("destination")==null?null:new File((String)request.get("destination"));
            PrintStream original=System.out;
            System.setOut(new PrintStream(new ByteArrayOutputStream()) {
                @Override public void println(String text) {int marker=text.indexOf("] : ");logs.add(marker<0?text:text.substring(marker+4));}
            });
            try {
                if(Boolean.TRUE.equals(request.get("helper"))) {
                    File file=(File)helper.invoke(null,destination,new ZipEntry((String)request.get("name")));
                    result.put("path",file.toString());
                } else {
                    ZipExtractor.unzip(Boolean.TRUE.equals(request.get("nullInput"))?null:counted,destination);result.put("success",true);
                }
            } catch(Exception error) {
                Throwable cause=error instanceof InvocationTargetException?error.getCause():error;
                result.put("exception",cause.getClass().getName());
                result.put("message",cause instanceof NullPointerException||cause.getMessage()==null?"":cause.getMessage());
            } finally {System.setOut(original);}
            result.put("position",counted.position);result.put("closes",counted.closes);result.put("reads",counted.reads);result.put("logs",logs);
            System.out.println(result.toJSONString());
        }
    }
}
