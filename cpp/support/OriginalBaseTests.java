import java.io.*;
import org.junit.Test;
import java.util.*;
import org.minima.objects.*;
import org.minima.objects.base.*;
import org.minima.utils.json.JSONObject;

public class OriginalBaseTests {
    public static void main(String[] args)throws Exception {
        String name=args[0]; int assertions=switch(name) {
            case "MiniNumberTests"->41; case "MiniDataTests"->33;
            default->throw new IllegalArgumentException(name);
        };
        String[] methods=switch(name) {
            case "MiniNumberTests"->new String[]{"testValueZero","testValues","testWriteDataStream","testReadDataStream","testReadStreamTooLarge","testReadFromStream"};
            default->new String[]{"testValueBytes","testReadFromStreamData","testReadFromStreamDataTooBig","testWriteHashStreamData","testReadHashFromStream","testReadHashStreamDataLarge"};
        };
        Class<?> type=Class.forName("org.minima.objects.base."+name); var actual=new HashSet<String>();
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
        var report=new JSONObject(); report.put("class",name); report.put("tests",methods.length); report.put("assertions",assertions); report.put("passed",true);
        System.out.println("ORIGINAL_BASE_RESULT "+report);
    }
}
