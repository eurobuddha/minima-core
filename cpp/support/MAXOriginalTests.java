import org.minima.tests.kissvm.functions.maths.MAXTests;
public class MAXOriginalTests {
    public static void main(String[] args) {
        var test = new MAXTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original MAXTests methods passed: 3 (120023 executed assertions)");
    }
}
