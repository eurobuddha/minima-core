package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.StringTokenizer;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

/**
 * A ';' inside a quoted or bracketed value is data, never a command separator (2026-11-23), and the splitter and
 * the parameter parser read a line through ONE tokeniser (CommandRunner.tokenise), so they agree on where every
 * value ends. CommandSplitDifferentialTest checks that on generated lines.
 *
 * The old runMultiCommand split the whole line at every ';', so an app that put chain data into
 * runscript script:"..", newscript script:"..", txnstate value:".." or a JSON parameter let that data start a
 * command of its own - run with the app's rights.
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

	/**
	 * What the old splitter (new StringTokenizer(line, ";"), each piece trimmed) made of a line, empty pieces left out
	 */
	static List<String> oldSplit(String zLine) {
		ArrayList<String> pieces = new ArrayList<>();
		StringTokenizer strtok = new StringTokenizer(zLine, ";");
		while(strtok.hasMoreTokens()) {
			String piece = strtok.nextToken().trim();
			if(!piece.isEmpty()) {
				pieces.add(piece);
			}
		}
		return pieces;
	}

	static void assertSplitsAsBefore(String zLine) {
		assertEquals(zLine, oldSplit(zLine), CommandRunner.splitCommands(zLine));
	}

	static JSONObject params(String zCommand) {
		return CommandRunner.getRunner().getCommand(zCommand).getParams();
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
		//a value that starts with a bracket runs to the bracket that closes it - every bracket counts, as the
		//parser's own JSON tokenising counts them, and a quote inside it is a character
		assertSplit("x p:{\"a\":\"1;2\",\"b\":{\"c\":\"d\"}};y", "x p:{\"a\":\"1;2\",\"b\":{\"c\":\"d\"}}", "y");
		assertSplit("x p:[1,[2;3]];y", "x p:[1,[2;3]]", "y");
		assertSplit("x p:[\";\"];y", "x p:[\";\"]", "y");
		//NO escapes: a quoted value ends at the next '"' - the parser has never read \" as an escape
		assertSplit("x v:\"a\\\";b\";y", "x v:\"a\\\"", "b\"", "y");
		assertSplit("x v:\"a\\\\\";y", "x v:\"a\\\\\"", "y");
		assertSplit("backup file:\"C:\\minima\\backups\\\";status", "backup file:\"C:\\minima\\backups\\\"", "status");
		//a quote or bracket that does not START a value is a character: the ';' after it splits, as always
		assertSplit("x v:a\"b;y v:c\"d", "x v:a\"b", "y v:c\"d");
		assertSplit("x v:a[b;y v:c]d", "x v:a[b", "y v:c]d");
		assertSplit("x \"a;b\";y", "x \"a", "b\"", "y");
		assertSplit("x v:\"a\"b;c\";y", "x v:\"a\"b", "c\"", "y");
		//outside a value a backslash and a stray closing bracket are plain text
		assertSplit("x file:C:\\dir\\f;y", "x file:C:\\dir\\f", "y");
		assertSplit("x v:a];y", "x v:a]", "y");
		//whitespace before the value means no value starts at the quote
		assertSplit("x v: \"a;b\"", "x v: \"a", "b\"");
	}

	@Test
	public void unclosedQuoteOrBracketSplitsAsBefore() {
		//the parser has always read a quote or bracket that is never closed as a plain character - and the old code
		//ran such lines, split at every ';'. They still are, never refused.
		String[] lines = {
				"a;b v:\"open",
				"a;b v:\"ends with an escaped quote\\\"",
				"a;b v:\"ends with a backslash\\",
				"a;b p:{\"a\":1",
				"a;b p:[1,2",
				"a;b p:{\"a\":\"open}",
				"a v:\"x;b v:[y;c v:{z;d",
				"a v:]x;b v:}y;c"};
		for(String line : lines) {
			assertSplitsAsBefore(line);
		}
	}

	@Test
	public void unclosedQuoteRunsAsBefore() {
		JSONArray res = CommandRunner.getRunner().runMultiCommand(
				"txncreate id:unclosed;txnstate id:unclosed port:1 value:\"[1]");
		assertEquals(res.toString(), 2, res.size());
		assertTrue(res.toString(), (boolean) ((JSONObject) res.get(1)).get("status"));
		assertEquals("[1]", MinimaDB.getDB().getCustomTxnDB().getTransactionRow("unclosed")
				.getTransaction().getStateValue(1).getData().toString());
		CommandRunner.getRunner().runMultiCommand("txndelete id:unclosed");
	}

	@Test
	public void scriptStringLiteralWithSemicolonRunsOneCommand() throws Exception {
		Command real = CommandInjectionTest.swap("send", new CommandInjectionTest.Recorder(
				CommandRunner.getRunner().getCommandOnly("send")));
		try {
			//A KISS string literal holding a ';'
			String script = "LET k=[x;send address:0xFF amount:1;] RETURN SIGNEDBY(0x01)";
			String line = "runscript script:\"" + script + "\"";
			JSONArray res = CommandRunner.getRunner().runMultiCommand(line);
			assertEquals(1, res.size());
			assertEquals("runscript", ((JSONObject) res.get(0)).get("command"));
			assertNull("the ';' in the script ran a send", CommandInjectionTest.Recorder.LAST.get("send"));

			//and the script arrives whole - no parameter added, none replaced
			JSONObject expected = new JSONObject();
			expected.put("script", script);
			assertEquals(expected, params(line));
			assertEquals(expected, ((JSONObject) res.get(0)).get("params"));
		}finally {
			CommandInjectionTest.swap("send", real);
		}
	}

	@Test
	public void appEscapedQuoteIsNotAnEscape() {
		//LIMIT: the parameter parser has never read \" as an escaped quote - the quote ends the value. So the splitter
		//does not either: an app that "escapes" a quote in data as \" gets the line split where the parser ends the
		//value - exactly as the old splitter split it. No splitter can keep such a value whole without disagreeing
		//with the parser. Apps must REJECT a value holding '"' or '\' (or send it as hex), never escape it.
		String line = "runscript script:\"LET k=[x\\\";send address:0xFF amount:1;\\\"] RETURN TRUE\"";
		assertSplitsAsBefore(line);
		assertSplit(line, "runscript script:\"LET k=[x\\\"", "send address:0xFF amount:1", "\\\"] RETURN TRUE\"");
	}

	@Test
	public void quoteInsideAValueCannotHideACommand() {
		//Two quoted values in one chain, each holding \": the first value ends at its first quote, so the ';' after
		//it splits - three commands run, as with the old splitter; none hides inside another's value
		String line = "txnstate id:hide port:1 value:\"abc\\\";txnstate id:hide port:2 value:\"b\"\";txndelete id:hide";
		assertSplitsAsBefore(line);
		List<String> pieces = CommandRunner.splitCommands(line);
		assertEquals(3, pieces.size());
		assertEquals("1", params(pieces.get(0)).get("port"));
		assertEquals("abc\\", params(pieces.get(0)).get("value"));
		assertEquals("2", params(pieces.get(1)).get("port"));
		assertEquals("txndelete", CommandRunner.getRunner().getCommand(pieces.get(2)).getName());
	}

	@Test
	public void escapedQuoteInOneCommandIsReadAsTheParserAlwaysDid() {
		//No ';' - one command, parsed exactly as before. The parser ends the value at the quote after the backslash,
		//so this text adds clean:true and a second script:. A node cannot tell that from a line typed this way:
		//apps must reject '"' and '\' in data.
		String line = "runscript script:\"LET k=[x] \\\" clean:true script:\\\"] RETURN SIGNEDBY(0x01)\"";
		assertSplit(line, line);
		JSONObject got = params(line);
		assertEquals("true", got.get("clean"));
		assertEquals("\\] RETURN SIGNEDBY(0x01)", got.get("script"));
	}

	@Test
	public void bracketInAJsonStringSlotEndsTheValueAsItAlwaysDid() {
		//LIMIT (not new, and not about ';'): the parameter parser counts EVERY bracket of a {..} / [..] value, also
		//one inside a JSON string. Data pasted into state:{"1":"<data>"} - JSON-escaped or not - that holds a ']' or
		//'}' ends the value early, and the rest of the data is read as parameters: the LAST address: wins. The
		//splitter counts the same way, so the two agree; apps must reject [ ] { } (and '"', '\', ';', control
		//characters) in data they put into a JSON parameter, or send it as hex.
		String line = "send address:0x01 amount:1 state:{\"1\":\"] address:0xEE x:[\"}";
		assertSplit(line, line);
		JSONObject got = params(line);
		assertEquals("0xEE", got.get("address"));
		assertEquals("{\"1\":\"]", got.get("state"));

		//with a ';' in the data the same reading holds (the tokeniser counts as the parser does)
		String semi = "send address:0x01 amount:1 state:{\"1\":\"] address:0xEE x:[;\"}";
		assertSplit(semi, semi);
		got = params(semi);
		assertEquals("0xEE", got.get("address"));
		assertEquals("{\"1\":\"]", got.get("state"));
	}

	@Test
	public void emptyAndControlCharacterPieces() {
		//a piece of nothing but whitespace or control characters (trim() drops them) is skipped
		assertSplit("a;\u0001;b; \t ;", "a", "b");
		JSONObject none = CommandRunner.getRunner().runOneCommand("0x00", " ; ");
		assertFalse((boolean) none.get("status"));
		assertEquals("No command given - nothing was run", none.get("error"));
		assertEquals(0, CommandRunner.getRunner().runMultiCommand(" ;; ").size());
	}

	@Test
	public void callerShapedLinesRunAsBefore() {
		//Lines shaped as apps build them, with data that holds a backslash, bracket, brace or quote: each splits
		//exactly as the old splitter split it (none is refused)
		ArrayList<String> lines = new ArrayList<>(Arrays.asList(
				//a recovery phrase / password / Windows folder ending in a backslash, inside quotes
				"megammrsync action:resync host:megammr.example:9001 phrase:\"alpha beta gamma delta\\\" anyphrase:true keyuses:1000",
				"backup file:\"C:\\minima\\backups\\\"",
				"restoresync file:\"/Users/me/minima backup.bak\" password:\"pass\\\"",
				"reset archivefile:\"/x/a.raw.dat\" action:restore file:\"/x/b.bak\" password:\"[pw\"",
				//unquoted names and paths with a stray bracket, brace or quote
				"maxima action:setname name:Bob [the {builder\"",
				"keets action:publish path:/chat/img-1.jpg file:/Users/me/Pictures/[draft {1}\"x.jpg"));

		String[] passwords = {"pa[ss", "pa{ss", "pa\"ss", "[pass", "{pass", "\"pass", "pass]", "p\"a\"ss", "p\\ss", "\"pa\"ss", "pa}ss"};
		for(String pw : passwords) {
			//unquoted password, restore from a backup
			lines.add("restore file:\"C:\\minima\\backups\\minima.bak\" password:" + pw);
			//a send with a JSON state and an unquoted vault password - after the state, and before it
			lines.add("send amount:1 address:0x05 state:{\"1\":\"0x06\",\"2\":\"0x07\",\"3\":\"0x08\"} password:" + pw);
			lines.add("send amount:1 address:0x05 tokenid:0x00  password:" + pw
					+ " state:{\"0\": \"0xFF\",\"1\": \"a\", \"2\":\"b\", \"3\":\"c\", \"4\": \"d\"}");
			//a claim chain that signs with an unquoted vault password, then posts and deletes
			lines.add("newscript trackall:false script:\"RETURN SIGNEDBY(0x01)\";txndelete id:myhtlc;txncreate id:myhtlc;"
					+ "txninput id:myhtlc coinid:0x01;txnoutput id:myhtlc amount:1 address:0x02 storestate:false;"
					+ "txnstate id:myhtlc port:0 value:0x03;txnsign id:myhtlc publickey:0x04 password:" + pw + ";"
					+ "txnpost id:myhtlc auto:true;txndelete id:myhtlc");
			lines.add("txncreate id:checksignature1;txnsign id:checksignature1 publickey: 0x04 password:" + pw + ";");
		}

		for(String line : lines) {
			assertSplitsAsBefore(line);
		}

		//the claim chain is still nine commands, whatever the password holds
		for(String pw : passwords) {
			String chain = "newscript trackall:false script:\"RETURN SIGNEDBY(0x01)\";txndelete id:myhtlc;txncreate id:myhtlc;"
					+ "txninput id:myhtlc coinid:0x01;txnoutput id:myhtlc amount:1 address:0x02 storestate:false;"
					+ "txnstate id:myhtlc port:0 value:0x03;txnsign id:myhtlc publickey:0x04 password:" + pw + ";"
					+ "txnpost id:myhtlc auto:true;txndelete id:myhtlc";
			assertEquals(pw, 9, CommandRunner.splitCommands(chain).size());
		}

		//and a single command holding such a value is ONE command to runOneCommand
		assertEquals(1, CommandRunner.splitCommands("restore file:\"C:\\minima\\backups\\\" password:[pw").size());
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

	@Test
	public void unquotedStringStateStillSplitsButRunOneCommandRefuses() {
		//LIMIT: a STRING state is [..], so a coin state "[1];txncreate id:unqpwn;[2]" pasted UNQUOTED after value: is
		//the closed value [1], a ';', and a command. The splitter cannot tell - apps must quote (and validate) data.
		CommandRunner.getRunner().runMultiCommand("txncreate id:unqrow");
		String line = "txnstate id:unqrow port:9 value:[1];txncreate id:unqpwn;[2]";
		CommandRunner.getRunner().runMultiCommand(line);
		assertTrue("documented limit: an unquoted chain still splits", txnExists("unqpwn"));
		CommandRunner.getRunner().runMultiCommand("txndelete id:unqpwn");

		//runOneCommand: a caller that means ONE command - the line runs nothing
		JSONObject one = CommandRunner.getRunner().runOneCommand("0x00", line);
		assertFalse((boolean) one.get("status"));
		assertTrue(one.toString(), String.valueOf(one.get("error")).contains("Exactly one command expected"));
		assertFalse("runOneCommand ran the second command", txnExists("unqpwn"));

		//one command - also with a ';' inside a quoted value - runs as before
		one = CommandRunner.getRunner().runOneCommand("0x00", "txnstate id:unqrow port:9 value:\"[1];x;[2]\"");
		assertTrue(one.toString(), (boolean) one.get("status"));
		//an unclosed quote is a character, as it always was to the parser: one command, it runs
		one = CommandRunner.getRunner().runOneCommand("0x00", "txnstate id:unqrow port:9 value:\"[1]");
		assertTrue(one.toString(), (boolean) one.get("status"));
		CommandRunner.getRunner().runMultiCommand("txndelete id:unqrow");
	}
}
