package org.minima.system.commands;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.utils.json.JSONObject;

/**
 * The non-admin gate (2026-11-21): commands a non-admin caller (0xFF - a non-admin companion, a read-mode RPC
 * user) must not run, because they act with the node's own keys or files.
 */
public class WriteGateTest {

	@BeforeClass
	public static void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
	}
	
	static JSONObject run(String zUser, String zCommand) {
		return (JSONObject) CommandRunner.getRunner().runMultiCommand(zUser, zCommand).get(0);
	}
	
	static boolean adminRefusal(JSONObject zRes) {
		return String.valueOf(zRes.get("error")).contains("ADMIN");
	}
	
	@Test
	public void txnautoNeedsAdmin() {
		//sign:true signs with the node wallet - with txnpost (open) that spends the node's coins
		JSONObject res = run("0xFF", "txnauto id:gatetxn amount:1 address:0x00 sign:true");
		assertTrue("a non-admin caller ran txnauto: "+res, adminRefusal(res));
		assertFalse(MinimaDB.getDB().getCustomTxnDB().getTransactionRow("gatetxn") != null);
		
		//ADMIN still runs it (it fails later here: no wallet in the test)
		assertFalse(adminRefusal(run("0x00", "txnauto id:gatetxn2 amount:1 address:0x00")));
	}
	
	@Test
	public void keetsFileNeedsAdmin() {
		boolean keets = org.minima.system.params.GeneralParams.KEETS_ENABLED;
		org.minima.system.params.GeneralParams.KEETS_ENABLED = true;
		try {
			keetsFileGate();
		}finally {
			org.minima.system.params.GeneralParams.KEETS_ENABLED = keets;
		}
	}
	
	void keetsFileGate() {
		JSONObject res = run("0xFF", "keets action:publish path:/x file:/etc/hosts");
		assertTrue("a non-admin caller used keets file: "+res, String.valueOf(res.get("error")).contains("file: needs ADMIN"));
		res = run("0xFF", "keets action:fetch url:x file:/tmp/keets-gate-test");
		assertTrue(String.valueOf(res.get("error")).contains("file: needs ADMIN"));
		
		//ADMIN passes the gate (it fails later here: no keets sidecar in the test)
		assertFalse(String.valueOf(run("0x00", "keets action:publish path:/x file:/etc/hosts").get("error")).contains("file: needs ADMIN"));
	}
	
	@Test
	public void txnpostBurnNeedsAdmin() {
		CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:burntxn");
		JSONObject res = run("0xFF", "txnpost id:burntxn burn:1");
		assertTrue("a non-admin caller asked the node to burn its own coins: "+res,
				String.valueOf(res.get("error")).contains("burn: needs ADMIN"));
		
		//without burn, and for ADMIN, the gate does not stand in the way (txnpost fails later here: no chain)
		assertFalse(String.valueOf(run("0xFF", "txnpost id:burntxn").get("error")).contains("needs ADMIN"));
		assertFalse(String.valueOf(run("0x00", "txnpost id:burntxn burn:1").get("error")).contains("needs ADMIN"));
	}
	
	/** A TxnRow as the Trading Account (VirtualWallet.hex) serialises it for postfrom data: */
	static String blob(String zID) throws Exception {
		org.minima.database.userprefs.txndb.TxnRow row = new org.minima.database.userprefs.txndb.TxnRow(
				zID, new org.minima.objects.Transaction(), new org.minima.objects.Witness());
		java.io.ByteArrayOutputStream bo = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream d = new java.io.DataOutputStream(bo);
		row.writeDataStream(d);
		d.flush();
		return new org.minima.objects.base.MiniData(bo.toByteArray()).to0xString();
	}
	
	@Test
	public void postfromCannotOverwriteAnotherDraft() throws Exception {
		CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:victimdraft");
		CommandRunner.getRunner().runMultiCommand("0x00", "txnstate id:victimdraft port:1 value:42");
		
		for(String cmd : new String[] {"postfrom", "signfrom"}) {
			String extra = cmd.equals("signfrom") ? " privatekey:0x01 keyuses:1" : " mine:false";
			JSONObject res = run("0xFF", cmd+" data:"+blob("victimdraft")+extra);
			assertTrue(cmd+" overwrote another draft: "+res, String.valueOf(res.get("error")).contains("already exists"));
			org.minima.database.userprefs.txndb.TxnRow row = MinimaDB.getDB().getCustomTxnDB().getTransactionRow("victimdraft");
			assertTrue(cmd+" removed the victim's draft", row != null);
			assertTrue(cmd+" replaced the victim's draft", row.getTransaction().getStateValue(1) != null);
		}
	}
	
	@Test
	public void tradingAccountPostfromStillPasses() throws Exception {
		//VirtualWallet.post: postfrom data:<fresh id> mine:false, as a non-admin companion
		JSONObject res = run("0xFF", "postfrom data:"+blob("dx0011223344556677")+" mine:false");
		assertFalse(String.valueOf(res.get("error")).contains("already exists"));
		assertFalse(adminRefusal(res));
	}
}
