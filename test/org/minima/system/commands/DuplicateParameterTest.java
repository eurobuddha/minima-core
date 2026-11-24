package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.Test;
import org.minima.system.commands.base.missingcmd;
import org.minima.utils.json.JSONObject;

/**
 * A parameter is given once. The parser kept the LAST value of a repeated name and counts every bracket,
 * those inside JSON strings too, so text an app put into a value could close the value and name "address", "amount",
 * "port" or "script" again: the command then ran with the text's values, with no ';' and no quote involved.
 */
public class DuplicateParameterTest {

	static Command parse(String zLine) {
		return CommandRunner.getRunner().getCommand(zLine);
	}

	static void refused(String zLine, String zName) throws Exception {
		Command c = parse(zLine);
		assertTrue(zLine+" -> "+c.getClass().getName(), c instanceof missingcmd);
		JSONObject res = c.runCommand();
		assertFalse(zLine, (boolean) res.get("status"));
		assertTrue(res.toString(), ((String) res.get("error")).contains("Parameter given twice") && ((String) res.get("error")).endsWith(" : "+zName));
	}

	@Test
	public void aRepeatedNameIsRefusedNotTheLastOneUsed() throws Exception {
		//outside data in a JSON string slot closes the value and names amount / address / state again
		refused("send amount:0.01 address:0x94F2 state:{\"0\":\"0xAA\",\"6\":\"[1]}x amount:5000 address:0xEE state:{} debug:y{[1]\",\"7\":\"1\"}", "amount");
		refused("send amount:10 mine:true address:0x0C state:{\"0\":\"0xAA\",\"4\":\"}x address:0xEE amount:5000 state:{} debug:y{\",\"5\":\"0xBB\"} tokenid:0x00", "address");
		refused("send address:0x01 amount:1 state:{\"1\":\"] address:0xEE state:{} password:[\"}", "address");
		//outside data in a plain slot adds a second port / value
		refused("txnstate id:fill_1 port:15 value:Mx port:9 value:0xEE", "port");
		refused("txnstate id:autoreveal_1 port:9 value:[a] port:0 value:[b]", "port");
		//a quote that ends a quoted script early, then the name again
		refused("runscript script:\"LET k=[x] \\\" clean:true script:\\\"] RETURN SIGNEDBY(0x01)\"", "script");
		//the plain case
		refused("balance tokenid:0x00 tokenid:0x01", "tokenid");
	}

	@Test
	public void aRepeatedNameInOneCommandOfAChainStopsThatCommandOnly() {
		//the same parser reads a command that holds a ';'
		java.util.List<String> pieces = CommandRunner.splitCommands("txnstate id:t port:1 value:\"a;b\" port:2;txndelete id:t");
		assertEquals(2, pieces.size());
		assertTrue(parse(pieces.get(0)) instanceof missingcmd);
		assertEquals("txndelete", parse(pieces.get(1)).getName());
	}

	@Test
	public void everyNameOnceRunsAsBefore() {
		JSONObject p = parse("send amount:1 address:0x01 state:{\"1\":\"a\",\"2\":\"b\"} tokenid:0x00").getParams();
		assertEquals("1", p.get("amount"));
		assertEquals("0x01", p.get("address"));
		assertEquals("0x00", p.get("tokenid"));
		assertTrue(p.get("state") instanceof JSONObject);
		//the same name in two DIFFERENT commands is two commands, not a repeat
		assertEquals("txnstate", parse("txnstate id:a port:1 value:1").getName());
		assertEquals("2", parse("txnstate id:a port:2 value:1").getParams().get("port"));
	}
}
