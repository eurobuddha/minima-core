import java.lang.reflect.InvocationTargetException;
import org.junit.Test;
import org.minima.system.params.*;
public class ParamConfigurerOriginalTests {
    public static void main(String[] args)throws Exception {
        int count=0;
        for(var method:ParamConfigurerTest.class.getDeclaredMethods())if(method.isAnnotationPresent(Test.class)) {
            GeneralParams.resetDefaults();var expected=method.getAnnotation(Test.class).expected();Throwable thrown=null;
            try {method.invoke(new ParamConfigurerTest());}catch(InvocationTargetException error){thrown=error.getCause();}
            if(expected==Test.None.class?thrown!=null:thrown==null||!expected.isInstance(thrown))throw new AssertionError(method.getName(),thrown);
            ++count;
        }
        System.out.println("Original ParamConfigurerTest methods passed: "+count);
    }
}
