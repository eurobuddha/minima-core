package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.junit.Test;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;
import org.minima.utils.json.parser.JSONParser;

/**
 * The ';' splitter against the parameter parser, on generated lines (2026-11-23).
 *
 * The rule (CommandRunner.tokenise): a token's value starts after its first ':'. A value that STARTS with '"' runs to
 * the next '"' (no escapes); one that STARTS with '{' or '[' runs to the bracket that brings the count of { [ minus
 * } ] back to 0. With no such end the character is ordinary. A ';' inside such a value is data; every other ';' ends
 * the command. A command that holds a ';' is parsed by the same tokeniser; one without is parsed as it always was.
 *
 * Here an independent reference - written from that rule with regular expressions and bracket prefix sums, sharing
 * no code with the tokeniser - splits and parses each line, and for every line:
 * - the splitter's pieces equal the reference's;
 * - every ';' the splitter left inside a piece is inside a quoted / bracketed value - in the piece and in the line
 *   (it can never end up in a command or parameter name, or start a parameter);
 * - each piece's command name and parameter map (or its refusal) from CommandRunner.getCommand equal the
 *   reference's;
 * - a line with no ';' inside a value splits exactly as the old splitter (StringTokenizer at every ';') split it.
 *
 * Two more oracles, neither written from the rule:
 * - THE ORIGINAL PARSER (wellFormedLinesParseAsTheOriginalParserReadThem): chains as apps build them - quoted text,
 *   JSON, plain values, with ';' inside the quoted and JSON ones. The splitter must give back exactly the commands
 *   the generator joined, and each command's name and parameters must equal what the ORIGINAL parameter parser
 *   (the unchanged code path of a command without ';') gives for the same command with every ';' swapped for a
 *   character that parser has no rule for. (Well formed = no stray quote or bracket in an unquoted value and no
 *   ':' in quoted text: there the original parser's own counters run on, or it pads the ':' with spaces - the
 *   tokeniser does neither.)
 * - SLOTS (dataInAQuotedSlotNeverAddsOrReplacesAParameter): an app's template with adversarial data in its quoted
 *   slots - anything but a '"', which apps must reject. The commands that run are the template's, in order; none
 *   gains a parameter name; no parameter outside a slot changes its value; a slot whose data holds a ';' arrives
 *   verbatim. The same for data JSON-escaped into a JSON string slot - as long as it holds no bracket: the parser
 *   counts every bracket, inside JSON strings too (CommandSplitTest.bracketInAJsonStringSlotEndsTheValueAsItAlwaysDid).
 */
public class CommandSplitDifferentialTest {

	static final int LINES = 30000;

	static final String[] COMMANDS = {"txnstate", "runscript", "hash", "send", "txncreate", "status", "RunScript", "nosuchcommand"};
	static final String[] NAMES    = {"id", "port", "value", "script", "clean", "data", "amount", "address", "state", "x", ""};
	static final String[] BITS     = {"\"", "\\", "[", "]", "{", "}", ";", ":", " ", "\t", "a", "b", "1", "\\\"", ",",
			"0x01", "  ", "\n", "\"\"", "[]", "{}", "\"a\":\"b\"", "\"a\":1", ";;", " ; "};

	/** Same whitespace as the regex \s - space, tab, LF, VT, FF, CR */
	static final Pattern NAME 	= Pattern.compile("[^\\s;:]*");
	static final Pattern REST 	= Pattern.compile("[^\\s;]*");
	static final Pattern QUOTED = Pattern.compile("\"[^\"]*\"");

	static final int PLAIN = 0, DELIM = 1, INSIDE = 2;

	/**
	 * Reference: for each character, PLAIN, or a quote that opens / closes a quoted value (DELIM), or INSIDE a quoted
	 * or bracketed value
	 */
	static int[] refKinds(String zLine) {
		int len 	= zLine.length();
		int[] kinds = new int[len];

		//net brackets up to and including each character
		int[] net = new int[len];
		int sum = 0;
		for(int i=0;i<len;i++) {
			char cc = zLine.charAt(i);
			sum += (cc == '[' || cc == '{') ? 1 : (cc == ']' || cc == '}') ? -1 : 0;
			net[i] = sum;
		}

		int pos = 0;
		while(pos < len) {
			char cc = zLine.charAt(pos);
			if(cc == ';' || Character.toString(cc).matches("\\s")) {
				pos++;
				continue;
			}
			Matcher name = NAME.matcher(zLine).region(pos, len);
			name.lookingAt();
			pos = name.end();
			if(pos < len && zLine.charAt(pos) == ':') {
				int start = pos+1;
				int end   = -1;
				if(start < len && zLine.charAt(start) == '"') {
					Matcher quoted = QUOTED.matcher(zLine).region(start, len);
					if(quoted.lookingAt()) {
						end = quoted.end()-1;
						kinds[start] = DELIM;
						kinds[end]   = DELIM;
						for(int i=start+1;i<end;i++) {
							kinds[i] = INSIDE;
						}
					}
				}else if(start < len && (zLine.charAt(start) == '[' || zLine.charAt(start) == '{')) {
					int before = start == 0 ? 0 : net[start-1];
					for(int i=start;i<len;i++) {
						if(net[i] == before) {
							end = i;
							break;
						}
					}
					if(end != -1) {
						for(int i=start;i<=end;i++) {
							kinds[i] = INSIDE;
						}
					}
				}
				Matcher rest = REST.matcher(zLine).region(end == -1 ? start : end+1, len);
				rest.lookingAt();
				pos = rest.end();
			}
		}
		return kinds;
	}

