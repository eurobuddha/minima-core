package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.minidapps.MiniDAPP;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.database.userprefs.txndb.TxnRow;
import org.minima.objects.Transaction;
import org.minima.objects.Witness;
import org.minima.objects.base.MiniData;
import org.minima.system.Main;
import org.minima.system.mds.MDSManager;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.JSONObject;

/**
 * Keets line, MiniDapp System: the uses of open commands that need WRITE rights (txnpost burn:, keets file:, a
 * file: outside the base folder, postfrom over an existing row, overwriting a file) follow the MDS model - a WRITE
 * MiniDapp runs them, a READ MiniDapp gets a pending request, an unknown caller is refused - and a WRITE MiniDapp
 * counts as write-allowed on every command, so its file: means the same everywhere. No node runs: Main and the
 * MiniDapp System are allocated without constructors; FakeMDS answers getMiniDAPP from a map.
 */
public class MdsGateTest {

	/** The MiniDapp System with its installed dapps in a map (no database) */
	public static class FakeMDS extends MDSManager {
		Map<String, MiniDAPP> mDapps;
		@Override
		public MiniDAPP getMiniDAPP(String zMiniDAPPID) {
			return mDapps.get(zMiniDAPPID);
		}
	}

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
	FakeMDS mds;
	String oldbase;

	@Before
	public void setup() throws Exception {
		MinimaDB.createDB();
		set(MinimaDB.getDB(), MinimaDB.class, "mTxnDB", new TxnDB());
		mds = (FakeMDS) unsafe().allocateInstance(FakeMDS.class);
		mds.mDapps = new HashMap<>();
		set(mds, MDSManager.class, "mPublicMiniUID", "0xPUBLIC");
		set(mds, MDSManager.class, "mPending", new ArrayList<>());
		//its message queue, so a log line's notify (posted to the MDS) has somewhere to go
		set(mds, org.minima.utils.messages.MessageStack.class, "mMessages", new java.util.LinkedList<>());
		set(mds, org.minima.utils.messages.MessageStack.class, "mLock", new Object());
		Main main = (Main) unsafe().allocateInstance(Main.class);
		set(main, Main.class, "mMDS", mds);
		mainInstance = Main.class.getDeclaredField("mMainInstance");
		mainInstance.setAccessible(true);
		mainInstance.set(null, main);
		oldbase = GeneralParams.BASE_FILE_FOLDER;
	}

	@After
	public void teardown() throws Exception {
		mainInstance.set(null, null);
		GeneralParams.BASE_FILE_FOLDER = oldbase;
	}

	/** A MiniDapp with this permission, installed; returns its id */
	String install(String zPermission) {
		String uid = "0xDAPP" + zPermission.toUpperCase();
		JSONObject conf = new JSONObject();
		conf.put("name", "gate-test");
		conf.put("permission", zPermission);
		mds.mDapps.put(uid, new MiniDAPP(uid, conf));
		return uid;
	}

	static JSONObject run(String zUser, String zCommand) {
		return (JSONObject) CommandRunner.getRunner().runMultiCommand(zUser, zCommand).get(0);
	}

	static boolean pending(JSONObject zRes) {
		return Boolean.TRUE.equals(zRes.get("pending"));
	}

	static boolean refused(JSONObject zRes) {
		String error = String.valueOf(zRes.get("error"));
		return error.contains("ADMIN") || error.contains("only use files") || error.contains("already exists")
				|| error.contains("only creates new files");
	}

	static String blob(String zID) throws Exception {
		TxnRow row = new TxnRow(zID, new Transaction(), new Witness());
		java.io.ByteArrayOutputStream bo = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream d = new java.io.DataOutputStream(bo);
		row.writeDataStream(d);
		d.flush();
		return new MiniData(bo.toByteArray()).to0xString();
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
	public void readMiniDappPostfromOverAnExistingRowGoesPending() throws Exception {
		String dapp = install("read");
		MinimaDB.getDB().getCustomTxnDB().createTransaction("theirdraft");
		JSONObject res = run(dapp, "postfrom data:"+blob("theirdraft")+" mine:false");
		assertTrue(res.toString(), pending(res));
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
	public void writeMiniDappFileMeansTheSameOnEveryCommand() throws Exception {
		//no base folder: a plain name on an allowed command must not be treated as a non-admin's
		GeneralParams.BASE_FILE_FOLDER = "";
		String dapp = install("write");
		JSONObject res = run(dapp, "sendview file:gate-missing.txn");
		assertFalse("a WRITE MiniDapp's plain name was refused while an absolute path runs: "+res, refused(res));
	}

	@Test
	public void readMiniDappOnAnAllowedCommandStaysNonAdmin() throws Exception {
		GeneralParams.BASE_FILE_FOLDER = "";
		String dapp = install("read");
		JSONObject res = run(dapp, "sendview file:gate-missing.txn");
		assertFalse(pending(res));
		assertTrue(res.toString(), String.valueOf(res.get("error")).contains("only use files"));
	}

	@Test
	public void unknownCallerRefusedWithAMiniDappSystem() throws Exception {
		install("write");
		JSONObject res = run("0xFF", "txnpost id:x burn:1");
		assertFalse(pending(res));
		assertTrue(res.toString(), refused(res));
		assertEquals(Boolean.FALSE, res.get("status"));
	}
}
