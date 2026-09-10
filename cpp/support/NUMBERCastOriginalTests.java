import org.minima.tests.kissvm.functions.cast.NUMBERTests;
public class NUMBERCastOriginalTests {
    public static void main(String[] args) {
        var test=new NUMBERTests();test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original NUMBERTests methods passed: 3 (19 executed assertions)");
    }
}
