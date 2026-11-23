package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;
import java.util.Arrays;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

/**
 * A ';' inside a quoted or JSON value is data, never a command separator (2026-11-23).
 *
 * The old runMultiCommand split the whole line at every ';', so an app that put chain data into
 * runscript script:"..", newscript script:"..", txnstate value:".." or a JSON parameter let that data start a
 * command of its own - run with the app's rights (found in the PandaPools pool discovery: a stranger's coin
 * state reached runscript script:"..[x;send ..;].." and the node ran the send).
 */
public class CommandSplitTest {

	@BeforeClass
	public static void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
	}

	static boolean txnExists(String zID) {
		return MinimaDB.getDB().getCustomTxnDB().getTransactionRow(zID) != null;
	}

	static void assertSplit(String zLine, String... zCommands) {
		assertEquals(zLine, Arrays.asList(zCommands), CommandRunner.splitCommands(zLine));
	}

	@Test
	public void splitterEdgeCases() {
		assertSplit("a;b;c", "a", "b", "c");
		assertSplit(" a ;; b ; ", "a", "b");
		assertSplit("");
		assertSplit("runscript script:\"LET x=[a;b] RETURN TRUE\";status",
				"runscript script:\"LET x=[a;b] RETURN TRUE\"", "status");
		//a bracket inside a quoted value is text - it does not hold the next ';'
		assertSplit("x v:\"[\";y", "x v:\"[\"", "y");
		//JSON: its strings may hold ';' or a bracket, an escaped quote does not end them
		assertSplit("x p:{\"a\":\"1;2\",\"b\":{\"c\":\"]\"}};y", "x p:{\"a\":\"1;2\",\"b\":{\"c\":\"]\"}}", "y");
		assertSplit("x p:{\"a\":\"say \\\";\\\" ok\"};y", "x p:{\"a\":\"say \\\";\\\" ok\"}", "y");
		assertSplit("x p:[1,[2;3]];y", "x p:[1,[2;3]]", "y");
		//a backslash escapes the next character in a quoted value: \" stays inside, \\ is one backslash
		assertSplit("x v:\"a\\\";b\";y", "x v:\"a\\\";b\"", "y");
		assertSplit("x v:\"a\\\\\";y", "x v:\"a\\\\\"", "y");
		//outside a value a backslash and a stray closing bracket are plain text
		assertSplit("x file:C:\\dir\\f;y", "x file:C:\\dir\\f", "y");
		assertSplit("x v:a];y", "x v:a]", "y");
	}

	@Test
	public void unclosedQuoteOrBracketIsRefused() {
		assertNull(CommandRunner.splitCommands("a;b v:\"open"));
		assertNull(CommandRunner.splitCommands("a;b v:\"ends with an escaped quote\\\""));
		assertNull(CommandRunner.splitCommands("a;b v:\"ends with a backslash\\"));
		assertNull(CommandRunner.splitCommands("a;b p:{\"a\":1"));
		assertNull(CommandRunner.splitCommands("a;b p:[1,2"));
		assertNull(CommandRunner.splitCommands("a;b p:{\"a\":\"open}"));
	}

	@Test
	public void unclosedQuoteRunsNothing() {
		//The old code ran the txncreate, then the rest - with the open quote swallowing nothing it could see
		JSONArray res = CommandRunner.getRunner().runMultiCommand(
				"txncreate id:unclosed;txnstate id:unclosed port:1 value:\"abc");
		assertEquals(1, res.size());
		JSONObject result = (JSONObject) res.get(0);
		assertFalse((boolean) result.get("status"));
		assertTrue(String.valueOf(result.get("error")).contains("nothing was run"));
		assertFalse("a piece before the open quote ran", txnExists("unclosed"));
	}

	@Test
	public void pandaPoolsScriptRunsOneRunscriptAndNoSend() throws Exception {
		Command real = CommandInjectionTest.swap("send", new CommandInjectionTest.Recorder(
				CommandRunner.getRunner().getCommandOnly("send")));
		try {
			//The covenant with a stranger's state pasted into a KISS string literal
			JSONArray res = CommandRunner.getRunner().runMultiCommand(
					"runscript script:\"LET k=[x;send address:0xFF amount:1;] RETURN SIGNEDBY(0x01)\"");
			assertEquals(1, res.size());
			assertEquals("runscript", ((JSONObject) res.get(0)).get("command"));
			assertNull("the ';' in the script ran a send", CommandInjectionTest.Recorder.LAST.get("send"));

			//The same state with a quote, escaped by the app as \" (PandaPools' scriptArg)
			res = CommandRunner.getRunner().runMultiCommand(
					"runscript script:\"LET k=[x\\\";send address:0xFF amount:1;\\\"] RETURN TRUE\"");
			assertEquals(1, res.size());
			assertNull("the escaped quote let a send out", CommandInjectionTest.Recorder.LAST.get("send"));
		}finally {
			CommandInjectionTest.swap("send", real);
		}
	}

	@Test
	public void semicolonInJsonOrStateValueIsData() {
		JSONArray res = CommandRunner.getRunner().runMultiCommand("txncreate id:jsonsemi;"
				+ "txnstate id:jsonsemi port:1 value:\"[a;txncreate id:pwnedsemi;b]\"");
		assertEquals(2, res.size());
		assertTrue((boolean) ((JSONObject) res.get(1)).get("status"));
		assertFalse("the ';' in the state value ran a txncreate", txnExists("pwnedsemi"));
		assertEquals("[a;txncreate id:pwnedsemi;b]", MinimaDB.getDB().getCustomTxnDB().getTransactionRow("jsonsemi")
				.getTransaction().getStateValue(1).getData().toString());
		CommandRunner.getRunner().runMultiCommand("txndelete id:jsonsemi");

		res = CommandRunner.getRunner().runMultiCommand("rawtxnfrom inputs:[] outputs:[] scripts:[] "
				+ "state:{\"1\":\"[a];txncreate id:pwnedjson;[b]\"}");
		assertEquals(1, res.size());
		assertFalse("the ';' in the JSON state ran a txncreate", txnExists("pwnedjson"));
	}

	@Test
	public void plainChainsRunAsBefore() {
		JSONArray res = CommandRunner.getRunner().runMultiCommand(
				"txncreate id:plainchain;txnstate id:plainchain port:1 value:7;txndelete id:plainchain;");
		assertEquals(3, res.size());
		for(Object obj : res) {
			assertTrue(obj.toString(), (boolean) ((JSONObject) obj).get("status"));
		}
		assertFalse(txnExists("plainchain"));

		//still stops at the first failure
		res = CommandRunner.getRunner().runMultiCommand("txnstate id:nosuchrow port:1 value:1;txncreate id:afterfail");
		assertEquals(1, res.size());
		assertFalse(txnExists("afterfail"));
	}

	@Test
	public void nonAdminRightsAreCheckedPerPiece() throws Exception {
		Command real = CommandInjectionTest.swap("send", new CommandInjectionTest.Recorder(
				CommandRunner.getRunner().getCommandOnly("send")));
		try {
			//a real second command is still gated on its own
			JSONArray res = CommandRunner.getRunner().runMultiCommand("0xFF", "hash data:0x01;send address:0xFF amount:1");
			assertEquals(2, res.size());
			assertFalse((boolean) ((JSONObject) res.get(1)).get("status"));
			assertTrue(String.valueOf(((JSONObject) res.get(1)).get("error")).contains("ADMIN"));
			assertNull(CommandInjectionTest.Recorder.LAST.get("send"));

			//a ';' inside the quoted value starts nothing
			res = CommandRunner.getRunner().runMultiCommand("0xFF", "hash data:\"a;send address:0xFF amount:1\"");
			assertEquals(1, res.size());
			assertEquals("hash", ((JSONObject) res.get(0)).get("command"));
			assertNull(CommandInjectionTest.Recorder.LAST.get("send"));
		}finally {
			CommandInjectionTest.swap("send", real);
		}
	}
}
