package org.minima.system.commands.send;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;
import java.util.ArrayList;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.objects.StateVariable;
import org.minima.objects.base.MiniNumber;
import org.minima.system.commands.CommandRunner;
import org.minima.utils.json.JSONObject;

/**
 * A multisig coin's state is on-chain data that anyone paying the address chooses; a spend with change copies
 * it into the change output. The old ';' chain ran a state like "[x];txncreate id:..;[y]" as a command when the
 * owner spent the coin. The spend commands are now built by
 * multisig.spendCommands from separated parameters: the state is one value of one txnstate.
 */
public class MultisigSpendStateTest {

	static final String HOSTILE = "[x];txncreate id:pwnedspend;[y]";
	
	@BeforeClass
	public static void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
	}
	
	@Test
	public void hostileStateIsCopiedAsOneValue() {
		ArrayList<StateVariable> state = new ArrayList<>();
		state.add(new StateVariable(1, "0x21"));
		state.add(new StateVariable(10, HOSTILE));
		
		ArrayList<Object[]> cmds = multisig.spendCommands("spendtxn", "0x01", MiniNumber.ONE, "0x02", "0x00",
				MiniNumber.ONE, state, "0x03", "spend.txn");
		
		StringBuilder names = new StringBuilder();
		for(Object[] c : cmds) {
			names.append(c[0]).append(' ');
		}
		assertEquals("txncreate txninput txnoutput txnstate txnstate txnoutput txnexport txndelete ", names.toString());
		assertEquals(HOSTILE, cmds.get(4)[6]);
		
		//Run the create and the state copies: nothing but the state is added
		CommandRunner runner = CommandRunner.getRunner();
		runner.runCommand("txncreate", "id", "spendtxn");
		for(Object[] c : cmds) {
			if(c[0].equals("txnstate")) {
				JSONObject res = runner.runCommand((String) c[0], java.util.Arrays.copyOfRange(c, 1, c.length));
				assertTrue(res.toString(), (boolean) res.get("status"));
			}
		}
		assertNull("the state ran a command", MinimaDB.getDB().getCustomTxnDB().getTransactionRow("pwnedspend"));
		assertEquals(HOSTILE, MinimaDB.getDB().getCustomTxnDB().getTransactionRow("spendtxn")
				.getTransaction().getStateValue(10).toString());
	}
}
