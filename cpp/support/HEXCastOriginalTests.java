import org.minima.tests.kissvm.functions.cast.HEXTests;
public class HEXCastOriginalTests {
    public static void main(String[] args) {
        var test=new HEXTests();test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original HEXTests methods passed: 3 (24 executed assertions)");
    }
}
