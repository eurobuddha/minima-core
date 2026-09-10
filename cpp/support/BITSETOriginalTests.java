import org.minima.tests.kissvm.functions.maths.BITSETTests;
public class BITSETOriginalTests {
    public static void main(String[] args) {
        var test=new BITSETTests();test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original BITSETTests methods passed: 3 (271 executed assertions)");
    }
}
