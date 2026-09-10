import java.io.*;
import java.util.*;
import org.minima.objects.base.*;
import org.minima.utils.BIP39;
import org.minima.utils.json.*;
import org.minima.utils.json.parser.JSONParser;
public class BIP39Oracle {
    public static void main(String[] args)throws Exception{var input=new BufferedReader(new InputStreamReader(System.in));String line;while((line=input.readLine())!=null){String[] saved=BIP39.WORD_LIST.clone();try{var data=(JSONObject)new JSONParser().parse(line);Locale.setDefault(Locale.forLanguageTag((String)data.get("locale")));String op=(String)data.get("op");if(data.containsKey("replace")){var change=(JSONArray)data.get("replace");BIP39.WORD_LIST[((Number)change.get(0)).intValue()]=(String)change.get(1);}String result;
        if(op.equals("clean"))result=BIP39.cleanSeedPhrase((String)data.get("text"));
        else if(op.equals("seed"))result=BIP39.convertStringToSeed((String)data.get("text")).to0xString();
        else if(op.equals("list")||op.equals("listseed")){var list=(JSONArray)data.get("words");String[] words=list==null?null:(String[])list.toArray(new String[0]);result=op.equals("list")?BIP39.convertWordListToString(words):BIP39.convertWordListToSeed(words).to0xString();}
        else if(op.equals("words")){var list=new JSONArray();Collections.addAll(list,BIP39.WORD_LIST);System.out.println(list.toJSONString());continue;}
        else{var words=BIP39.getNewWordList();if(words.length!=24)throw new Exception("wrong length");for(String word:words){boolean found=false;for(String candidate:BIP39.WORD_LIST)if(candidate.toUpperCase().trim().equals(word)){found=true;break;}if(!found)throw new Exception("wrong word");}result="VALID";}
        System.out.println(JSONValue.toJSONString(result));
    }catch(IllegalArgumentException e){System.out.println(JSONValue.toJSONString("ERROR:"+e.getMessage()));}catch(Exception e){System.out.println("\"ERROR\"");}finally{System.arraycopy(saved,0,BIP39.WORD_LIST,0,2048);}}}
}
