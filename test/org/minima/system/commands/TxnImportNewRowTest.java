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
 * A non-admin caller's txnimport never replaces an existing row (by its id: or the id inside the data): the
 * in-progress transaction table is one shared table. A new id still works.
 */
public class TxnImportNewRowTest {

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

	static boolean row(String zID) {
		return MinimaDB.getDB().getCustomTxnDB().getTransactionRow(zID) != null;
	}

	static boolean stateKept(String zID) {
		return MinimaDB.getDB().getCustomTxnDB().getTransactionRow(zID).getTransaction().getStateValue(1) != null;
	}

	@Test
	public void nonAdminTxnimportNeverReplacesARow() throws Exception {
		CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:importvictim;txnstate id:importvictim port:1 value:42");

		//under its own id:, or the id inside the data
		for(String cmd : new String[] {"txnimport id:importvictim data:"+PostfromSignfromTest.blob("other"),
										"txnimport data:"+PostfromSignfromTest.blob("importvictim")}) {
			JSONObject res = run("0xFF", cmd);
			assertTrue(cmd+" was not refused: "+res, PostfromSignfromTest.adminRefusal(res));
			assertTrue(cmd+" replaced the row", stateKept("importvictim"));
		}

		//a new id passes (the companion flow); ADMIN may still replace a row
		assertTrue((boolean) run("0xFF", "txnimport id:importnew data:"+PostfromSignfromTest.blob("x")).get("status"));
		assertTrue((boolean) run("0x00", "txnimport id:importvictim data:"+PostfromSignfromTest.blob("x")).get("status"));
		assertFalse(stateKept("importvictim"));
	}
}
