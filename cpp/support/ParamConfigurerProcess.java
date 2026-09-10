import java.util.HashMap;
import org.minima.system.params.ParamConfigurer;
public class ParamConfigurerProcess {
    public static void main(String[] args){if(args.length>0&&args[0].equals("__test_version")){org.minima.system.params.GlobalParams.MINIMA_VERSION="before";System.out.println(org.minima.system.params.TestParams.MINIMA_VERSION);org.minima.system.params.GlobalParams.MINIMA_VERSION="after";org.minima.system.params.TestParams.setTestParams();System.out.println(org.minima.system.params.GlobalParams.MINIMA_VERSION);return;}new ParamConfigurer().usingConfFile(args).usingEnvVariables(new HashMap<>()).usingProgramArgs(args).configure();}
}
