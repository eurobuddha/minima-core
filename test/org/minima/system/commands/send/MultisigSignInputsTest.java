package org.minima.system.commands.send;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.lang.reflect.Field;
import java.nio.file.Files;
import java.util.ArrayList;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.database.userprefs.txndb.TxnRow;
import org.minima.database.wallet.Wallet;
import org.minima.objects.Address;
import org.minima.objects.Coin;
import org.minima.objects.StateVariable;
import org.minima.objects.Transaction;
import org.minima.objects.Witness;
import org.minima.objects.base.MiniData;
import org.minima.objects.base.MiniNumber;
import org.minima.system.commands.CommandRunner;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

/**
 * multisig action:sign signs the whole transaction with every wallet key named in input 0's state (2026-11-23):
 * a transaction with any input that is not at the multisig address is refused, before any key is looked up - and
 * (2026-11-24) so is one whose inputs are not all the same multisig (root key, required, total, keys), since every
 * multisig coin is at the one address. Inputs of one multisig still go through.
 */
public class MultisigSignInputsTest {

	static final String KEY = "0x2868AE4C92C22C5CDAF223574203F2906E8AA92A17050CD99D851EC5DBB61B66";
	static final String KEY2 = "0x00000000000000000000000000000000000000000000000000000000000000B2";

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

	static Coin multisigCoin(String zCoinID, String zID, String zRoot, String zRequired, String... zKeys) {
		Coin coin = new Coin(new MiniData(zCoinID), new Address(multisig.MULTISIG_CONTRACT).getAddressData(),
				MiniNumber.ONE, MiniData.ZERO_TXPOWID);
		ArrayList<StateVariable> state = new ArrayList<>();
		if(zID != null) {
			state.add(new StateVariable(0, zID));
		}
		state.add(new StateVariable(1, zRoot));
		state.add(new StateVariable(2, zRequired));
		state.add(new StateVariable(3, ""+zKeys.length));
		for(int i=0;i<zKeys.length;i++) {
			state.add(new StateVariable(4+i, zKeys[i]));
		}
		coin.setState(state);
		return coin;
	}

	static void assertRefusedAsAnotherMultisig(Coin zOther) throws Exception {
		int rows  = MinimaDB.getDB().getCustomTxnDB().listTxns().size();
		File file = txnFile(multisigCoin("0x01", "0xAA", "0x21", "2", KEY, KEY2), zOther);
		try {
			JSONObject res = sign(file);
			assertFalse(res.toString(), (boolean) res.get("status"));
			assertTrue(res.toString(), String.valueOf(res.get("error")).contains("Not one multisig - input 0x02"));
			assertFalse(new File(file.getParentFile(), "signed_"+file.getName()).exists());
			assertEquals("the refused sign left its txn row", rows, MinimaDB.getDB().getCustomTxnDB().listTxns().size());
		}finally {
			file.delete();
		}
	}

	@Test
	public void signRefusesAnInputOfAnotherMultisig() throws Exception {
		//the signer's key alone spends the second coin: as its root key, as 1 of 2, as the only key
		assertRefusedAsAnotherMultisig(multisigCoin("0x02", "0xAA", KEY, "2", KEY, KEY2));
		assertRefusedAsAnotherMultisig(multisigCoin("0x02", "0xAA", "0x21", "1", KEY, KEY2));
		assertRefusedAsAnotherMultisig(multisigCoin("0x02", "0xAA", "0x21", "1", KEY));
		//same keys in another order is another state too
		assertRefusedAsAnotherMultisig(multisigCoin("0x02", "0xAA", "0x21", "2", KEY2, KEY));
	}

	@Test
	public void inputsOfOneMultisigStillSign() throws Exception {
		//two coins of the same multisig (the second was created without an id), one of its keys in a real wallet:
		//the checks pass, the wallet key signs, the signed file is written and the imported row is deleted
		MinimaDB.getDB().setAllowSaveState(false);
		File dir = Files.createTempDirectory("mssign").toFile();
		Wallet wallet = MinimaDB.getDB().getWallet();
		wallet.loadDB(new File(dir, "wallet"));
		File file = null, signed = null;
		try {
			String mine = wallet.createNewKey().getPublicKey();
			int rows = MinimaDB.getDB().getCustomTxnDB().listTxns().size();
			file   = txnFile(multisigCoin("0x01", "0xAA", "0x21", "2", KEY, mine), multisigCoin("0x02", null, "0x21", "2", KEY, mine));
			signed = new File(file.getParentFile(), "signed_"+file.getName());

			JSONObject res = sign(file);
			assertTrue(res.toString(), (boolean) res.get("status"));
			JSONArray steps = (JSONArray) res.get("response");
			assertEquals(res.toString(), "txnsign", ((JSONObject) steps.get(0)).get("command"));
			assertTrue(res.toString(), (boolean) ((JSONObject) steps.get(0)).get("status"));
			assertTrue(signed.exists());
			assertEquals(rows, MinimaDB.getDB().getCustomTxnDB().listTxns().size());

			//the signed file holds one signature
			JSONObject imp = CommandRunner.getRunner().runCommand("txnimport", "id", "mssigned", "file", signed.getAbsolutePath());
			assertTrue(imp.toString(), (boolean) imp.get("status"));
			assertEquals(1, MinimaDB.getDB().getCustomTxnDB().getTransactionRow("mssigned").getWitness().getAllSignatures().size());
			CommandRunner.getRunner().runCommand("txndelete", "id", "mssigned");
		}finally {
			wallet.hardCloseDB();
			MinimaDB.getDB().setAllowSaveState(true);
			if(file != null) file.delete();
			if(signed != null) signed.delete();
			for(File f : dir.listFiles()) f.delete();
			dir.delete();
		}
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
