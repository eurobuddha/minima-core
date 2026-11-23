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
 * The in-progress transaction table is ONE shared table with no owner (2026-11-23). A non-admin caller that is
 * not an installed MiniDapp (a read-mode RPC user, a companion app, a gateway - 0xFF here) may not list, export
 * or edit its rows, and its txnimport never replaces an existing row - it could rewrite or swap another caller's
 * draft between that caller's import and its txnsign. The companion relay (txnimport of a new id, txnbasics,
 * txnpost) stays open.
 */
public class TxnTableGateTest {

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
		for(String cmd : new String[] {"txnimport id:importvictim data:"+WriteGateTest.blob("other"),
										"txnimport data:"+WriteGateTest.blob("importvictim")}) {
			JSONObject res = run("0xFF", cmd);
			assertTrue(cmd+" was not refused: "+res, WriteGateTest.adminRefusal(res));
			assertTrue(cmd+" replaced the row", stateKept("importvictim"));
		}

		//a new id passes (the companion flow); ADMIN may still replace a row
		assertTrue((boolean) run("0xFF", "txnimport id:importnew data:"+WriteGateTest.blob("x")).get("status"));
		assertTrue((boolean) run("0x00", "txnimport id:importvictim data:"+WriteGateTest.blob("x")).get("status"));
		assertFalse(stateKept("importvictim"));
	}

	@Test
	public void nonAdminCannotListOrEditTheTable() throws Exception {
		CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:tabledraft;txnstate id:tabledraft port:1 value:42");

		for(String cmd : new String[] {"txnlist", "txnlist id:tabledraft", "txnexport id:tabledraft",
				"txninput id:tabledraft coinid:0x01", "txnoutput id:tabledraft amount:1 address:0x01",
				"txnstate id:tabledraft port:1 value:1", "txnclear id:tabledraft", "txndelete id:tabledraft",
				"txncreate id:tablenew"}) {
			JSONObject res = run("0xFF", cmd);
			assertTrue(cmd+" ran for a non-admin caller: "+res, WriteGateTest.adminRefusal(res));
		}
		assertTrue(row("tabledraft") && stateKept("tabledraft"));
		assertFalse(row("tablenew"));

		//the companion relay stays open (txnbasics / txnpost fail later here: no chain)
		assertTrue((boolean) run("0xFF", "txnimport id:tablerelay data:"+WriteGateTest.blob("x")).get("status"));
		assertFalse(WriteGateTest.adminRefusal(run("0xFF", "txnbasics id:tablerelay")));
		assertFalse(WriteGateTest.adminRefusal(run("0xFF", "txnpost id:tablerelay")));

		//ADMIN as before
		assertTrue((boolean) run("0x00", "txnlist id:tabledraft").get("status"));
		assertTrue((boolean) run("0x00", "txndelete id:tabledraft").get("status"));
	}
}