	/** Reference: the pieces - cut at every PLAIN ';', trimmed, blank ones left out */
	static List<String> refPieces(String zLine) {
		int[] kinds = refKinds(zLine);
		ArrayList<String> pieces = new ArrayList<>();
		int start = 0;
		for(int i=0;i<=zLine.length();i++) {
			if(i == zLine.length() || (zLine.charAt(i) == ';' && kinds[i] == PLAIN)) {
				String piece = zLine.substring(start, i).trim();
				if(!piece.isEmpty()) {
					pieces.add(piece);
				}
				start = i+1;
			}
		}
		return pieces;
	}

	/** The outcome of parsing one command: its name and parameters, or that it was refused / threw */
	static String outcome(Command zCommand) {
		if(zCommand.getName().equals("missingcmd")) {
			return "REFUSED";
		}
		return zCommand.getName()+" "+zCommand.getParams().toString();
	}

	static String production(String zPiece) {
		try {
			return outcome(CommandRunner.getRunner().getCommand(zPiece));
		}catch(Exception exc) {
			return "THREW "+exc.getClass().getSimpleName();
		}
	}

	/**
	 * Reference parse of a piece. A piece with no ';' goes through the unchanged parser - nothing about it is new. A
	 * piece with one: split at PLAIN whitespace, the quotes of a quoted value dropped, name before the first ':'.
	 */
	static String reference(String zPiece) {
		if(zPiece.indexOf(';') == -1) {
			return production(zPiece);
		}
		try {
			int[] kinds = refKinds(zPiece);
			ArrayList<String> tokens = new ArrayList<>();
			StringBuilder token = new StringBuilder();
			for(int i=0;i<=zPiece.length();i++) {
				boolean end = i == zPiece.length() || (kinds[i] == PLAIN && Character.toString(zPiece.charAt(i)).matches("\\s"));
				if(end) {
					if(token.length() > 0) {
						tokens.add(token.toString());
					}
					token = new StringBuilder();
				}else if(kinds[i] != DELIM) {
					token.append(zPiece.charAt(i));
				}
			}

			String name = tokens.get(0).toLowerCase();
			if(CommandRunner.getRunner().getCommandOnly(name) == null) {
				return "REFUSED";
			}
			JSONObject params = new JSONObject();
			for(String tok : tokens.subList(1, tokens.size())) {
				int colon = tok.indexOf(':');
				if(colon == -1) {
					return "REFUSED";
				}
				String value = tok.substring(colon+1).trim();
				Object typed;
				if(value.startsWith("{") && value.endsWith("}")) {
					try {
						typed = (JSONObject) new JSONParser().parse(value);
					}catch(org.minima.utils.json.parser.ParseException exc) {
						return "REFUSED";
					}
				}else if(value.startsWith("[") && value.endsWith("]")) {
					if(name.equals("txnstate")) {
						typed = value;
					}else {
						try {
							typed = (JSONArray) new JSONParser().parse(value);
						}catch(org.minima.utils.json.parser.ParseException exc) {
							return "REFUSED";
						}
					}
				}else {
					if(value.startsWith("\"")) {
						value = value.substring(1);
					}
					if(value.endsWith("\"")) {
						value = value.substring(0, value.length()-1);
					}
					typed = value;
				}
				//a parameter is given once: a repeated name is refused, the last one no longer wins
				if(params.containsKey(tok.substring(0, colon).trim())) {
					return "REFUSED";
				}
				params.put(tok.substring(0, colon).trim(), typed);
			}
			return name+" "+params.toString();
		}catch(Exception exc) {
			return "THREW "+exc.getClass().getSimpleName();
		}
	}

