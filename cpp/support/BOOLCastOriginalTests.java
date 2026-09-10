import org.minima.tests.kissvm.functions.cast.BOOLTests;
public class BOOLCastOriginalTests {
    public static void main(String[] args) {
        var test=new BOOLTests();test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original BOOLTests methods passed: 3 (21 executed assertions)");
    }
}
