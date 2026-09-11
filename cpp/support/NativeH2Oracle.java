import java.io.*;
import java.sql.*;
import java.util.*;
import org.minima.utils.SqlDB;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;

// Actual pinned SqlDB and actual H2 JDBC. No recording driver or SQL rewriting.
public class NativeH2Oracle {
    static class DB extends SqlDB {protected void createSQL(){}}
    static String text(JSONObject step,String key){return (String)step.get(key);}
    static int number(JSONObject step,String key){return ((Number)step.get(key)).intValue();}
    static String units(String value){if(value==null)return null;var b=new StringBuilder();for(char c:value.toCharArray())b.append(String.format("%04X",(int)c));return b.toString();}
    static JSONArray run(JSONObject request)throws Exception {
        org.h2.Driver.load();var connections=new HashMap<String,Connection>();var statements=new HashMap<String,Statement>();var rows=new HashMap<String,ResultSet>();
        var metadata=new HashMap<String,ResultSetMetaData>();var objects=new HashMap<String,Object>();var output=new JSONArray();var db=new DB();
        try{for(Object raw:(JSONArray)request.get("ops")){
            var step=(JSONObject)raw;String op=text(step,"op"),id=text(step,"id"),save=text(step,"save");var result=new JSONObject();Object value=null;
            try{switch(op){
                case "version":value=org.h2.engine.Constants.FULL_VERSION;break;
                case "open":connections.put(save,DriverManager.getConnection(text(step,"url"),text(step,"user"),text(step,"password")));break;
                case "closed":value=connections.get(id).isClosed();break;
                case "autocommit":connections.get(id).setAutoCommit(Boolean.TRUE.equals(step.get("value")));break;
                case "statement":statements.put(save,connections.get(id).createStatement());break;
                case "prepare":statements.put(save,connections.get(id).prepareStatement(text(step,"sql")));break;
                case "preparedType":value=statements.get(id) instanceof PreparedStatement;break;
                case "execute":value=statements.get(id).execute(text(step,"sql"));break;
                case "query":rows.put(save,statements.get(id).executeQuery(text(step,"sql")));break;
                case "result":rows.put(save,statements.get(id).getResultSet());value=rows.get(save)==null;break;
                case "closeStatement":statements.get(id).close();break;
                case "clear":((PreparedStatement)statements.get(id)).clearParameters();break;
                case "bind":{
                    var statement=(PreparedStatement)statements.get(id);int index=number(step,"index");Object v=step.get("value");
                    switch(text(step,"kind")){
                        case "string":statement.setString(index,(String)v);break;
                        case "int":statement.setInt(index,((Number)v).intValue());break;
                        case "long":statement.setLong(index,Long.parseLong((String)v));break;
                        case "double":statement.setDouble(index,Double.longBitsToDouble(Long.parseUnsignedLong((String)v,16)));break;
                        case "bytes":statement.setBytes(index,v==null?null:HexFormat.of().parseHex((String)v));break;
                    }break;
                }
                case "executePrepared":value=((PreparedStatement)statements.get(id)).execute();break;
                case "queryPrepared":rows.put(save,((PreparedStatement)statements.get(id)).executeQuery());break;
                case "update":value=((PreparedStatement)statements.get(id)).executeUpdate();break;
                case "next":value=rows.get(id).next();break;
                case "string":value=units(rows.get(id).getString(text(step,"column")));break;
                case "int":value=rows.get(id).getInt(text(step,"column"));break;
                case "long":value=Long.toString(rows.get(id).getLong(text(step,"column")));break;
                case "bytes":{byte[] bytes=rows.get(id).getBytes(text(step,"column"));value=bytes==null?null:HexFormat.of().withUpperCase().formatHex(bytes);break;}
                case "meta":metadata.put(save,rows.get(id).getMetaData());break;
                case "columns":value=metadata.get(id).getColumnCount();break;
                case "columnName":value=units(metadata.get(id).getColumnName(number(step,"index")));break;
                case "columnClass":value=metadata.get(id).getColumnClassName(number(step,"index"));break;
                case "object":{Object item=rows.get(id).getObject(number(step,"index"));objects.put(save,item);value=item==null?"null":item instanceof Double?"double":item instanceof Clob?"clob":"object";break;}
                case "text":value=units(objects.get(id).toString());break;
                case "double":value=String.format("%016X",Double.doubleToRawLongBits(((Double)objects.get(id)).doubleValue()));break;
                case "clobLength":value=Long.toString(((Clob)objects.get(id)).length());break;
                case "clobString":value=units(((Clob)objects.get(id)).getSubString(Long.parseLong(text(step,"start")),number(step,"length")));break;
                case "closeConnection":connections.get(id).close();break;
                case "releaseRow":rows.remove(id);break;
                case "dbLoad":db.loadDB(new File(text(step,"file")));break;
                case "dbEncrypted":db.loadEncryptedSQLDB(new File(text(step,"file")),text(step,"password"));break;
                case "dbSQL":value=db.executeGenericSQL(text(step,"sql"));break;
                case "dbBackup":db.backupToFile(new File(text(step,"file")),Boolean.TRUE.equals(step.get("gzip")));break;
                case "dbRestore":db.restoreFromFile(new File(text(step,"file")),Boolean.TRUE.equals(step.get("gzip")));break;
                case "dbSave":db.saveDB(Boolean.TRUE.equals(step.get("compact")));break;
                case "dbClose":db.hardCloseDB();break;
                case "dbOpen":value=db.isOpen();break;
                case "dbCheck":value=db.checkOpen();break;
                default:throw new IllegalArgumentException(op);
            }result.put("value",value);}catch(Throwable error){result.put("exception",error.toString());}
            output.add(result);
        }}finally{db.hardCloseDB();for(Connection connection:connections.values())connection.close();}
        return output;
    }
    public static void main(String[] ignored)throws Exception {
        Locale.setDefault(Locale.ENGLISH);var reader=new BufferedReader(new InputStreamReader(System.in));String line;
        while((line=reader.readLine())!=null)System.out.println(run((JSONObject)new JSONParser().parse(line)).toJSONString());
    }
}
