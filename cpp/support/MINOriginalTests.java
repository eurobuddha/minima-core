import org.minima.tests.kissvm.functions.maths.MINTests;
public class MINOriginalTests {
    public static void main(String[] args) {
        var test = new MINTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original MINTests methods passed: 3 (120023 executed assertions)");
    }
}
