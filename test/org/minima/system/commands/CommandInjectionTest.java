package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.system.Main;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

/**
 * Command injection through commands that build other commands (2026-11-21).
 *
 * Runs real commands through CommandRunner with only the in-memory transaction table - no node, no chain.
 * Uses only APIs the unfixed code also has, so the same file proves the defect on the old code (the injection
 * tests fail there) and the fix on the new.
 *
 * A JSON parameter decodes the escapes backslash-u003b to ';' and backslash-u0022 to a quote - that is how a
 * ';' or quote reaches a value (a raw ';' in a typed command is split off before any command sees it). Braces
 * are escaped too (u007b / u007d) because the command tokenizer counts them even inside JSON strings.
 */
public class CommandInjectionTest {

	@BeforeClass
	public static void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
	}

	static JSONObject run(String zCommand) {
		JSONArray res = CommandRunner.getRunner().runMultiCommand(zCommand);
		return (JSONObject) res.get(0);
	}

	static boolean txnExists(String zID) {
		return MinimaDB.getDB().getCustomTxnDB().getTransactionRow(zID) != null;
	}

	/** The state variables of the transaction a rawtxnfrom exported */
	static JSONArray exportedState(JSONObject zResult) {
		JSONObject txn = (JSONObject) ((JSONObject) zResult.get("response")).get("txn");
		return (JSONArray) ((JSONObject) txn.get("transaction")).get("state");
	}

	static String stateAt(JSONArray zState, int zPort) {
		for(Object obj : zState) {
			JSONObject sv = (JSONObject) obj;
			if(Integer.parseInt(sv.get("port").toString()) == zPort) {
				return sv.get("data").toString();
			}
		}
		return null;
	}

	@Test
	public void stateWithSemicolonRunsNoSecondCommand() {
		//sendfrom / rawfrom / rawtxnfrom paste a JSON state into "txnstate .. value:" - same line in all three
		JSONObject res = run("rawtxnfrom inputs:[] outputs:[] scripts:[] "
				+ "state:{\"1\":\"[a]\\u003btxncreate id:pwnedstate\\u003b[b]\"}");

		assertFalse("the ';' in the state ran 'txncreate id:pwnedstate'", txnExists("pwnedstate"));

		//The value is kept whole as a string state
		assertTrue((boolean) res.get("status"));
		assertEquals("[a];txncreate id:pwnedstate;[b]", stateAt(exportedState(res), 1));
	}

	@Test
	public void stateWithQuoteCannotMoveThePort() {
		//value:"[a]" port:"7" value:"[b]" - the quotes closed the value and replaced port:1 and the value
		JSONObject res = run("rawtxnfrom inputs:[] outputs:[] scripts:[] "
				+ "state:{\"1\":\"[a]\\u0022 port:\\u00227\\u0022 value:\\u0022[b]\"}");

		JSONArray state = exportedState(res);
		assertNull("a quote in the state moved it to port 7", stateAt(state, 7));
		assertEquals("[a]\" port:\"7\" value:\"[b]", stateAt(state, 1));
	}

	@Test
	public void ordinaryStateAndScriptUnchanged() {
		JSONObject res = run("rawtxnfrom inputs:[] outputs:[] scripts:[\"RETURN TRUE\"] "
				+ "state:{\"1\":\"0xABCD\",\"2\":\"[text with spaces]\",\"3\":\"42\"}");

		assertTrue((boolean) res.get("status"));
		JSONArray state = exportedState(res);
		assertEquals(3, state.size());
		assertEquals("0xABCD", stateAt(state, 1));
		assertEquals("[text with spaces]", stateAt(state, 2));
		assertEquals("42", stateAt(state, 3));

		JSONObject txn = (JSONObject) ((JSONObject) res.get("response")).get("txn");
		assertTrue(txn.get("witness").toString().contains("RETURN TRUE"));
	}

	/** 0x + 64 characters once decoded, so the old startsWith("0x") / length 66 check let it through */
	static String key66(String zDecoded, String zEncoded) {
		StringBuilder pad = new StringBuilder();
		for(int i=0;i<64-zDecoded.length();i++) {
			pad.append('0');
		}
		return "0x"+zEncoded+pad;
	}

	@Test
	public void multisigKeyWithSemicolonRefused() {
		String key = key66("00\"};txncreate id:pwnedkey;{\"", "00\\u0022\\u007d\\u003btxncreate id:pwnedkey\\u003b\\u007b\\u0022");
		JSONObject res = run("multisig action:create amount:1 required:1 publickeys:[\""+key+"\"]");

		assertFalse((boolean) res.get("status"));
		assertFalse(txnExists("pwnedkey"));
		assertTrue("not refused as a key: "+res, String.valueOf(res.get("error")).contains("Invalid public key"));
	}

	@Test
	public void multisigKeyWithQuoteRefused() {
		//"4":"0x00","2":"0","9":"000.." - a repeated "2" makes the coin spendable with 0 signatures
		String key = key66("00\",\"2\":\"0\",\"9\":\"", "00\\u0022,\\u00222\\u0022:\\u00220\\u0022,\\u00229\\u0022:\\u0022");
		JSONObject res = run("multisig action:create amount:1 required:1 publickeys:[\""+key+"\"]");

		assertFalse((boolean) res.get("status"));
		assertTrue("not refused as a key: "+res, String.valueOf(res.get("error")).contains("Invalid public key"));
	}

	@Test
	public void resetQuotedValueCannotReplaceThePhrase() throws Exception {
		//reset checks the keys are created - give it a Main that says so, without starting a node
		Field unsafeField = sun.misc.Unsafe.class.getDeclaredField("theUnsafe");
		unsafeField.setAccessible(true);
		Main main = (Main) ((sun.misc.Unsafe) unsafeField.get(null)).allocateInstance(Main.class);
		Field keys = Main.class.getDeclaredField("mInitKeysCreated");
		keys.setAccessible(true);
		keys.set(main, true);
		Field inst = Main.class.getDeclaredField("mMainInstance");
		inst.setAccessible(true);
		inst.set(null, main);

		try {
			JSONObject res = run("reset archivefile:/nonexistent-cmdinject/archive.gz action:seedsync "
					+ "phrase:\"real words\" keys:\"5 phrase:evil anyphrase:true\"");

			//The archive import it ran fails on the missing file - its reply shows the parameters it got
			JSONObject archive = (JSONObject) res.get("response");
			JSONObject params  = (JSONObject) archive.get("params");
			assertEquals("the phrase was replaced", "real words", params.get("phrase"));
			assertFalse("anyphrase was added", params.containsKey("anyphrase"));
		}finally {
			inst.set(null, null);
		}
	}

	@Test
	public void singleCommandRunsOnlyOne() {
		JSONObject res = CommandRunner.getRunner().runSingleCommand("txncreate id:singleone;txncreate id:singletwo");

		assertFalse("runSingleCommand ran the second command", txnExists("singletwo"));
		assertFalse((boolean) res.get("status"));

		//One command still runs as before
		assertTrue((boolean) CommandRunner.getRunner().runSingleCommand("txncreate id:singlethree").get("status"));
		assertTrue(txnExists("singlethree"));
	}
}
