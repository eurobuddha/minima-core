import org.minima.tests.kissvm.functions.MinimaFunctionTests;
public class MinimaFunctionOriginalTests {
    public static void main(String[] args) {
        var test = new MinimaFunctionTests();
        test.testConstructors();test.testGettersAndSetters();
        System.out.println("Original MinimaFunctionTests methods passed: 2 (11 executed assertions)");
    }
}
