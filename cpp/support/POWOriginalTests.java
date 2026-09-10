import org.minima.tests.kissvm.functions.maths.POWTests;
public class POWOriginalTests {
    public static void main(String[] args) {
        var test = new POWTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original POWTests methods passed: 3 (25 executed assertions)");
    }
}
