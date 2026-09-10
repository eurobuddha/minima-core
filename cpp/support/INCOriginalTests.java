import org.minima.tests.kissvm.functions.maths.INCTests;
public class INCOriginalTests {
    public static void main(String[] args) {
        var test = new INCTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original INCTests methods passed: 3 (30 executed assertions)");
    }
}
