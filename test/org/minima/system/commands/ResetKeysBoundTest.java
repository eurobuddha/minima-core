package org.minima.system.commands;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.nio.file.Files;

import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.wallet.Wallet;
import org.minima.utils.json.JSONObject;

/**
 * vault action:resetkeys keyuses: sets every new wallet key's use count (2026-11-23). It stays below the capacity
 * of the keys it creates (262144 for the default 64x64x64 key) and is checked before anything is wiped. A real H2
 * wallet in a temp folder; saving disabled.
 */
public class ResetKeysBoundTest {

	@Test
	public void keyusesAtCapacityIsRefusedBeforeAnyWipe() throws Exception {
		MinimaDB.createDB();
		MinimaDB.getDB().setAllowSaveState(false);
		File dir = Files.createTempDirectory("resetkeys").toFile();
		Wallet wallet = MinimaDB.getDB().getWallet();
		wallet.loadDB(new File(dir, "wallet"));
		try {
			String key = wallet.createNewKey().getPublicKey();
			for(String uses : new String[] {"262144", "2147483647"}) {
				JSONObject res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0x00",
						"vault action:resetkeys keyuses:"+uses+" phrase:\"word word word\"").get(0);
				assertFalse(res.toString(), (boolean) res.get("status"));
				assertTrue(res.toString(), String.valueOf(res.get("error")).contains("from 0 to 262143"));
			}
			assertTrue("the wallet was wiped", wallet.getKeyFromPublic(key) != null);
		}finally {
			wallet.hardCloseDB();
			MinimaDB.getDB().setAllowSaveState(true);
			for(File f : dir.listFiles()) f.delete();
			dir.delete();
		}
	}
}
