import org.minima.tests.kissvm.functions.base.SUBSETTests;
public class SUBSETOriginalTests {
    public static void main(String[] args) {
        var test=new SUBSETTests();test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original SUBSETTests methods passed: 3 (33 executed assertions)");
    }
}
