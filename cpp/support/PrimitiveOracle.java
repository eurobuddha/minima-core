import java.io.*;
import java.util.HexFormat;
import org.minima.objects.base.*;
import org.minima.utils.Streamable;

// Compiled against the pinned Java source; used only during tests.
public class PrimitiveOracle {
    static String wire(Streamable object) throws Exception {
        var bytes = new ByteArrayOutputStream();
        object.writeDataStream(new DataOutputStream(bytes));
        return "0x" + HexFormat.of().withUpperCase().formatHex(bytes.toByteArray());
    }
    public static void main(String[] args) throws Exception {
        var input = new BufferedReader(new InputStreamReader(System.in));
        String line;
        while ((line = input.readLine()) != null) {
            try {
                String[] p = line.split(" ");
                String op = p[0], a = p[1];
                if (op.equals("byte")) {
                    var n = new MiniByte(Integer.parseInt(a));
                    System.out.println(n + "|" + n.getByteValue() + "|" + wire(n));
                } else if (op.equals("data")) {
                    var n = new MiniData(a);
                    System.out.println(n + "|" + wire(n));
                } else if (op.equals("less")) {
                    System.out.println(new MiniData(a).isLess(new MiniData(p[2])));
                } else if (op.equals("string")) {
                    System.out.println(wire(new MiniString(new MiniData(a).getBytes())));
                } else {
                    var n = new MiniNumber(a);
                    if(op.equals("long") || op.equals("int")) {System.out.println(op.equals("long")?n.getAsLong():n.getAsInt());continue;}
                    n = switch (op) {
                        case "number" -> n;
                        case "add" -> n.add(new MiniNumber(p[2]));
                        case "sub" -> n.sub(new MiniNumber(p[2]));
                        case "mul" -> n.mult(new MiniNumber(p[2]));
                        case "div" -> n.div(new MiniNumber(p[2]));
                        case "pow" -> n.pow(Integer.parseInt(p[2]));
                        case "sqrt" -> n.sqrt();
                        case "mod" -> n.modulo(new MiniNumber(p[2]));
                        case "floor" -> n.floor();
                        case "ceil" -> n.ceil();
                        case "sigdig" -> n.setSignificantDigits(Integer.parseInt(p[2]));
                        default -> throw new IllegalArgumentException(op);
                    };
                    System.out.println(n + "|" + wire(n));
                }
            } catch (Exception ex) { System.out.println("ERROR"); }
        }
    }
}
