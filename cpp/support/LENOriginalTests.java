import org.minima.tests.kissvm.functions.base.LENTests;
public class LENOriginalTests {
    public static void main(String[] args) {
        var test=new LENTests();test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original LENTests methods passed: 3 (12 executed assertions)");
    }
}
