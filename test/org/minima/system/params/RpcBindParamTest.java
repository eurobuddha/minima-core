package org.minima.system.params;

import static org.junit.Assert.assertEquals;

import java.util.HashMap;

import org.junit.After;
import org.junit.Test;

/** -rpcbind: default loopback, 'all' = every interface as before, or one address */
public class RpcBindParamTest {

	@After
	public void reset() {
		GeneralParams.resetDefaults();
	}
	
	static void configure(String... zArgs) {
		new ParamConfigurer().usingConfFile(zArgs).usingEnvVariables(new HashMap<>()).usingProgramArgs(zArgs).configure();
	}
	
	@Test
	public void defaultIsLoopback() {
		GeneralParams.resetDefaults();
		configure();
		assertEquals("127.0.0.1", GeneralParams.RPC_BIND_HOST);
	}
	
	@Test
	public void allIsEveryInterface() {
		configure("-rpcbind", "all");
		assertEquals("", GeneralParams.RPC_BIND_HOST);
	}
	
	@Test
	public void oneAddress() {
		configure("-rpcbind", "192.168.1.20");
		assertEquals("192.168.1.20", GeneralParams.RPC_BIND_HOST);
	}
	
	@Test
	public void emptyIsTheDefaultNotEveryInterface() {
		java.util.Map<String, String> env = new HashMap<>();
		env.put("MINIMA_RPCBIND", "");
		new ParamConfigurer().usingConfFile(new String[0]).usingEnvVariables(env).usingProgramArgs(new String[0]).configure();
		assertEquals("127.0.0.1", GeneralParams.RPC_BIND_HOST);
		configure("-rpcbind", "");
		assertEquals("127.0.0.1", GeneralParams.RPC_BIND_HOST);
	}
}
