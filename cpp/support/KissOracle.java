import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.minima.kissvm.values.*;
import org.minima.kissvm.tokens.*;
import org.minima.kissvm.exceptions.*;
import org.minima.objects.base.*;
import org.minima.utils.BaseConverter;
import org.minima.utils.json.*;
public class KissOracle {
    static String str(String hex) {return new String(BaseConverter.decode16(hex),StandardCharsets.UTF_8);}
    static JSONArray result(Value value) {
        var out=new JSONArray();out.add(value.getValueType());out.add(value.toString());
        if(value instanceof HexValue h) out.add(BaseConverter.encode16(h.getRawData()));
        if(value instanceof StringValue s) out.add(BaseConverter.encode16(s.getBytes()));
        if(value instanceof NumberValue n) out.add(MiniData.getMiniDataVersion(n.getNumber()).to0xString());
        return out;
    }
    public static void main(String[] args) throws Exception {
        var input=new BufferedReader(new InputStreamReader(System.in,StandardCharsets.UTF_8));String line;
        while((line=input.readLine())!=null) {
            String out;
            try {
                var a=line.split(" ");var rows=new JSONArray();
                switch(a[0]) {
                case "value":out=result(Value.getValue(str(a[1]))).toString();break;
                case "type":out=""+Value.getValueType(str(a[1]));break;
                case "hexnumber":out=result(new HexValue(new MiniNumber(str(a[1])))).toString();break;
                case "size": {int n=Integer.parseInt(a[2]);out=result(a[1].equals("hex")?new HexValue(new byte[n]):new StringValue("🐼".repeat(n))).toString();break;}
                case "verify": {var value=Value.getValue(str(a[1]));value.verifyType(Integer.parseInt(a[2]));out="OK";break;}
                case "same": {Value.checkSameType(Value.getValue(str(a[1])),Value.getValue(str(a[2])),Integer.parseInt(a[3]));out="OK";break;}
                case "number": {var x=new NumberValue(str(a[1]));var y=new NumberValue(str(a[2]));rows.add(result(x.add(y)));rows.add(result(x.sub(y)));rows.add(result(x.mult(y)));rows.add(result(x.div(y)));rows.add(x.isEqual(y));rows.add(x.isLess(y));rows.add(x.isMoreEqual(y));out=rows.toString();break;}
                case "string":out=result(new StringValue(str(a[1])).add(new StringValue(str(a[2])))).toString();break;
                case "tokens": {var tokens=new ScriptTokenizer(str(a[1]),a[2].equals("1")).tokenize();for(var token:tokens) {var row=new JSONArray();row.add(token.getTokenType());row.add(token.getToken());row.add(token.toString());rows.add(row);}out=rows.toString();break;}
                case "lexical": {var tokens=ScriptToken.tokenize(str(a[1]));var lexical=new LexicalTokenizer(tokens,Integer.parseInt(a[2]));rows.add(lexical.getStackDepth());lexical.incrementStackDepth();lexical.decrementStackDepth();lexical.decrementStackDepth();rows.add(lexical.getStackDepth());while(lexical.hasMoreElements()) {rows.add(lexical.getCurrentPosition());rows.add(lexical.getNextToken().toString());}rows.add(lexical.checkAllTokensUsed());if(!tokens.isEmpty()) {lexical.goBackToken();rows.add(lexical.getNextToken().toString());}out=rows.toString();break;}
                case "check": {var word=str(a[1]);rows.add(ScriptTokenizer.isNumeric(word));rows.add(ScriptTokenizer.isHex(word));rows.add(ScriptTokenizer.isVariable(word));rows.add(ScriptTokenizer.isGlobal(word));rows.add(ScriptTokenizer.isWhiteSpace(word));out=rows.toString();break;}
                default:throw new IllegalArgumentException();
                }
            }catch(MinimaException error) {out="PARSE|"+error.getMessage();}
            catch(Exception error) {out="ERROR";}
            System.out.println(BaseConverter.encode16(out.getBytes(StandardCharsets.UTF_8)));
        }
    }
}
