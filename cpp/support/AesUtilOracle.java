import java.io.*;
import java.lang.reflect.*;
import java.nio.charset.StandardCharsets;
import javax.crypto.*;
import javax.crypto.spec.*;
import org.minima.objects.base.MiniData;
import org.minima.utils.encrypt.javajs.AesUtil;
import org.minima.utils.json.*;

public class AesUtilOracle {
    static String text(Object value) {
        if (value == null) return null;
        StringBuilder out = new StringBuilder();
        for (Object c : (JSONArray)value) out.append((char)((Number)c).intValue());
        return out.toString();
    }
    static byte[] bytes(Object value) { return value == null ? null : new MiniData((String)value).getBytes(); }
    static String units(String value) {
        StringBuilder out = new StringBuilder();
        for (char c : value.toCharArray()) out.append(String.format("%04x", (int)c));
        return out.toString();
    }
    static JSONArray result(Object value) {
        JSONArray out = new JSONArray();
        out.add(value == null ? "null" : value instanceof String ? "text" : "bytes");
        if (value != null) out.add(value instanceof String ? units((String)value) : new MiniData((byte[])value).to0xString());
        return out;
    }
    public static void main(String[] args) throws Exception {
        if (args.length > 0) { AesUtil.main(new String[0]); return; }
        Method derive = AesUtil.class.getDeclaredMethod("generateKey", String.class, String.class);
        Method finish = AesUtil.class.getDeclaredMethod("doFinal", int.class, SecretKey.class, String.class, byte[].class);
        Method fail = AesUtil.class.getDeclaredMethod("fail", Exception.class);
        derive.setAccessible(true); finish.setAccessible(true); fail.setAccessible(true);
        var input = new BufferedReader(new InputStreamReader(System.in, StandardCharsets.UTF_8));
        String line;
        while ((line = input.readLine()) != null) {
            JSONObject request = (JSONObject)JSONValue.parseWithException(line);
            AesUtil aes = new AesUtil(((Number)request.get("bits")).intValue(), ((Number)request.get("iterations")).intValue());
            JSONArray results = new JSONArray();
            for (Object value : (JSONArray)request.get("ops")) {
                JSONObject op = (JSONObject)value;
                try {
                    Object out;
                    String name = (String)op.get("op");
                    switch (name) {
                    case "encrypt": out = aes.encrypt(text(op.get("salt")), text(op.get("iv")), text(op.get("password")), text(op.get("data"))); break;
                    case "decrypt": out = aes.decrypt(text(op.get("salt")), text(op.get("iv")), text(op.get("password")), text(op.get("data"))); break;
                    case "derive": { SecretKey key = (SecretKey)derive.invoke(aes, text(op.get("salt")), text(op.get("password"))); out = key == null ? null : key.getEncoded(); break; }
                    case "decode": out = AesUtil.base64Decode(text(op.get("data"))); break;
                    case "encode": out = AesUtil.base64Encode(bytes(op.get("data"))); break;
                    case "hex": out = AesUtil.hex(text(op.get("data"))); break;
                    case "final": {
                        byte[] keyBytes = bytes(op.get("key"));
                        SecretKey key = keyBytes == null ? null : new SecretKeySpec(keyBytes, "AES");
                        out = finish.invoke(aes, ((Number)op.get("mode")).intValue(), key, text(op.get("iv")), bytes(op.get("data"))); break;
                    }
                    case "raw": {
                        SecretKey key = (SecretKey)derive.invoke(aes, text(op.get("salt")), text(op.get("password")));
                        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
                        cipher.init(Cipher.ENCRYPT_MODE, key, new IvParameterSpec(AesUtil.hex(text(op.get("iv")))));
                        out = AesUtil.base64Encode(cipher.doFinal(bytes(op.get("data")))); break;
                    }
                    case "fail": out = fail.invoke(aes, new Exception("fixture")); break;
                    default: throw new IllegalArgumentException("Unknown fixture operation");
                    }
                    results.add(result(out));
                } catch (Exception e) {
                    Throwable cause = e instanceof InvocationTargetException ? e.getCause() : e;
                    JSONArray out = new JSONArray(); out.add("error"); out.add(cause.getClass().getName());
                    out.add(units(cause instanceof NullPointerException || cause.getMessage() == null ? "" : cause.getMessage()));
                    results.add(out);
                }
            }
            System.out.println(results.toJSONString());
        }
    }
}
