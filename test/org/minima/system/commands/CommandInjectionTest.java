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
	public void multisigKeyWithSemicolonRefused() throws Exception {
		String key = key66("00\"};txncreate id:pwnedkey;{\"", "00\\u0022\\u007d\\u003btxncreate id:pwnedkey\\u003b\\u007b\\u0022");
		Command real = swap("send", new Recorder(CommandRunner.getRunner().getCommandOnly("send")));
		try {
			JSONObject res = run("multisig action:create amount:1 required:1 publickeys:[\""+key+"\"]");
			
			//The old code ran the send (here a stand-in that succeeds), then the txncreate after the ';'
			assertFalse("the ';' in a public key ran 'txncreate id:pwnedkey'", txnExists("pwnedkey"));
			assertFalse((boolean) res.get("status"));
			assertTrue("not refused as a key: "+res, String.valueOf(res.get("error")).contains("Invalid public key"));
			assertNull("the send ran", Recorder.LAST.get("send"));
		}finally {
			swap("send", real);
		}
	}

	@Test
	public void multisigKeyWithQuoteRefused() throws Exception {
		//"4":"0x00","2":"0","9":"000.." - a repeated "2" makes the coin spendable with 0 signatures
		String key = key66("00\",\"2\":\"0\",\"9\":\"", "00\\u0022,\\u00222\\u0022:\\u00220\\u0022,\\u00229\\u0022:\\u0022");
		Command real = swap("send", new Recorder(CommandRunner.getRunner().getCommandOnly("send")));
		try {
			JSONObject res = run("multisig action:create amount:1 required:1 publickeys:[\""+key+"\"]");
			
			JSONObject sent = Recorder.LAST.get("send");
			if(sent != null) {
				JSONObject state = (JSONObject) sent.get("state");
				assertFalse("a quote in a public key set required to "+state.get("2"), "0".equals(String.valueOf(state.get("2"))));
			}
			assertFalse((boolean) res.get("status"));
			assertTrue("not refused as a key: "+res, String.valueOf(res.get("error")).contains("Invalid public key"));
		}finally {
			swap("send", real);
		}
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
	public void singleCommandStillRunsChainsAsBefore() {
		//Callers send chains through runSingleCommand on purpose (the casino engine's
		//"txnbasics id:X;txnpost id:X" via the Parlons AdminRpc, the Android terminal): every piece runs,
		//the first piece's reply comes back - exactly as on the unfixed code.
		JSONObject res = CommandRunner.getRunner().runSingleCommand("txncreate id:chainone;txndelete id:chainone");
		
		assertEquals("txncreate", res.get("command"));
		assertTrue((boolean) res.get("status"));
		assertFalse("the second piece did not run", txnExists("chainone"));
	}
	
	static int txnRows() {
		return MinimaDB.getDB().getCustomTxnDB().listTxns().size();
	}
	
	@Test
	public void failedBuildLeavesNoTxnRow() {
		//A state that is not a valid state variable throws inside rawtxnfrom after its txncreate
		int before = txnRows();
		JSONObject res = run("rawtxnfrom inputs:[] outputs:[] scripts:[] state:{\"1\":\"[a]\\u0022 x\"}");
		assertFalse((boolean) res.get("status"));
		assertEquals("rawtxnfrom left its txn row behind", before, txnRows());
	}
	
	@Test
	public void failedMultisigChainLeavesNoTxnRow() throws Exception {
		//A transaction file multisig post can import, whose txnpost then fails (no node in the test)
		java.io.File file = java.io.File.createTempFile("multisig-post", ".txn");
		try {
			run("txncreate id:postfile");
			assertTrue((boolean) run("txnexport id:postfile file:"+file.getAbsolutePath()).get("status"));
			run("txndelete id:postfile");
			
			int before = txnRows();
			JSONObject res = run("multisig action:post file:"+file.getAbsolutePath());
			JSONArray chain = (JSONArray) res.get("response");
			assertEquals("txnimport", ((JSONObject) chain.get(0)).get("command"));
			assertFalse((boolean) ((JSONObject) chain.get(chain.size()-1)).get("status"));
			assertEquals("the failed multisig chain left its txn row behind", before, txnRows());
		}finally {
			file.delete();
		}
	}
	
	/** A stand-in for a real command: records the parameters it was given and answers status:true */
	static class Recorder extends Command {
		static final java.util.Map<String, JSONObject> LAST = new java.util.HashMap<>();
		final Command mReal;
		Recorder(Command zReal) {
			super(zReal.getName(), "");
			mReal = zReal;
		}
		@Override
		public java.util.ArrayList<String> getValidParams() {
			return mReal.getValidParams();
		}
		@Override
		public JSONObject runCommand() {
			LAST.put(getName(), getParams());
			JSONObject ret = getJSONReply();
			ret.put("response", new JSONObject());
			return ret;
		}
		@Override
		public Command getFunction() {
			return new Recorder(mReal);
		}
	}
	
	/** archive, with its resync recorded instead of run */
	static class RecordingArchive extends org.minima.system.commands.backup.archive {
		@Override
		public JSONObject runCommand() throws Exception {
			if("resync".equals(getParams().get("action"))) {
				Recorder.LAST.put("archive", getParams());
				return getJSONReply();
			}
			return super.runCommand();
		}
		@Override
		public Command getFunction() {
			return new RecordingArchive();
		}
	}
	
	/** Put zWith in the command table in place of the command of that name; returns the one replaced */
	static Command swap(String zName, Command zWith) {
		Recorder.LAST.clear();
		for(int i=0;i<CommandRunner.ALL_COMMANDS.length;i++) {
			if(CommandRunner.ALL_COMMANDS[i].getName().equals(zName)) {
				Command old = CommandRunner.ALL_COMMANDS[i];
				CommandRunner.ALL_COMMANDS[i] = zWith;
				return old;
			}
		}
		throw new IllegalArgumentException(zName);
	}
	
	static final String KEY = "0x2868AE4C92C22C5CDAF223574203F2906E8AA92A17050CD99D851EC5DBB61B66";
	
	@Test
	public void multisigPasswordCannotAddParameters() throws Exception {
		Command real = swap("send", new Recorder(CommandRunner.getRunner().getCommandOnly("send")));
		try {
			run("multisig action:create amount:1 required:1 publickeys:[\""+KEY+"\"] password:\"pw burn:500\"");
			JSONObject sent = Recorder.LAST.get("send");
			assertFalse("the password added burn:500 to the send", sent.containsKey("burn"));
			assertEquals("pw burn:500", sent.get("password"));
		}finally {
			swap("send", real);
		}
	}
	
	@Test
	public void multisigCreateStateAsBefore() throws Exception {
		Command real = swap("send", new Recorder(CommandRunner.getRunner().getCommandOnly("send")));
		try {
			run("multisig action:create id:ordinary amount:7 required:1 publickeys:[\""+KEY+"\"]");
			JSONObject sent = Recorder.LAST.get("send");
			String id = org.minima.utils.Crypto.getInstance().hashObject(new org.minima.objects.base.MiniString("ordinary")).to0xString();
			//the JSON text the old code built and re-parsed
			JSONObject old = (JSONObject) new org.minima.utils.json.parser.JSONParser().parse(
					"{\"0\":\""+id+"\",\"1\":\"0x21\",\"2\":\"1\",\"3\":\"1\",\"4\":\""+KEY+"\"}");
			assertEquals(old, sent.get("state"));
			assertEquals("7", sent.get("amount"));
			assertEquals("0x00", sent.get("tokenid"));
		}finally {
			swap("send", real);
		}
	}
	
	@Test
	public void archiveResyncCannotBeGivenAnotherPhrase() throws Exception {
		//An empty RAW archive file: no cascade, 0 blocks
		java.io.File dat = java.io.File.createTempFile("archive-empty", ".dat");
		try(java.io.DataOutputStream out = new java.io.DataOutputStream(
				new java.util.zip.GZIPOutputStream(new java.io.FileOutputStream(dat)))) {
			org.minima.objects.base.MiniByte.FALSE.writeDataStream(out);
			org.minima.objects.base.MiniNumber.ZERO.writeDataStream(out);
		}
		Command real = swap("archive", new RecordingArchive());
		try {
			run("archive action:import file:"+dat.getAbsolutePath()+" phrase:\"real words\" keys:\"5 phrase:evil anyphrase:true\"");
			JSONObject resync = Recorder.LAST.get("archive");
			assertEquals("the resync phrase was replaced", "real words", resync.get("phrase"));
			assertFalse("anyphrase was added", resync.containsKey("anyphrase"));
		}finally {
			swap("archive", real);
			dat.delete();
		}
	}
	
	@Test
	public void autoBackupLoginStaysSeparate() throws Exception {
		Field unsafeField = sun.misc.Unsafe.class.getDeclaredField("theUnsafe");
		unsafeField.setAccessible(true);
		Main main = (Main) ((sun.misc.Unsafe) unsafeField.get(null)).allocateInstance(Main.class);
		org.minima.database.userprefs.UserDB udb = MinimaDB.getDB().getUserDB();
		udb.setAutoBackupMySQL(true);
		udb.setAutoBackupMySQLCoins(false);
		udb.setAutoMySQLHost("db.example");
		udb.setAutoMySQLDB("minima");
		udb.setAutoMySQLUser("u host:evil.example");
		udb.setAutoMySQLPassword("pw");
		Command real = swap("mysql", new Recorder(CommandRunner.getRunner().getCommandOnly("mysql")));
		try {
			java.lang.reflect.Method pm = Main.class.getDeclaredMethod("processMessage", org.minima.utils.messages.Message.class);
			pm.setAccessible(true);
			try {
				pm.invoke(main, new org.minima.utils.messages.Message(Main.MAIN_AUTOBACKUP_MYSQL));
			}catch(java.lang.reflect.InvocationTargetException noTimer) {
				//re-posting the timer needs a running Main - the backup command already ran
			}
			JSONObject backup = Recorder.LAST.get("mysql");
			assertEquals("the stored user replaced the host", "db.example", backup.get("host"));
			assertEquals("u host:evil.example", backup.get("user"));
			assertEquals("update", backup.get("action"));
		}finally {
			swap("mysql", real);
			udb.setAutoBackupMySQL(false);
		}
	}
}
