package org.minima.system.commands.send;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.lang.reflect.Field;
import java.util.ArrayList;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.database.userprefs.txndb.TxnRow;
import org.minima.objects.Address;
import org.minima.objects.Coin;
import org.minima.objects.StateVariable;
import org.minima.objects.Transaction;
import org.minima.objects.Witness;
import org.minima.objects.base.MiniData;
import org.minima.objects.base.MiniNumber;
import org.minima.system.commands.CommandRunner;
import org.minima.utils.json.JSONObject;

/**
 * multisig action:sign signs the whole transaction with every wallet key named in input 0's state (2026-11-23):
 * a transaction with any input that is not at the multisig address is refused, before any key is looked up.
 */
public class MultisigSignInputsTest {

	static final String KEY = "0x2868AE4C92C22C5CDAF223574203F2906E8AA92A17050CD99D851EC5DBB61B66";

	@BeforeClass
	public static void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
	}

	/** A .txn file holding these inputs */
	static File txnFile(Coin... zInputs) throws Exception {
		Transaction txn = new Transaction();
		for(Coin input : zInputs) {
			txn.addInput(input);
		}
		MinimaDB.getDB().getCustomTxnDB().addCompleteTransaction(new TxnRow("mssignfile", txn, new Witness()));
		File file = File.createTempFile("mssign", ".txn");
		JSONObject res = CommandRunner.getRunner().runCommand("txnexport", "id", "mssignfile", "file", file.getAbsolutePath());
		assertTrue(res.toString(), (boolean) res.get("status"));
		CommandRunner.getRunner().runCommand("txndelete", "id", "mssignfile");
		return file;
	}

	static JSONObject sign(File zFile) {
		return (JSONObject) CommandRunner.getRunner().runMultiCommand("multisig action:sign file:"+zFile.getAbsolutePath()).get(0);
	}

	@Test
	public void signRefusesAnInputOutsideTheMultisigAddress() throws Exception {
		MiniData msaddress = new Address(multisig.MULTISIG_CONTRACT).getAddressData();
		Coin ms = new Coin(new MiniData("0x01"), msaddress, MiniNumber.ONE, MiniData.ZERO_TXPOWID);
		ArrayList<StateVariable> state = new ArrayList<>();
		state.add(new StateVariable(4, KEY));
		ms.setState(state);
		Coin own = new Coin(new MiniData("0x02"), new MiniData(KEY), new MiniNumber(1000), MiniData.ZERO_TXPOWID);

		int rows = MinimaDB.getDB().getCustomTxnDB().listTxns().size();
		File file = txnFile(ms, own);
		try {
			JSONObject res = sign(file);
			assertFalse((boolean) res.get("status"));
			assertTrue(res.toString(), String.valueOf(res.get("error")).contains("is not at the multisig address"));
			assertFalse(new File(file.getParentFile(), "signed_"+file.getName()).exists());
			assertEquals("the refused sign left its txn row", rows, MinimaDB.getDB().getCustomTxnDB().listTxns().size());
		}finally {
			file.delete();
		}

		file = txnFile();
		try {
			assertTrue(String.valueOf(sign(file).get("error")).contains("no inputs"));
		}finally {
			file.delete();
		}
	}
}