	static String randomBits(Random zRand, int zMax) {
		StringBuilder sb = new StringBuilder();
		int n = zRand.nextInt(zMax+1);
		for(int i=0;i<n;i++) {
			sb.append(BITS[zRand.nextInt(BITS.length)]);
		}
		return sb.toString();
	}

	static String randomValue(Random zRand) {
		switch(zRand.nextInt(6)) {
		case 0:
			return "\"" + randomBits(zRand, 6) + "\"";
		case 1:
			return "[" + randomBits(zRand, 5) + "]";
		case 2:
			return "{\"k\":\"" + randomBits(zRand, 5) + "\"}";
		case 3:
			return "\"" + randomBits(zRand, 6);
		default:
			return randomBits(zRand, 4).replace(" ", "").replace("\t", "").replace("\n", "");
		}
	}

	static String randomLine(Random zRand) {
		if(zRand.nextInt(4) == 0) {
			//character soup
			return randomBits(zRand, 30);
		}
		StringBuilder line = new StringBuilder();
		int commands = 1 + zRand.nextInt(4);
		for(int c=0;c<commands;c++) {
			if(c > 0) {
				line.append(new String[] {";", " ; ", ";;", "; ", ";\t"}[zRand.nextInt(5)]);
			}
			line.append(COMMANDS[zRand.nextInt(COMMANDS.length)]);
			int params = zRand.nextInt(5);
			String last = null;
			for(int p=0;p<params;p++) {
				//sometimes the same name twice
				String name = (last != null && zRand.nextInt(5) == 0) ? last : NAMES[zRand.nextInt(NAMES.length)];
				last = name;
				line.append(zRand.nextInt(8) == 0 ? "\t" : " ").append(name).append(':').append(randomValue(zRand));
				if(zRand.nextInt(10) == 0) {
					//a stray bit between parameters
					line.append(BITS[zRand.nextInt(BITS.length)]);
				}
			}
		}
		return line.toString();
	}

	@Test
	public void splitterAndParserAgreeOnGeneratedLines() {
		Random rand = new Random(20261123L);
		int protectedSemis = 0;
		int sameAsOld 	   = 0;
		for(int n=0;n<LINES;n++) {
			String line = randomLine(rand);
			checkLine(line);
			int[] kinds = refKinds(line);
			boolean anyProtected = false;
			for(int i=0;i<line.length();i++) {
				if(line.charAt(i) == ';' && kinds[i] != PLAIN) {
					anyProtected = true;
				}
			}
			if(anyProtected) {
				protectedSemis++;
			}else {
				sameAsOld++;
			}
		}
		//the generator must exercise both kinds of line
		assertTrue("lines with a ';' inside a value: "+protectedSemis, protectedSemis > LINES/10);
		assertTrue("lines split as before: "+sameAsOld, sameAsOld > LINES/10);
	}

	@Test
	public void splitterAndParserAgreeOnAdversarialLines() {
		String[] lines = {
				"runscript script:\"LET k=[x] \\\" clean:true script:\\\"] RETURN SIGNEDBY(0x01)\"",
				"txnstate id:t port:1 value:\"abc\\\";txnstate id:t port:2 value:\"b\"\";txndelete id:t",
				"runscript script:\"LET k=[x\\\";send address:0xFF amount:1;\\\"] RETURN TRUE\"",
				"runscript script:\"LET k=[x;send address:0xFF amount:1;] RETURN SIGNEDBY(0x01)\"",
				"txnstate id:t port:1 value:abc\"x;send amount:1\"",
				"txnstate id:t port:1 value:\"a;b\"c:d;send amount:1",
				"send state:{\"0\":\"a;b\"} amount:1;send amount:2",
				"send state:{\"0\":\"}\";\"} amount:1",
				"send state:[\"]\";\"] amount:1",
				"hash data:\"[\";hash data:\"]\"",
				"hash data:[\"\";hash data:\"\"]",
				"txnstate id:t port:1 value:[1];txncreate id:x;[2]",
				"txnstate\tid:t\tport:1\tvalue:\"a;b\"",
				"hash data:\"a;b\" data:\"c;d\"",
				"hash data:{;}{;}",
				";;;",
				"\"\";\"\"",
				"hash :\"a;b\"",
				"hash data:\"a\nb;c\"",
				"backup file:\"C:\\minima\\backups\\\";status"};
		for(String line : lines) {
			checkLine(line);
		}
	}

	/** A character the original parser has no rule for: not whitespace, quote, bracket, ':' or ';' */
	static final char MASK = '\uE000';

