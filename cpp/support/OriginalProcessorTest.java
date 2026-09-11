import org.minima.system.brains.TxPoWProcessorTest;
import org.junit.Test;
import java.util.*;
public class OriginalProcessorTest {
    public static void main(String[] args)throws Exception {
        var type=TxPoWProcessorTest.class;
        var names=new HashSet<String>();
        for(var method:type.getDeclaredMethods()) {
            var annotation=method.getAnnotation(Test.class);
            if(annotation!=null) {
                if(annotation.expected()!=Test.None.class||annotation.timeout()!=0)throw new AssertionError("Unexpected options");
                names.add(method.getName());
            }
            if(method.isAnnotationPresent(org.junit.Before.class)||method.isAnnotationPresent(org.junit.After.class)||method.isAnnotationPresent(org.junit.BeforeClass.class)||method.isAnnotationPresent(org.junit.AfterClass.class))throw new AssertionError("Unexpected lifecycle method");
        }
        if(!names.equals(Set.of("testTxPoWProcessor","testTxPoWProcessor2")))throw new AssertionError("Original method set changed");
        for(String name:names)type.getMethod(name).invoke(type.getConstructor().newInstance());
        System.out.println("original_methods=2;original_assertions=0");
        long start=System.nanoTime();TxPoWProcessorTest.delay();
        if(System.nanoTime()-start<90000000L)throw new AssertionError("Default delay returned too early");
        System.out.println("default=100ms");
        for(long value:new long[]{0,1,-1,-100,Long.MIN_VALUE}) {
            String result="returned";
            try {TxPoWProcessorTest.delay(value);}catch(IllegalArgumentException e){result=e.getClass().getName()+": "+e.getMessage();}
            System.out.println("delay("+value+")="+result);
        }
        for(long value:new long[]{0,1,Long.MAX_VALUE,-1}) {
            Thread.currentThread().interrupt();
            String result="returned";
            try {TxPoWProcessorTest.delay(value);}catch(IllegalArgumentException e){result=e.getClass().getName()+": "+e.getMessage();}
            System.out.println("interrupted("+value+")="+result+";status="+Thread.currentThread().isInterrupted());
            Thread.interrupted();
        }
    }
}
