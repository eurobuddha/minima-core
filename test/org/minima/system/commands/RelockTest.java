package org.minima.system.commands;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.lang.reflect.Field;
import java.nio.file.Files;

import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.database.wallet.KeyRow;
import org.minima.database.wallet.Wallet;
import org.minima.system.commands.backup.vault;
import org.minima.utils.json.JSONObject;

/**
 * Round-6 review: a command that password-unlocks the wallet to sign must lock it again on EVERY exit. A refused
 * signature (an exhausted key - Wallet.signData's capacity guard) used to skip the relock, leaving the seed in memory.
 * A real H2 wallet in a temp folder; saving disabled.
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
			KeyRow key = wallet.createNewKey();
			wallet.updateAllKeyUses(key.getMaxUses());		//exhausted
			vault.passwordLockDB("relockpass");
			assertFalse(wallet.isBaseSeedAvailable());
			
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:relocktxn");
			JSONObject res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0x00",
					"txnsign id:relocktxn publickey:"+key.getPublicKey()+" password:relockpass").get(0);
			
			//refused where the wallet guards its keys' capacity; either way the wallet must be locked again
			assertFalse("the wallet was left UNLOCKED after txnsign: "+res, wallet.isBaseSeedAvailable());
			if(!(boolean) res.get("status")) {
				assertTrue(res.toString(), String.valueOf(res.get("error")).contains("exhausted"));
			}
		}finally {
			wallet.hardCloseDB();
			MinimaDB.getDB().setAllowSaveState(true);
			for(File f : dir.listFiles()) f.delete();
			dir.delete();
		}
	}
}