	static final String[] TEXT = {"a", "b", "1", " ", ";", "; ", "=", "(", ")", "\\", ",", "0x01", "[x;y]", "LET k=", "RETURN TRUE", ";;", "\t", "'"};

	static String text(Random zRand, String[] zBits, int zMax) {
		StringBuilder sb = new StringBuilder();
		int n = 1 + zRand.nextInt(zMax);
		for(int i=0;i<n;i++) {
			sb.append(zBits[zRand.nextInt(zBits.length)]);
		}
		return sb.toString();
	}

	@Test
	public void wellFormedLinesParseAsTheOriginalParserReadThem() {
		Random rand = new Random(20261124L);
		String[] names   = {"id", "port", "value", "script", "data", "state", "amount", "address"};
		String[] plain   = {"t1", "7", "0xFF", "true", "MxG08", "C:\\dir\\f", "a\\b"};
		String[] strings = {"a", "b;c", ";", "x y", "0x01", ""};
		int withSemi = 0;
		for(int n=0;n<LINES;n++) {
			ArrayList<String> commands = new ArrayList<>();
			int count = 1 + rand.nextInt(4);
			for(int c=0;c<count;c++) {
				StringBuilder cmd = new StringBuilder(COMMANDS[rand.nextInt(6)]);
				int params = rand.nextInt(5);
				for(int p=0;p<params;p++) {
					cmd.append(' ').append(names[rand.nextInt(names.length)]).append(':');
					switch(rand.nextInt(5)) {
					case 0:
					case 1:
						cmd.append('"').append(text(rand, TEXT, 6)).append('"');
						break;
					case 2:
						cmd.append("{\"").append(rand.nextInt(9)).append("\":\"").append(strings[rand.nextInt(strings.length)])
							.append("\",\"k\":\"").append(strings[rand.nextInt(strings.length)]).append("\"}");
						break;
					case 3:
						cmd.append("[\"").append(strings[rand.nextInt(strings.length)]).append("\",1]");
						break;
					default:
						cmd.append(plain[rand.nextInt(plain.length)]);
					}
				}
				commands.add(cmd.toString().trim());
			}
			String line = String.join(new String[] {";", " ; ", ";  "}[rand.nextInt(3)], commands) + (rand.nextBoolean() ? ";" : "");

			//exactly the commands that were joined
			assertEquals(line, commands, CommandRunner.splitCommands(line));

			//each parsed as the original parser reads the same command without its ';'
			for(String command : commands) {
				if(command.indexOf(';') != -1) {
					withSemi++;
				}
				String original = production(command.replace(';', MASK)).replace(MASK, ';');
				assertEquals(line+" @ "+command, original, production(command));
			}
		}
		assertTrue("commands holding a ';': "+withSemi, withSemi > LINES/2);
	}

	/** One command of a template: its name and parameters; a parameter value of null is a slot */
	static final class Slotted {
		final String mName;
		final String[][] mParams;
		Slotted(String zName, String[]... zParams) {
			mName   = zName;
			mParams = zParams;
		}
	}

	static final Slotted[][] TEMPLATES = {
			{new Slotted("runscript", new String[] {"script", null}, new String[] {"clean", "true"})},
			{new Slotted("txnstate", new String[] {"id", "t"}, new String[] {"port", "1"}, new String[] {"value", null}),
			 new Slotted("txnstate", new String[] {"id", "t"}, new String[] {"port", "2"}, new String[] {"value", null}),
			 new Slotted("txndelete", new String[] {"id", "t"})},
			{new Slotted("hash", new String[] {"data", null}),
			 new Slotted("send", new String[] {"address", "0xFF"}, new String[] {"amount", "1"})},
			{new Slotted("txncreate", new String[] {"id", "t"}),
			 new Slotted("txnstate", new String[] {"value", null}, new String[] {"id", "t"}, new String[] {"port", "3"}),
			 new Slotted("txnstate", new String[] {"id", "t"}, new String[] {"value", null}, new String[] {"port", "4"})}};

	static String jsonEscape(String zData) {
		return zData.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n").replace("\t", "\\t");
	}

