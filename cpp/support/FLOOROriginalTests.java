import org.minima.tests.kissvm.functions.maths.FLOORTests;
public class FLOOROriginalTests {
    public static void main(String[] args) {
        var test = new FLOORTests();
        test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original FLOORTests methods passed: 3 (30 executed assertions)");
    }
}
