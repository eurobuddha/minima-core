import org.minima.tests.kissvm.values.HexValueTests;

/** Execute the unchanged source assertions, including their expected exception types. */
public class HexValueOriginalTests {
    public static void main(String[] args) {
        new HexValueTests().testConstructors();
        new HexValueTests().testComaprisons();
        System.out.println("Original HexValueTests methods passed: 2 (35 executed assertions)");
    }
}
