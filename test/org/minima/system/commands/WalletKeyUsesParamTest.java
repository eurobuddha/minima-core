package org.minima.system.commands;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.JSONObject;

/**
 * OWNER MAY DROP (round-5 review 2026-11-21): a wallet-wide keyuses: is refused at or past the wallet keys'
 * capacity (262144), before the command does anything. It used to be accepted up to 2147483647, which put every
 * key past its last leaf ("vault action:resetkeys keyuses:300000", then the next spend signed leaf 0 again).
 * Runs with no Main (and a scratch data folder), so a command that got past the check stops at once.
 */
public class WalletKeyUsesParamTest {

	static final String[] COMMANDS = {
		"vault action:resetkeys phrase:x keyuses:",
		"archive action:resync host:auto keyuses:",
		"mysql action:resync keyuses:",
		"megammrsync action:resync host:x keyuses:",
		"keys action:createallkeys keyuses:"
		//restoresync: keyuses is not one of its valid params (always the default 256) - its ADD (and restore's) is
		//capped per key in Wallet.updateIncrementAllKeyUses (WalletCapacityTest)
	};
	
	@BeforeClass
	public static void setup() throws Exception {
		MinimaDB.createDB();
	}
	
	static String error(String zCommand) throws Exception {
		Field inst = Main.class.getDeclaredField("mMainInstance");
		inst.setAccessible(true);
		Object oldmain = inst.get(null);
		String olddata = GeneralParams.DATA_FOLDER;
		boolean oldmobile = GeneralParams.IS_MOBILE;
		java.io.File scratch = java.nio.file.Files.createTempDirectory("wkt").toFile();
		inst.set(null, null);
		GeneralParams.DATA_FOLDER = scratch.getAbsolutePath();
		GeneralParams.IS_MOBILE = false;		//mysql refuses on mobile before its params
		try {
			JSONObject res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0x00", zCommand).get(0);
			return String.valueOf(res.get("error"));
		}finally {
			inst.set(null, oldmain);
			GeneralParams.DATA_FOLDER = olddata;
			GeneralParams.IS_MOBILE = oldmobile;
			org.minima.utils.MiniFile.deleteFileOrFolder(scratch.getAbsolutePath(), scratch);
		}
	}
	
	@Test
	public void capacityOrMoreIsRefused() throws Exception {
		for(String cmd : COMMANDS) {
			for(String uses : new String[] {"262144", "300000", "2147483647"}) {
				String err = error(cmd+uses);
				assertTrue(cmd+uses+" : "+err, err.contains("keyuses must be a whole number from 0 to 262143"));
			}
			assertFalse(cmd+"262143", error(cmd+"262143").contains("keyuses must be"));
		}
	}
}
