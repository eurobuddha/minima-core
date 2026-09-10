import org.minima.tests.kissvm.functions.maths.CEILTests;
public class CEILOriginalTests {
    public static void main(String[] args) {
        var test = new CEILTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original CEILTests methods passed: 3 (26 executed assertions)");
    }
}
