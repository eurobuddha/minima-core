import org.minima.tests.kissvm.functions.cast.SCRIPTTests;
public class SCRIPTCastOriginalTests {
    public static void main(String[] args) {
        var test=new SCRIPTTests();test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original SCRIPTTests methods passed: 3 (23 executed assertions)");
    }
}
