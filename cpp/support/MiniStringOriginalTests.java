import org.minima.objects.base.MiniStringTests;
import org.junit.Test;
public class MiniStringOriginalTests {
    public static void main(String[] args) throws Exception {
        if(args.length>0){new MiniStringTests().testWriteDataStreamString();System.out.println("Unannotated helper passed: 1 method (1 successful-path assertion)");return;}
        // Reuse the repository's direct-call runner convention. Inspect @Test
        // markers so the unannotated helper cannot silently join the JUnit set.
        int annotated=0;for(var method:MiniStringTests.class.getDeclaredMethods())if(method.isAnnotationPresent(Test.class))annotated++;
        if(annotated!=2||!MiniStringTests.class.getMethod("testValueString").isAnnotationPresent(Test.class)||!MiniStringTests.class.getMethod("testReadFromStream").isAnnotationPresent(Test.class))throw new AssertionError("annotated method set changed");
        new MiniStringTests().testValueString();new MiniStringTests().testReadFromStream();
        System.out.println("Original MiniStringTests passed: 2 annotated methods (5 successful-path assertions)");
    }
}
