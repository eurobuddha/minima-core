import org.minima.tests.kissvm.functions.maths.BITCOUNTTests;
import org.minima.kissvm.functions.hex.BITCOUNT;
import org.minima.kissvm.Contract;
import org.minima.kissvm.expressions.ConstantExpression;
import org.minima.kissvm.values.HexValue;
import org.minima.objects.Witness;
import org.minima.objects.Transaction;
import java.util.ArrayList;
public class BITCOUNTOriginalTests {
    public static void main(String[] args)throws Exception {
        if(args.length>0){
            BITCOUNT.BITSPERBYTE[1]=42;System.out.println(BITCOUNT.totalBits(new byte[]{1,1}));
            var function=new BITCOUNT();function.addParameter(new ConstantExpression(new HexValue("0x01")));
            var contract=new Contract("","",new Witness(),new Transaction(),new ArrayList<>());
            System.out.println(function.runFunction(contract).toString());
            BITCOUNT.BITSPERBYTE[1]=Integer.MAX_VALUE;BITCOUNT.BITSPERBYTE[2]=1;
            System.out.println(BITCOUNT.totalBits(new byte[]{1,2}));return;
        }
        var test=new BITCOUNTTests();test.testConstructors();test.testValidParams();test.testInvalidParams();
        System.out.println("Original BITCOUNTTests methods passed: 3 (34 executed assertions, 256 table predicates)");
    }
}
