import junit.framework.*;
import org.minima.utils.json.JSONObject;
public class OriginalP2PTests {
 public static void main(String[] args)throws Exception{
  String name=args[0];int assertions=switch(name){case "SwapLinksFunctionsTest"->10;case "WalkLinksFuncsTest"->3;case "UtilFuncsTest"->2;default->throw new IllegalArgumentException(name);};
  var suite=new TestSuite(Class.forName("org.minima.system.network.p2p."+name));var result=new TestResult();suite.run(result);
  if(!result.wasSuccessful()){var errors=result.errors();while(errors.hasMoreElements())System.err.println(errors.nextElement());var failures=result.failures();while(failures.hasMoreElements())System.err.println(failures.nextElement());throw new AssertionError(name+" failed");}
  var out=new JSONObject();out.put("class",name);out.put("tests",result.runCount());out.put("assertions",assertions);out.put("passed",true);System.out.println(out.toJSONString());
 }
}
