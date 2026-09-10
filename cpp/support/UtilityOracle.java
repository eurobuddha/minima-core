import java.io.*;
import java.util.*;
import java.util.zip.*;
import java.nio.charset.StandardCharsets;
import org.minima.objects.base.*;
import org.minima.database.cascade.*;
import org.minima.objects.*;
import org.minima.utils.*;
import org.minima.utils.encrypt.*;
import org.minima.utils.json.*;
import org.minima.system.params.GeneralParams;
public class UtilityOracle {
    static byte[] bytes(String s){return BaseConverter.decode16(s);}
    static String str(String s){return new String(bytes(s),StandardCharsets.UTF_8);}
    static String hx(byte[] bytes){return BaseConverter.encode16(bytes);}
    static String hx(String text){return hx(text.getBytes(StandardCharsets.UTF_8));}
    static byte[] gzip(byte[] data) throws Exception {var out=new ByteArrayOutputStream();var zip=new GZIPOutputStream(out);zip.write(data);zip.close();return out.toByteArray();}
    public static void main(String[] args) throws Exception {
        var in=new BufferedReader(new InputStreamReader(System.in));String line;File root=new File(args.length>0?args[0]:".");
        while((line=in.readLine())!=null){try{var p=line.split(" ");String op=p[0],out;
            switch(op){
            case "gen":{var key=GenerateKey.generateKeyPair();out=hx(key.getPublic().getEncoded())+"|"+hx(key.getPrivate().getEncoded());break;}
            case "key":out=hx((p[1].equals("public")?GenerateKey.convertBytesToPublic(bytes(p[2])):GenerateKey.convertBytesToPrivate(bytes(p[2]))).getEncoded());break;
            case "derive":out=hx(GenerateKey.secretKey(str(p[1]),bytes(p[2])).getEncoded());break;
            case "aesenc":out=hx(EncryptDecrypt.encryptSYM(bytes(p[1]),bytes(p[2]),bytes(p[3])));break;
            case "aesdec":out=hx(EncryptDecrypt.decryptSYM(bytes(p[1]),bytes(p[2]),bytes(p[3])));break;
            case "rsaenc":out=hx(EncryptDecrypt.encryptASM(bytes(p[1]),bytes(p[2])));break;
            case "rsadec":out=hx(EncryptDecrypt.decryptASM(bytes(p[1]),bytes(p[2])));break;
            case "sign":out=hx(SignVerify.sign(bytes(p[1]),bytes(p[2])));break;
            case "verify":out=""+SignVerify.verify(bytes(p[1]),bytes(p[2]),bytes(p[3]));break;
            case "pwenc":out=PasswordCrypto.encryptPassword(str(p[1]),new MiniData(bytes(p[2]))).to0xString();break;
            case "pwdec":out=PasswordCrypto.decryptPassword(str(p[1]),new MiniData(bytes(p[2]))).to0xString();break;
            case "packenc":{var pack=new CryptoPackage();pack.encrypt(bytes(p[2]),bytes(p[1]));out=pack.getCompleteEncryptedData().to0xString();break;}
            case "packdec":{var pack=new CryptoPackage();pack.ConvertMiniDataVersion(new MiniData(bytes(p[2])));out=hx(pack.decrypt(bytes(p[1])));break;}
            case "gzip":out=hx(gzip(bytes(p[1])));break;
            case "ungzip":{var zip=new GZIPInputStream(new ByteArrayInputStream(bytes(p[1])));out=hx(MiniFile.readAllBytes(zip));break;}
            case "size":out=hx(MiniFormat.formatSize(Long.parseLong(p[1])));break;
            case "time":out=hx(MiniFormat.ConvertMilliToTime(Long.parseLong(p[1])));break;
            case "pad":out=hx(MiniFormat.zeroPad(Integer.parseInt(p[1]),new MiniNumber(p[2])));break;
            case "type":out=hx(MiniFile.getContentType(str(p[1])));break;
            case "child":out=""+MiniFile.isChild(new File(str(p[1])),new File(str(p[2])));break;
            case "pretty":{var value=JSONValue.parseWithException(str(p[1]));out=hx(value instanceof JSONArray?MiniFormat.JSONPretty((JSONArray)value):MiniFormat.JSONPretty((JSONObject)value));break;}
            case "files":{
                File folder=new File(root,"fixture");MiniFile.deleteFileOrFolder(root.getAbsolutePath(),folder);folder.mkdirs();
                File source=new File(folder,"sub/input.dat"),copy=new File(folder,"copy.dat"),zipped=new File(folder,"file.gz"),plain=new File(folder,"plain.dat");
                byte[] payload=bytes(p[1]);MiniFile.writeDataToFile(source,payload);MiniFile.writeDataToFile(source,new byte[]{1,2,3},true);MiniFile.copyFile(source,copy);MiniFile.copyFileOrFolder(new File(folder,"sub"),new File(folder,"subcopy"));
                MiniFile.compressGzipFile(copy,zipped);MiniFile.decompressGzipFile(zipped,plain);
                out=hx(MiniFile.readCompleteFile(source))+"|"+hx(MiniFile.readCompleteFile(copy))+"|"+hx(MiniFile.readCompleteFile(zipped))+"|"+hx(MiniFile.readCompleteFile(plain))+"|"+MiniFile.getTotalFileSize(folder);
                JSONObject sizes=new JSONObject();out+="|"+MiniFile.getTotalFileSizeWithNames(folder,sizes,2,0)+"|"+hx(sizes.toString());
                File object=new File(folder,"object.dat");var original=new MiniData(payload);MiniFile.saveObject(object,original);var loaded=new MiniData("0xFE");MiniFile.loadObject(object,loaded);out+="|"+loaded.to0xString();MiniFile.saveObjectDirect(object,original);MiniFile.loadObjectSlow(object,loaded);out+="|"+loaded.to0xString();
                MiniFile.saveObjectEncrypted("password",object,original);loaded=new MiniData("0xFE");MiniFile.loadObjectEncrypted("password",object,loaded);out+="|"+loaded.to0xString();
                MiniFile.deleteFileOrFolder(root.getAbsolutePath(),new File(folder,"subcopy"));out+="|"+new File(folder,"subcopy").exists();break;}
            default:throw new IllegalArgumentException(op);
            }System.out.println(out);
        }catch(Exception e){System.out.println("ERROR");}}
    }
}
