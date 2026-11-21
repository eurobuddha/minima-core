package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;
import java.util.ArrayList;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.minidapps.MiniDAPP;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.system.Main;
import org.minima.system.mds.MDSManager;
import org.minima.utils.json.JSONObject;

/**
 * Keets line, MiniDapp System (third review MINOR 1): the uses of open commands that need WRITE rights
 * (txnpost burn:, keets file:, a file: outside the base folder, postfrom over an existing row) follow the MDS
 * model - a WRITE MiniDapp runs them, a READ MiniDapp gets a pending request, an unknown caller is refused.
 * No node runs: Main and MDSManager are allocated without constructors; the MiniDapp sits in the manager's
 * "untrusted" slot, which getMiniDAPP returns by identity.
 */
public class MdsGateTest {

	static sun.misc.Unsafe unsafe() throws Exception {
		Field f = sun.misc.Unsafe.class.getDeclaredField("theUnsafe");
		f.setAccessible(true);
		return (sun.misc.Unsafe) f.get(null);
	}
	
	static void set(Object zOn, Class<?> zClass, String zField, Object zValue) throws Exception {
		Field f = zClass.getDeclaredField(zField);
		f.setAccessible(true);
		f.set(zOn, zValue);
	}
	
	Field mainInstance;
	
	/** A MiniDapp with this permission, installed in a fake MDS; returns its id (the same String instance) */
	String install(String zPermission) throws Exception {
		String uid = new String("0xDAPP" + zPermission.toUpperCase());
		MDSManager mds = (MDSManager) unsafe().allocateInstance(MDSManager.class);
		JSONObject conf = new JSONObject();
		conf.put("name", "gate-test");
		conf.put("permission", zPermission);
		set(mds, MDSManager.class, "mUntrustedMiniUID", uid);
		set(mds, MDSManager.class, "mUntrustedMiniDAPP", new MiniDAPP(uid, conf));
		set(mds, MDSManager.class, "mPublicMiniUID", "0xPUBLIC");
		set(mds, MDSManager.class, "mPending", new ArrayList<>());
		//its message queue, so a log line's notify (posted to the MDS) has somewhere to go
		set(mds, org.minima.utils.messages.MessageStack.class, "mMessages", new java.util.LinkedList<>());
		set(mds, org.minima.utils.messages.MessageStack.class, "mLock", new Object());
		Main main = (Main) unsafe().allocateInstance(Main.class);
		set(main, Main.class, "mMDS", mds);
		mainInstance.set(null, main);
		return uid;
	}
	
	@Before
	public void setup() throws Exception {
		MinimaDB.createDB();
		set(MinimaDB.getDB(), MinimaDB.class, "mTxnDB", new TxnDB());
		mainInstance = Main.class.getDeclaredField("mMainInstance");
		mainInstance.setAccessible(true);
	}
	
	@After
	public void teardown() throws Exception {
		mainInstance.set(null, null);
	}
	
	static JSONObject run(String zUser, String zCommand) {
		return (JSONObject) CommandRunner.getRunner().runMultiCommand(zUser, zCommand).get(0);
	}
	
	static boolean pending(JSONObject zRes) {
		return Boolean.TRUE.equals(zRes.get("pending"));
	}
	
	static boolean refused(JSONObject zRes) {
		String error = String.valueOf(zRes.get("error"));
		return error.contains("ADMIN") || error.contains("only use files") || error.contains("already exists");
	}
	
	@Test
	public void readMiniDappGetsPending() throws Exception {
		String dapp = install("read");
		MinimaDB.getDB().getCustomTxnDB().createTransaction("burnrow");
		assertTrue(pending(run(dapp, "txnpost id:burnrow burn:1")));
		assertTrue(pending(run(dapp, "sendview file:/etc/hosts")));
		assertTrue(pending(run(dapp, "keets action:publish path:/x file:/etc/hosts")));
	}
	
	@Test
	public void writeMiniDappRuns() throws Exception {
		String dapp = install("write");
		MinimaDB.getDB().getCustomTxnDB().createTransaction("burnrow2");
		JSONObject res = run(dapp, "txnpost id:burnrow2 burn:1");
		assertFalse("a WRITE MiniDapp was refused or held: "+res, pending(res) || refused(res));
		res = run(dapp, "sendview file:/etc/hosts");
		assertFalse("a WRITE MiniDapp was refused or held: "+res, pending(res) || refused(res));
	}
	
	@Test
	public void unknownCallerStillRefused() throws Exception {
		//0xFF is no installed MiniDapp (no MDS here: on a node the MDS database answers "none")
		JSONObject res = run("0xFF", "txnpost id:x burn:1");
		assertFalse(pending(res));
		assertTrue(res.toString(), refused(res));
		assertEquals(Boolean.FALSE, res.get("status"));
	}
}
