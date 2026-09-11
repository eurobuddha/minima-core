import java.io.*;
import java.sql.*;
import java.lang.reflect.*;
import java.util.*;
import java.util.logging.Logger;
import org.minima.utils.SqlDB;
import org.minima.objects.base.MiniData;
import org.minima.system.Main;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

// Only JDBC is a recording fixture. Every SqlDB method is the pinned source.
public class SqlDBOracle {
    static JSONObject config;
    static JSONArray events;
    static int calls, connectionID, preparedID;
    static Map<String,Integer> queryBatches=new HashMap<>();
    static int integer(JSONObject o,String key,int fallback) { Object v=o.get(key); return v==null?fallback:((Number)v).intValue(); }
    static boolean flag(String key) { return Boolean.TRUE.equals(config.get(key)); }
    static void event(String method,Object... args) throws SQLException {
        JSONArray e=new JSONArray();e.add(method);for(Object a:args)e.add(a);events.add(e);
        if(++calls==integer(config,"fail",-1)){if(flag("runtimeFailure"))throw new IllegalStateException("fixture failure");throw new SQLException("fixture failure");}
    }
    interface Invoke { Object call(Method method,Object[] args)throws Throwable; }
    @SuppressWarnings("unchecked") static <T> T proxy(Class<T> type,Invoke invoke) {
        return (T)Proxy.newProxyInstance(SqlDBOracle.class.getClassLoader(),new Class[]{type},(p,m,a)->invoke.call(m,a));
    }
    static JSONArray columns() { return (JSONArray)config.get("columns"); }
    static JSONArray rows() { return (JSONArray)config.get("rows"); }
    static Object cell(Object value) {
        if(value==null)return null;
        if(value instanceof JSONObject d && d.containsKey("doubleBits"))return Double.longBitsToDouble(Long.parseUnsignedLong(d.getString("doubleBits"),16));
        if(value instanceof JSONObject c && c.containsKey("clob"))return proxy(Clob.class,(m,a)->{
            if(m.getName().equals("length")){event("clob.length");return c.containsKey("length")?((Number)c.get("length")).longValue():(long)((String)c.get("clob")).length();}
            if(m.getName().equals("getSubString")){event("clob.getSubString",a);return (String)c.get("clob");}
            if(m.getName().equals("toString")){event("object.toString");return c.get("text");}
            throw new UnsupportedOperationException(m.getName());
        });
        return new Object(){public String toString(){try{event("object.toString");}catch(SQLException e){throw new RuntimeException(e.getMessage());}return (String)value;}};
    }
    static JSONArray rows(String sql) {
        if(config.containsKey("rowsBySQL")){var choices=(JSONObject)config.get("rowsBySQL");if(choices.containsKey(sql))return (JSONArray)choices.get(sql);}
        return rows();
    }
    static ResultSet resultSet() {return resultSet("");}
    static ResultSet resultSet(String sql) {
        int[] row={-1};
        JSONArray selected=rows(sql);
        if(config.containsKey("rowsBySQLBatches")){var choices=(JSONObject)config.get("rowsBySQLBatches");if(choices.containsKey(sql)){var batches=(JSONArray)choices.get(sql);int index=queryBatches.getOrDefault(sql,0);queryBatches.put(sql,index+1);selected=index<batches.size()?(JSONArray)batches.get(index):new JSONArray();}}
        final JSONArray selectedRows=selected;
        return proxy(ResultSet.class,(m,a)->{
            String name=m.getName();
            if(name.equals("next")){event("rows.next");return ++row[0]<selectedRows.size();}
            if(name.equals("getObject")){event("rows.getObject",a);return cell(((JSONArray)selectedRows.get(row[0])).get((int)a[0]-1));}
            if(name.equals("getString")){event("rows.getString",a);return (String)((JSONObject)selectedRows.get(row[0])).get((String)a[0]);}
            if(name.equals("getBytes")){event("rows.getBytes",a);String v=(String)((JSONObject)selectedRows.get(row[0])).get((String)a[0]);return v==null?null:new MiniData(v).getBytes();}
            if(name.equals("getLong")){event("rows.getLong",a);Object v=((JSONObject)selectedRows.get(row[0])).get((String)a[0]);return v==null?0L:((Number)v).longValue();}
            if(name.equals("getInt")){event("rows.getInt",a);return integer((JSONObject)selectedRows.get(row[0]),(String)a[0],0);}
            if(name.equals("getMetaData")){
                event("rows.getMetaData");if(flag("nullMeta"))return null;
                return proxy(ResultSetMetaData.class,(mm,aa)->{
                    String n=mm.getName();event("meta."+n,aa==null?new Object[0]:aa);
                    if(n.equals("getColumnCount"))return columns().size();
                    JSONObject col=(JSONObject)columns().get((int)aa[0]-1);
                    if(n.equals("getColumnName"))return col.get("name");
                    if(n.equals("getColumnClassName"))return col.get("class");
                    throw new UnsupportedOperationException(n);
                });
            }
            throw new UnsupportedOperationException(name);
        });
    }
    static Statement statement() {
        return proxy(Statement.class,(m,a)->{
            String name=m.getName();event("statement."+name,a==null?new Object[0]:a);
            if(name.equals("execute"))return flag("results");
            if(name.equals("getResultSet")||name.equals("executeQuery"))return flag("nullRows")?null:resultSet();
            if(name.equals("close"))return null;
            throw new UnsupportedOperationException(name);
        });
    }
    static Connection connection() {
        int id=++connectionID;boolean[] closed={false};
        return proxy(Connection.class,(m,a)->{
            String name=m.getName();
            if(name.equals("setAutoCommit")||name.equals("prepareStatement"))event("connection."+name,id,a[0]);else event("connection."+name,id);
            if(name.equals("isClosed"))return closed[0];
            if(name.equals("close")){closed[0]=true;return null;}
            if(name.equals("setAutoCommit"))return null;
            if(name.equals("createStatement"))return flag("nullStatement")?null:statement();
            if(name.equals("prepareStatement"))return flag("nullPrepared")?null:preparedStatement((String)a[0]);
            throw new UnsupportedOperationException(name);
        });
    }
    static PreparedStatement preparedStatement() {return preparedStatement("");}
    static PreparedStatement preparedStatement(String sql) {
        int id=++preparedID;
        return proxy(PreparedStatement.class,(m,a)->{
            String name=m.getName();
            if(name.equals("setDouble"))event("prepared."+name,id,a[0],Double.toString((Double)a[1]));
            else if(name.equals("setBytes"))event("prepared."+name,id,a[0],a[1]==null?null:new MiniData((byte[])a[1]).to0xString());
            else if(name.equals("setString")||name.equals("setLong")||name.equals("setInt"))event("prepared."+name,id,a[0],a[1]);else event("prepared."+name,id);
            if(name.equals("executeQuery"))return flag("nullRows")?null:resultSet(sql);
            if(name.equals("executeUpdate"))return integer(config,"updated",0);
            if(name.equals("execute"))return flag("results");
            if(name.equals("clearParameters")||name.equals("setDouble")||name.equals("setString")||name.equals("setLong")||name.equals("setInt")||name.equals("setBytes")||name.equals("close"))return null;
            throw new UnsupportedOperationException(name);
        });
    }
    static class DB extends SqlDB {
        protected void createSQL() throws SQLException { event("createSQL"); }
        void clear(){mSQLConnection=null;}
    }
    static File file(JSONObject op,String root) { return op.containsKey("file")?(op.get("file")==null?null:new File((String)op.get("file"))):new File(root+"/db/main"); }
    static JSONObject run(JSONObject input,String root)throws Exception {
        connectionID=0;DB db=new DB();JSONArray outputs=new JSONArray();
        for(Object entry:(JSONArray)input.get("ops")) {
            config=(JSONObject)entry;events=new JSONArray();calls=0;Main.STARTUP_DEBUG_LOGS=flag("debug");
            JSONObject output=new JSONObject();ByteArrayOutputStream log=new ByteArrayOutputStream();PrintStream previous=System.out;System.setOut(new PrintStream(log,true,"UTF-8"));
            File f=file(config,root);
            if(flag("touch")){f.getParentFile().mkdirs();try(var out=new FileOutputStream(f)){out.write(42);}}
            try {
                Object result=null;String command=(String)config.get("cmd");
                switch(command){
                    case "load":db.loadDB(f);break;
                    case "encrypted":db.loadEncryptedSQLDB(f,(String)config.get("password"));break;
                    case "open":result=db.isOpen();break;
                    case "check":result=db.checkOpen(!Boolean.FALSE.equals(config.get("logs")));break;
                    case "hard":db.hardCloseDB();break;
                    case "save":db.saveDB(flag("compact"));break;
                    case "backup":db.backupToFile(f,flag("gzip"));break;
                    case "restore":db.restoreFromFile(f,flag("gzip"));break;
                    case "sql":result=db.executeGenericSQL((String)config.get("sql"));break;
                    case "clear":db.clear();break;
                    case "decode":result=SqlDB.convertDataToJSONObject(new MiniData((String)config.get("hex")));break;
                    case "encode":result=SqlDB.convertJSONObjectToData((JSONObject)config.get("json")).to0xString();break;
                    default:throw new IllegalArgumentException(command);
                }
                output.put("value",result);
            } catch(Exception error){output.put("exception",error.toString());}
            finally {System.setOut(previous);}
            JSONArray logs=new JSONArray();for(String line:log.toString("UTF-8").split("\n")){int index=line.indexOf("] : ");if(index>=0){String body=line.substring(index+4);if(!body.startsWith("     "))logs.add(body);}}
            output.put("logs",logs);output.put("events",events);output.put("file",db.getSQLFile()==null?null:db.getSQLFile().toString());output.put("exists",f!=null&&f.exists());outputs.add(output);
        }
        JSONObject result=new JSONObject();result.put("ops",outputs);return result;
    }
    static void installDriver() throws Exception {installDriver("jdbc:h2:");}
    static void installDriver(String prefix) throws Exception {
        // Remove auto-loaded H2: this fixture must never touch a real SQL database.
        for(Driver driver:Collections.list(DriverManager.getDrivers()))DriverManager.deregisterDriver(driver);
        DriverManager.registerDriver(new Driver(){
            public boolean acceptsURL(String url){return url.startsWith(prefix);}
            public Connection connect(String url,Properties properties)throws SQLException{if(!acceptsURL(url))return null;event("connect",url,properties.getProperty("user"),properties.getProperty("password"));return flag("nullConnection")?null:connection();}
            public DriverPropertyInfo[] getPropertyInfo(String u,Properties p){return new DriverPropertyInfo[0];}
            public int getMajorVersion(){return 1;}public int getMinorVersion(){return 0;}public boolean jdbcCompliant(){return false;}public Logger getParentLogger(){return Logger.getGlobal();}
        });
    }
    public static void main(String[] args)throws Exception {
        installDriver();
        var input=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=input.readLine())!=null)try{System.out.println(run((JSONObject)new JSONParser().parse(line),args[0]).toJSONString());}catch(Exception error){System.out.println("{\"fatal\":true}");error.printStackTrace(System.err);}
    }
}
