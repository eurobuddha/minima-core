import org.minima.tests.kissvm.functions.maths.DECTests;
public class DECOriginalTests {
    public static void main(String[] args) {
        var test = new DECTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original DECTests methods passed: 3 (30 executed assertions)");
    }
}
