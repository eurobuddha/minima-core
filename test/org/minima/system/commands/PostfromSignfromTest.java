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
 * postfrom / signfrom add a whole transaction row from data:, and TxnDB.addCompleteTransaction replaces a row with the
 * same id. A non-ADMIN caller (a MiniDapp, 0xFF here) may only add a NEW row - never replace another caller's draft.
 */
public class PostfromSignfromTest {

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
	
	/** A TxnRow serialised as postfrom / signfrom take it in data: */
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
	public void aNonAdminPostfromUnderANewIdStillPasses() throws Exception {
		//a non-admin caller posting its own new row (a fresh id)
		JSONObject res = run("0xFF", "postfrom data:"+blob("dx0011223344556677")+" mine:false");
		assertFalse(String.valueOf(res.get("error")).contains("already exists"));
		assertFalse(adminRefusal(res));
	}
}
