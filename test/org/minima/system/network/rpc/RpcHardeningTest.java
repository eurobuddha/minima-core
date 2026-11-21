package org.minima.system.network.rpc;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;
import java.net.ServerSocket;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.system.Main;
import org.minima.system.commands.CommandRunner;
import org.minima.system.network.NetworkManager;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

/**
 * RPC hardening (2026-11-21). No node is started: Main and NetworkManager are allocated without their
 * constructors, so the rpc command can stop / start only the RPC server, on a free loopback port.
 * Uses only APIs the unfixed code also has, so these tests prove the defects there.
 */
public class RpcHardeningTest {

	static sun.misc.Unsafe unsafe() throws Exception {
		Field f = sun.misc.Unsafe.class.getDeclaredField("theUnsafe");
		f.setAccessible(true);
		return (sun.misc.Unsafe) f.get(null);
	}

	static int freePort() throws Exception {
		try(ServerSocket ss = new ServerSocket(0)) {
			return ss.getLocalPort();
		}
	}

	NetworkManager network;
	Field mainInstance;

	@Before
	public void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());

		Main main = (Main) unsafe().allocateInstance(Main.class);
		network   = (NetworkManager) unsafe().allocateInstance(NetworkManager.class);
		Field net = Main.class.getDeclaredField("mNetwork");
		net.setAccessible(true);
		net.set(main, network);
		mainInstance = Main.class.getDeclaredField("mMainInstance");
		mainInstance.setAccessible(true);
		mainInstance.set(null, main);

		GeneralParams.RPC_PORT 			= freePort();
		GeneralParams.RPC_ENABLED 		= false;
		GeneralParams.RPC_AUTHENTICATE 	= false;
		GeneralParams.RPC_PASSWORD 		= "none";
	}

	@After
	public void teardown() throws Exception {
		network.stopRPC();
		mainInstance.set(null, null);
		GeneralParams.RPC_ENABLED 		= false;
		GeneralParams.RPC_AUTHENTICATE 	= false;
		GeneralParams.RPC_PASSWORD 		= "none";
	}

	static JSONObject run(String zCommand) {
		JSONArray res = CommandRunner.getRunner().runMultiCommand(zCommand);
		return (JSONObject) res.get(0);
	}

	@Test
	public void enableWithoutPasswordKeepsThePassword() {
		//As if started with -rpcenable -rpcpassword secret
		GeneralParams.RPC_ENABLED 		= true;
		GeneralParams.RPC_AUTHENTICATE 	= true;
		GeneralParams.RPC_PASSWORD 		= "secret";

		run("rpc enable:false");
		assertTrue("rpc enable:false switched authentication off", GeneralParams.RPC_AUTHENTICATE);

		JSONObject res = run("rpc enable:true");
		assertTrue((boolean) res.get("status"));
		assertTrue("rpc enable:true switched authentication off", GeneralParams.RPC_AUTHENTICATE);
		assertEquals("secret", GeneralParams.RPC_PASSWORD);
	}

	@Test
	public void firstEnableNeedsAPassword() {
		JSONObject res = run("rpc enable:true");
		assertFalse("RPC was enabled with no password", (boolean) res.get("status"));
		assertFalse(GeneralParams.RPC_ENABLED);

		res = run("rpc enable:true password:first");
		assertTrue((boolean) res.get("status"));
		assertTrue(GeneralParams.RPC_AUTHENTICATE);
		assertEquals("first", GeneralParams.RPC_PASSWORD);
	}
}
