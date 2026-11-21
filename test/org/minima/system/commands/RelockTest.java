package org.minima.system.commands;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.lang.reflect.Field;
import java.nio.file.Files;

import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.database.wallet.Wallet;
import org.minima.system.commands.backup.vault;
import org.minima.utils.json.JSONObject;

/**
 * Round-6 review: a command that password-unlocks the wallet to sign must lock it again on EVERY exit. A refused
 * signature used to skip the relock, leaving the seed in memory. The refusal here is txnsign publickey:custom with a
 * key-use count at the key's capacity (getKeyUsesParam), which throws after the unlock. (This line has no wallet-key
 * capacity guard - dropped on the owner's decision of 21 Nov 2026.) A real H2 wallet in a temp folder; saving disabled.
 */
public class RelockTest {

	@Test
	public void aRefusedSignatureStillRelocks() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
		MinimaDB.getDB().setAllowSaveState(false);
		File dir = Files.createTempDirectory("relock").toFile();
		Wallet wallet = MinimaDB.getDB().getWallet();
		wallet.loadDB(new File(dir, "wallet"));
		try {
			wallet.createNewKey();
			vault.passwordLockDB("relockpass");
			assertFalse(wallet.isBaseSeedAvailable());
			
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:relocktxn");
			JSONObject res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0x00",
					"txnsign id:relocktxn publickey:custom privatekey:0x2868AE4C92C22C5CDAF223574203F2906E8AA92A17050CD99D851EC5DBB61B66"
					+ " keyuses:262144 password:relockpass").get(0);
			assertFalse(res.toString(), (boolean) res.get("status"));
			assertTrue(res.toString(), String.valueOf(res.get("error")).contains("keyuses"));
			assertFalse("the wallet was left UNLOCKED after the refused txnsign: "+res, wallet.isBaseSeedAvailable());
		}finally {
			wallet.hardCloseDB();
			MinimaDB.getDB().setAllowSaveState(true);
			for(File f : dir.listFiles()) f.delete();
			dir.delete();
		}
	}
}
