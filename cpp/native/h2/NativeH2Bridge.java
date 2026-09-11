import java.io.*;
import java.sql.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;
import org.graalvm.nativeimage.IsolateThread;
import org.graalvm.nativeimage.c.function.CEntryPoint;
import org.graalvm.nativeimage.c.type.CCharPointer;

/** C ABI for the existing H2 dependency. No org.minima classes are linked here. */
public final class NativeH2Bridge {
    private static final ConcurrentHashMap<Long,Object> objects=new ConcurrentHashMap<>();
    private static final AtomicLong nextID=new AtomicLong(1);
    private static final ThreadLocal<byte[]> reply=new ThreadLocal<>();
    private static long retain(Object object){
        if(object==null)return 0;
        long id=nextID.getAndIncrement();if(id<=0)throw new IllegalStateException("H2 handle space exhausted");
        objects.put(id,object);return id;
    }
    private static Object object(long id){
        Object value=objects.get(id);if(value==null)throw new IllegalStateException("Invalid native H2 handle");return value;
    }
    private static String string(DataInputStream in)throws IOException {
        int length=in.readInt();if(length==-1)return null;
        if(length<0||length>in.available()/2)throw new IOException("Invalid native H2 string length");
        char[] chars=new char[length];for(int i=0;i<length;i++)chars[i]=in.readChar();return new String(chars);
    }
    private static void string(DataOutputStream out,String value)throws IOException {
        out.writeInt(value==null?-1:value.length());if(value!=null)out.writeChars(value);
    }
    private static byte[] bytes(DataInputStream in)throws IOException {
        int length=in.readInt();if(length==-1)return null;
        if(length<0||length>in.available())throw new IOException("Invalid native H2 byte length");
        return in.readNBytes(length);
    }
    private static void bytes(DataOutputStream out,byte[] value)throws IOException {
        out.writeInt(value==null?-1:value.length);if(value!=null)out.write(value);
    }
    private static void dispatch(int op,long id,DataInputStream in,DataOutputStream out)throws Exception {
        switch(op){
            // JDBC may retain a ResultSet after its C++ view is released (for
            // getResultSet again). Drop that view without closing the object.
            // Connection owners outlive every native child and close on release.
            case 0:{Object value=objects.remove(id);if(value instanceof Connection connection)connection.close();break;}
            case 1:{
                org.h2.Driver.load();String url=string(in);
                if(url!=null&&url.startsWith("jdbc:mysql:"))Class.forName("com.mysql.cj.jdbc.Driver");
                out.writeLong(retain(DriverManager.getConnection(url,string(in),string(in))));break;
            }
            case 2:out.writeBoolean(((Connection)object(id)).isClosed());break;
            case 3:((Connection)object(id)).setAutoCommit(in.readBoolean());break;
            case 4:out.writeLong(retain(((Connection)object(id)).createStatement()));break;
            case 5:out.writeLong(retain(((Connection)object(id)).prepareStatement(string(in))));break;
            case 6:((Connection)object(id)).close();break;
            case 10:out.writeBoolean(((Statement)object(id)).execute(string(in)));break;
            case 11:out.writeLong(retain(((Statement)object(id)).executeQuery(string(in))));break;
            case 12:out.writeLong(retain(((Statement)object(id)).getResultSet()));break;
            case 13:((Statement)object(id)).close();break;
            case 20:((PreparedStatement)object(id)).clearParameters();break;
            case 21:((PreparedStatement)object(id)).setString(in.readInt(),string(in));break;
            case 22:((PreparedStatement)object(id)).setLong(in.readInt(),in.readLong());break;
            case 23:((PreparedStatement)object(id)).setInt(in.readInt(),in.readInt());break;
            case 24:((PreparedStatement)object(id)).setDouble(in.readInt(),in.readDouble());break;
            case 25:((PreparedStatement)object(id)).setBytes(in.readInt(),bytes(in));break;
            case 26:out.writeBoolean(((PreparedStatement)object(id)).execute());break;
            case 27:out.writeLong(retain(((PreparedStatement)object(id)).executeQuery()));break;
            case 28:out.writeInt(((PreparedStatement)object(id)).executeUpdate());break;
            case 30:out.writeBoolean(((ResultSet)object(id)).next());break;
            case 31:string(out,((ResultSet)object(id)).getString(string(in)));break;
            case 32:out.writeInt(((ResultSet)object(id)).getInt(string(in)));break;
            case 33:out.writeLong(((ResultSet)object(id)).getLong(string(in)));break;
            case 34:bytes(out,((ResultSet)object(id)).getBytes(string(in)));break;
            case 35:out.writeLong(retain(((ResultSet)object(id)).getMetaData()));break;
            case 36:{Object value=((ResultSet)object(id)).getObject(in.readInt());out.writeLong(retain(value));out.writeByte(value instanceof Double?1:value instanceof Clob?2:0);break;}
            case 40:out.writeInt(((ResultSetMetaData)object(id)).getColumnCount());break;
            case 41:string(out,((ResultSetMetaData)object(id)).getColumnName(in.readInt()));break;
            case 42:string(out,((ResultSetMetaData)object(id)).getColumnClassName(in.readInt()));break;
            case 50:string(out,object(id).toString());break;
            case 51:out.writeDouble(((Double)object(id)).doubleValue());break;
            case 52:out.writeLong(((Clob)object(id)).length());break;
            case 53:string(out,((Clob)object(id)).getSubString(in.readLong(),in.readInt()));break;
            case 60:out.writeInt(objects.size());break;
            case 61:string(out,org.h2.engine.Constants.FULL_VERSION);break;
            default:throw new IllegalArgumentException("Unknown native H2 operation: "+op);
        }
    }
    @CEntryPoint(name="minima_h2_call")
    public static int call(IsolateThread thread,int op,long id,CCharPointer input,int length){
        try{
            var buffer=new ByteArrayOutputStream();var out=new DataOutputStream(buffer);
            try{
                if(length<0)throw new IllegalArgumentException("Negative native H2 request length");
                byte[] data=new byte[length];for(int i=0;i<length;i++)data[i]=input.read(i);
                var in=new DataInputStream(new ByteArrayInputStream(data));out.writeByte(0);
                dispatch(op,id,in,out);
                if(in.available()!=0)throw new IllegalArgumentException("Trailing native H2 request bytes");
            }catch(Throwable error){
                buffer.reset();out.writeByte(error instanceof OutOfMemoryError?4:error instanceof Error?3:error instanceof SQLException?1:2);
                string(out,error.getClass().getName());string(out,error.getMessage());
            }
            byte[] result=buffer.toByteArray();reply.set(result);return result.length;
        }catch(Throwable failure){reply.remove();return -1;} // Never unwind an exception across C.
    }
    @CEntryPoint(name="minima_h2_copy")
    public static int copy(IsolateThread thread,CCharPointer output,int capacity){
        try{
            byte[] data=reply.get();if(data==null||capacity<data.length)return -1;
            for(int i=0;i<data.length;i++)output.write(i,data[i]);return data.length;
        }finally{reply.remove();}
    }
    @CEntryPoint(name="minima_h2_discard")
    public static void discard(IsolateThread thread){reply.remove();}
}
