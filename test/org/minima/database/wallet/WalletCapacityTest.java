package org.minima.database.wallet;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.nio.file.Files;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.objects.base.MiniData;
import org.minima.objects.keys.TreeKey;

/**
 * OWNER MAY DROP (round-5 review 2026-11-21): the node wallet's own keys never restart at leaf 0. Wallet.signData
 * used to call TreeKey.sign at any stored use count, and TreeKey.sign resets to leaf 0 at its capacity (262144
 * for the wallet's 64x64x64 keys): every signature after that re-uses leaf 0 and leaks it.
 */
public class WalletCapacityTest {

	static File sDir;
	static Wallet sWallet;
	static KeyRow sKey;
	static final MiniData DATA = new MiniData("0x00112233445566778899AABBCCDDEEFF00112233445566778899AABBCCDDEEFF");
	
	@BeforeClass
	public static void setup() throws Exception {
		sDir    = Files.createTempDirectory("wallet-capacity").toFile();
		sWallet = new Wallet();
		sWallet.loadDB(new File(sDir, "wallet"));
		sKey    = sWallet.createNewKey();
	}
	
	@AfterClass
	public static void teardown() throws Exception {
		sWallet.hardCloseDB();
		for(File f : sDir.listFiles()) f.delete();
		sDir.delete();
	}
	
	static String hex(org.minima.objects.keys.Signature zSig) {
		return MiniData.getMiniDataVersion(zSig).to0xString();
	}
	
	@Test
	public void exhaustedKeyIsRefused() throws Exception {
		for(int uses : new int[] {262144, 262145, 300000}) {
			sWallet.updateAllKeyUses(uses);
			try {
				sWallet.signData(sKey.getPublicKey(), DATA);
				fail("the wallet signed with an exhausted key at uses "+uses);
			}catch(IllegalArgumentException expected) {
				assertTrue(expected.getMessage().contains("exhausted"));
			}
			assertEquals("a refused signature counted a use", uses, sWallet.getKeyFromPublic(sKey.getPublicKey()).getUses());
		}
	}
	
	@Test
	public void inRangeSignaturesAreUnchanged() throws Exception {
		for(int uses : new int[] {0, 1, 63, 64, 4095, 4096, 262143}) {
			sWallet.updateAllKeyUses(uses);
			String got = hex(sWallet.signData(sKey.getPublicKey(), DATA));
			
			//What the base code did: the stored key at that use count, TreeKey.sign
			TreeKey tk = new TreeKey(new MiniData(sKey.getPrivateKey()), sKey.getSize(), sKey.getDepth());
			tk.setUses(uses);
			assertEquals("uses "+uses, hex(tk.sign(DATA)), got);
			assertEquals(uses+1, sWallet.getKeyFromPublic(sKey.getPublicKey()).getUses());
		}
	}
	/**
	 * restore / restoresync add 256 uses to every key (uses=uses+? in SQL, on a bigint): capped per key at its
	 * capacity, never refused - a refusal left EVERY key un-raised, re-signing leaves used since the backup.
	 */
	@Test
	public void restoreAddIsCappedPerKey() throws Exception {
		KeyRow k2 = sWallet.createNewKey();
		java.lang.reflect.Field f = org.minima.utils.SqlDB.class.getDeclaredField("mSQLConnection");
		f.setAccessible(true);
		java.sql.Connection c = (java.sql.Connection) f.get(sWallet);
		c.createStatement().execute("UPDATE keys SET uses=261950 WHERE publickey='"+sKey.getPublicKey()+"'");
		c.createStatement().execute("UPDATE keys SET uses=10 WHERE publickey='"+k2.getPublicKey()+"'");
		
		sWallet.updateIncrementAllKeyUses(256);
		assertEquals(262144, sWallet.getKeyFromPublic(sKey.getPublicKey()).getUses());
		assertEquals(266, sWallet.getKeyFromPublic(k2.getPublicKey()).getUses());
		try {
			sWallet.signData(sKey.getPublicKey(), DATA);
			fail("a key raised to its capacity signed");
		}catch(IllegalArgumentException expected) {
			assertTrue(expected.getMessage().contains("exhausted"));
		}
		sWallet.signData(k2.getPublicKey(), DATA);
		
		//a key already past its capacity comes back to it; the add never passes int range
		c.createStatement().execute("UPDATE keys SET uses=300000 WHERE publickey='"+k2.getPublicKey()+"'");
		sWallet.updateIncrementAllKeyUses(Integer.MAX_VALUE);
		assertEquals(262144, sWallet.getKeyFromPublic(sKey.getPublicKey()).getUses());
		assertEquals(262144, sWallet.getKeyFromPublic(k2.getPublicKey()).getUses());
	}
}
