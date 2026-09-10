import org.minima.tests.kissvm.values.NumberValueTests;
import org.minima.kissvm.values.NumberValue;
import org.minima.objects.base.MiniNumber;

/** Execute unchanged pinned JUnit methods, then eleven separately labelled probes. */
public class NumberValueOriginalTests {
    static void errorObservation(Runnable operation) {
        try { operation.run(); }
        catch (ArithmeticException e) { System.out.println(e.toString()); return; }
        throw new AssertionError("Expected supplemental arithmetic error");
    }
    public static void main(String[] args) {
        var test = new NumberValueTests();
        test.testConstructors();
        test.testArithmentic();
        test.testComaprisons();
        test.testToString();
        System.out.println("Original NumberValueTests methods passed: 4 (99 executed assertions)");
        for (String a : new String[]{"0", "1", "-1"})
            for (String b : new String[]{"0", "0.000"})
                errorObservation(() -> new NumberValue(a).div(new NumberValue(b)));
        errorObservation(() -> new MiniNumber(0).pow(-1));
        for(int exponent:new int[]{-1000000000,Integer.MIN_VALUE,1000000000,Integer.MAX_VALUE})
            errorObservation(() -> MiniNumber.TEN.pow(exponent));
    }
}
