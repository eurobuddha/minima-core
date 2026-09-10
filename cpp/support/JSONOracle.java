package org.minima.utils.json.parser;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.utils.BaseConverter;
import org.minima.utils.json.*;
public class JSONOracle {
    static class Handler implements ContentHandler {
        JSONArray events=new JSONArray();int count,step;boolean ended;
        Handler(int step) {this.step=step;}
        boolean event(String type,Object value) {var row=new JSONArray();row.add(type);row.add(value);events.add(row);return step==0||++count%step!=0;}
        public void startJSON() {event("startJSON",null);}
        public void endJSON() {event("endJSON",null);ended=true;}
        public boolean startObject() {return event("startObject",null);}
        public boolean endObject() {return event("endObject",null);}
        public boolean startObjectEntry(String key) {return event("key",key);}
        public boolean endObjectEntry() {return event("endEntry",null);}
        public boolean startArray() {return event("startArray",null);}
        public boolean endArray() {return event("endArray",null);}
        public boolean primitive(Object value) {return event("value",value);}
    }
    public static void main(String[] args) throws Exception {
        var input=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=input.readLine())!=null) {
            String out;var events=new JSONArray();
            try {
                var a=line.split(" ");String text=new String(BaseConverter.decode16(a[2]),StandardCharsets.UTF_8);int mode=Integer.parseInt(a[1]);
                if(a[0].equals("lex")) {
                    var lexer=new Yylex(new StringReader(text));Yytoken token;
                    while((token=lexer.yylex())!=null) {var row=new JSONArray();row.add(lexer.getPosition());row.add(token.type);row.add(token.value);events.add(row);}
                    out=events+"|"+lexer.getPosition();
                } else if(a[0].equals("stream")) {
                    var handler=new Handler(mode);events=handler.events;var parser=new JSONParser();parser.parse(text,handler,false);
                    for(int i=0;i<10000&&!handler.ended;i++) parser.parse("ignored on resume",handler,true);
                    out=events+"|"+parser.getPosition();
                } else {
                    var parser=new JSONParser();Object value=parser.parse(text);out=JSONValue.toJSONString(value)+"|"+parser.getPosition();
                }
            } catch(ParseException e) {out="PARSE|"+e.getErrorType()+"|"+e.getPosition()+"|"+e.getMessage()+"|"+events;}
            catch(Exception e) {out="ERROR|"+events;}
            System.out.println(BaseConverter.encode16(out.getBytes(StandardCharsets.UTF_8)));
        }
    }
}