	@Test
	public void dataInAQuotedSlotNeverAddsOrReplacesAParameter() {
		Random rand = new Random(20261125L);
		ArrayList<String> noquote = new ArrayList<>();
		for(String bit : BITS) {
			if(bit.indexOf('"') == -1) {
				noquote.add(bit);
			}
		}
		noquote.addAll(Arrays.asList(" clean:true", " script:x", " port:9", ";send address:0xEE amount:9;", " id:other", "]", "[", "}"));
		String[] bits = noquote.toArray(new String[0]);

		int verbatim = 0;
		for(int n=0;n<LINES;n++) {
			Slotted[] template = TEMPLATES[rand.nextInt(TEMPLATES.length)];
			boolean json = rand.nextInt(4) == 0;

			//build the line as an app does
			StringBuilder line = new StringBuilder();
			ArrayList<String> datas = new ArrayList<>();
			for(Slotted cmd : template) {
				if(line.length() > 0) {
					line.append(';');
				}
				line.append(cmd.mName);
				for(String[] param : cmd.mParams) {
					line.append(' ').append(param[0]).append(':');
					if(param[1] != null) {
						line.append(param[1]);
					}else if(json) {
						//JSON-escaped data in a JSON string - without brackets: the parser counts every bracket, inside
						//JSON strings too (CommandSplitTest.bracketInAJsonStringSlotEndsTheValueAsItAlwaysDid)
						String data = randomBits(rand, 6).replaceAll("[\\[\\]{}]", "");
						datas.add(data);
						line.append("{\"1\":\"").append(jsonEscape(data)).append("\"}");
					}else {
						String data = text(rand, bits, 6);
						datas.add(data);
						line.append('"').append(data).append('"');
					}
				}
			}

			List<String> pieces = CommandRunner.splitCommands(line.toString());
			assertEquals(line.toString(), template.length, pieces.size());

			//the commands that run - a chain stops at the first refused one
			int slot = 0;
			for(int c=0;c<pieces.size();c++) {
				Command got = CommandRunner.getRunner().getCommand(pieces.get(c));
				if(got.getName().equals("missingcmd")) {
					assertFalse("JSON-escaped data without brackets was refused: "+line, json);
					//refused - nothing of it runs and the chain stops: quoted data that starts and ends with a bracket
					//is read as JSON, as it always was
					break;
				}
				Slotted want = template[c];
				assertEquals(line.toString(), want.mName, got.getName());

				JSONObject params = got.getParams();
				ArrayList<String> names = new ArrayList<>();
				boolean semi = false;
				for(String[] param : want.mParams) {
					names.add(param[0]);
					if(param[1] == null) {
						String data = datas.get(slot++);
						semi |= data.indexOf(';') != -1;
						if(json) {
							//the whole JSON value, decoded by the JSON parser
							assertEquals(line.toString(), data, ((JSONObject) params.get(param[0])).get("1"));
						}else if(data.indexOf(';') != -1) {
							//new: a slot holding a ';' arrives verbatim (the parser trims a value, and reads one
							//that starts and ends with a bracket as JSON - that may be refused above, never run)
							assertEquals(line.toString(), data.trim(), String.valueOf(params.get(param[0])));
							verbatim++;
						}
					}else if(params.containsKey(param[0])) {
						//a parameter outside the slots keeps its value
						assertEquals(line.toString(), param[1], params.get(param[0]));
					}
				}
				for(Object name : params.keySet()) {
					assertTrue("parameter "+name+" added by data: "+line, names.contains(name));
				}
				if(semi || json) {
					//and with the new tokeniser no parameter is lost either
					assertEquals(line.toString(), names.size(), params.size());
				}
			}
		}
		assertTrue("slots that arrived verbatim: "+verbatim, verbatim > LINES/10);
	}

	static void checkLine(String zLine) {
		List<String> pieces = CommandRunner.splitCommands(zLine);
		assertEquals(zLine, refPieces(zLine), pieces);

		int[] lineKinds = refKinds(zLine);
		int from = 0;
		for(String piece : pieces) {
			//where the piece sits in the line
			int at = zLine.indexOf(piece, from);
			assertTrue(zLine, at >= 0);
			from = at + piece.length();

			//every ';' left in the piece is inside a quoted / bracketed value - of the piece and of the line
			int[] pieceKinds = refKinds(piece);
			for(int i=0;i<piece.length();i++) {
				if(piece.charAt(i) == ';') {
					assertEquals(zLine+" @ "+piece, INSIDE, pieceKinds[i]);
					assertEquals(zLine+" @ "+piece, INSIDE, lineKinds[at+i]);
				}
			}

			//the same command and parameters as the reference
			assertEquals(zLine+" @ "+piece, reference(piece), production(piece));
		}

		//no ';' inside a value: exactly the old split
		boolean anyProtected = false;
		for(int i=0;i<zLine.length();i++) {
			if(zLine.charAt(i) == ';' && lineKinds[i] != PLAIN) {
				anyProtected = true;
			}
		}
		if(!anyProtected) {
			assertEquals(zLine, CommandSplitTest.oldSplit(zLine), pieces);
		}
	}
}
