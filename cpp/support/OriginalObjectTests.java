import java.io.*;
import org.junit.Test;
import java.util.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.utils.json.JSONObject;

public class OriginalObjectTests {
    public static void main(String[] args)throws Exception {
        String name=args[0]; int assertions=switch(name) {
            case "AddressTests"->10; case "CoinTests","TokenTests"->5;
            default->throw new IllegalArgumentException(name);
        };
        // Reuse MiniStringOriginalTests' annotation-checked direct runner.
        // Each source method still executes JUnit's real assertions on a fresh instance.
        String[] methods=switch(name) {
            case "AddressTests"->new String[]{"testAddress","testWriteAndReadDataStream","testMakeMinimaAddress"};
            case "CoinTests"->new String[]{"testCoin","testReadAndWriteDataStream","testStaticReadAndWriteDataStream"};
            default->new String[]{"testTokenProof","testStaticReadAndWriteDataStream"};
        };
        Class<?> type=Class.forName("org.minima.objects."+name); var actual=new HashSet<String>();
        for(var method:type.getDeclaredMethods()) {
            var annotation=method.getAnnotation(Test.class);
            if(annotation!=null) {
                if(annotation.expected()!=Test.None.class||annotation.timeout()!=0)throw new AssertionError("Unsupported test options");
                actual.add(method.getName());
            }
            if(method.isAnnotationPresent(org.junit.Before.class)||method.isAnnotationPresent(org.junit.After.class)||
               method.isAnnotationPresent(org.junit.BeforeClass.class)||method.isAnnotationPresent(org.junit.AfterClass.class))throw new AssertionError("Unexpected lifecycle method");
        }
        if(!actual.equals(new HashSet<>(Arrays.asList(methods))))throw new AssertionError("Annotated method set changed");
        for(String method:methods)type.getMethod(method).invoke(type.getConstructor().newInstance());
        var bytes=new ByteArrayOutputStream(); var output=new DataOutputStream(bytes); JSONObject decoded;
        switch(name) {
            case "AddressTests": {
                var value=new Address(new MiniData("0xfff0f0")); value.writeDataStream(output);
                decoded=Address.ReadFromStream(new DataInputStream(new ByteArrayInputStream(bytes.toByteArray()))).toJSON(); break;
            }
            case "CoinTests": {
                var value=new Coin(new MiniData("0xfff"),new MiniData("0xffffffff"),MiniNumber.TWELVE,new MiniData("123")); value.writeDataStream(output);
                decoded=Coin.ReadFromStream(new DataInputStream(new ByteArrayInputStream(bytes.toByteArray()))).toJSON(); break;
            }
            default: {
                var value=new Token(new MiniData("0xffff"),MiniNumber.TWELVE,MiniNumber.TWELVE,new MiniString("token-name"),new MiniString("FFFF")); value.writeDataStream(output);
                decoded=Token.ReadFromStream(new DataInputStream(new ByteArrayInputStream(bytes.toByteArray()))).toJSON(); break;
            }
        }
        var report=new JSONObject(); report.put("class",name); report.put("tests",methods.length); report.put("assertions",assertions); report.put("passed",true);
        report.put("wire",new MiniData(bytes.toByteArray()).to0xString()); report.put("decoded",decoded);
        System.out.println("ORIGINAL_OBJECT_RESULT "+report);
    }
}
