import org.minima.tests.kissvm.values.ScriptValueTests;

/** Reuses the direct original-JUnit invocation approach of ParamConfigurerOriginalTests. */
public class ScriptValueOriginalTests {
    public static void main(String[] args) {
        new ScriptValueTests().testConstructors();
        System.out.println("Original ScriptValueTests methods passed: 1 (10 assertions)");
    }
}
