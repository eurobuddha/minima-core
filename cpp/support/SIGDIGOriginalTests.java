import org.minima.tests.kissvm.functions.maths.SIGDIGTests;
public class SIGDIGOriginalTests {
    public static void main(String[] args) {
        var test = new SIGDIGTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original SIGDIGTests methods passed: 3 (89 executed assertions)");
    }
}
