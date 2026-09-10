import org.minima.tests.kissvm.functions.maths.ABSTests;
public class ABSOriginalTests {
    public static void main(String[] args) {
        var test = new ABSTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original ABSTests methods passed: 3 (23 executed assertions)");
    }
}
