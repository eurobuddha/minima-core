package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.objects.base.MiniData;
import org.minima.objects.keys.Signature;
import org.minima.objects.keys.TreeKey;
import org.minima.utils.json.JSONObject;

/**
 * txnsign publickey:custom keyuses: (2026-11-21). keyuses is the index of the NEXT one-time leaf: 0 .. 262143 for
 * the default 64x64x64 key. 262144 used to restart at leaf 0 (TreeKey.sign) and 4294967299 wrapped to 3
 * (MiniNumber.getAsInt keeps the low 32 bits): both re-sign with a used Winternitz leaf and leak the key.
 */
public class KeyUsesTest {

	static final String PRIV = "0x2868AE4C92C22C5CDAF223574203F2906E8AA92A17050CD99D851EC5DBB61B66";
	
	@BeforeClass
	public static void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
	}
	
	static JSONObject sign(String zID, String zUses) {
		CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:"+zID);
		return (JSONObject) CommandRunner.getRunner().runMultiCommand("0x00",
				"txnsign id:"+zID+" publickey:custom privatekey:"+PRIV+" keyuses:"+zUses).get(0);
	}
	
	@Test
	public void lastLeafSigns() {
		JSONObject res = sign("lastleaf", "262143");
		assertTrue(res.toString(), (boolean) res.get("status"));
	}
	
	@Test
	public void wrappedOrResetCountsAreRefused() {
		for(String uses : new String[] {"262144", "1000000", "4294967299", "-1", "1.5"}) {
			JSONObject res = sign("bad"+uses.replace("-","m").replace(".","p"), uses);
			assertFalse("keyuses:"+uses+" signed: "+res, (boolean) res.get("status"));
			String error = String.valueOf(res.get("error"));
			//this line already refused these (rex3 hardening): its own messages
			assertTrue(res.toString(), error.contains("keyuses must be a whole number") || error.contains("keyuses outside key capacity"));
		}
	}
	
	/** In range, the signature is byte-identical to the base code's, across the 64x64x64 tree's level boundaries */
	@Test
	public void inRangeCountsSignExactlyAsBefore() throws Exception {
		for(int uses : new int[] {0, 1, 5, 63, 64, 4095, 4096, 262143}) {
			JSONObject res = sign("inrange"+uses, Integer.toString(uses));
			assertTrue(res.toString(), (boolean) res.get("status"));
			
			//What the base code did: createDefault, setUses(n), sign the transaction id
			org.minima.database.userprefs.txndb.TxnRow row = MinimaDB.getDB().getCustomTxnDB().getTransactionRow("inrange"+uses);
			MiniData txid = row.getTransaction().getTransactionID();
			TreeKey key = TreeKey.createDefault(new MiniData(PRIV));
			key.setUses(uses);
			Signature expected = key.sign(txid);
			Signature got = row.getWitness().getAllSignatures().get(0);
			assertEquals("keyuses:"+uses, MiniData.getMiniDataVersion(expected).to0xString(), MiniData.getMiniDataVersion(got).to0xString());
		}
	}
	static String sig(String zID) {
		return MiniData.getMiniDataVersion(MinimaDB.getDB().getCustomTxnDB().getTransactionRow(zID)
				.getWitness().getAllSignatures().get(0)).to0xString();
	}
	
	static String expected(String zID, int zUses) {
		TreeKey key = new TreeKey(new MiniData(PRIV), 128, 4);
		key.setUses(zUses);
		return MiniData.getMiniDataVersion(key.sign(MinimaDB.getDB().getCustomTxnDB().getTransactionRow(zID)
				.getTransaction().getTransactionID())).to0xString();
	}
	
	/**
	 * Round-5 review: with -blockaskeyuses this line's createDefault is a 128x128x128x128 key (268435456 leaves), the
	 * leaf is max(chain tip, last used + 1, keyuses + 1), and this line's own validation applies. Pinned here.
	 */
	@Test
	public void blockModeUsesTheBlockKeyShape() throws Exception {
		boolean oldmode = org.minima.system.params.GeneralParams.USE_BLOCK_AS_KEYUSES;
		boolean oldsql  = org.minima.system.params.GeneralParams.USE_SQL_COINDB;		//the test tree node clears it
		org.minima.database.txpowtree.TxPowTree tree = MinimaDB.getDB().getTxPoWTree();
		Field tip = org.minima.database.txpowtree.TxPowTree.class.getDeclaredField("mTip");
		tip.setAccessible(true);
		Object oldtip = tip.get(tree);
		org.minima.objects.base.MiniNumber oldlast = MinimaDB.getDB().getUserDB().getLastBlockAsKeyUses();
		MinimaDB.getDB().setAllowSaveState(false);
		try {
			org.minima.system.params.GeneralParams.USE_BLOCK_AS_KEYUSES = true;
			assertEquals(268435456, TreeKey.createDefault(new MiniData(PRIV)).getMaxUses());
			MinimaDB.getDB().getUserDB().setLastBlockAsKeyUses(org.minima.objects.base.MiniNumber.ZERO);
			org.minima.objects.TxPoW txpow = new org.minima.objects.TxPoW();
			txpow.setBlockNumber(new org.minima.objects.base.MiniNumber(2417237));
			tip.set(tree, new org.minima.database.txpowtree.TxPoWTreeNode(txpow));
			
			//no keyuses: the chain tip; again: the next leaf, never the same one
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:blk1");
			JSONObject res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0x00",
					"txnsign id:blk1 publickey:custom privatekey:"+PRIV).get(0);
			assertTrue(res.toString(), (boolean) res.get("status"));
			assertEquals(expected("blk1", 2417237), sig("blk1"));
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:blk2");
			res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0x00",
					"txnsign id:blk2 publickey:custom privatekey:"+PRIV).get(0);
			assertTrue(res.toString(), (boolean) res.get("status"));
			assertEquals(expected("blk2", 2417238), sig("blk2"));
			
			//262144 is in range for this key shape (it is past a legacy key's end); the higher of it + 1 and the tip
			res = sign("blk3", "262144");
			assertTrue(res.toString(), (boolean) res.get("status"));
			assertEquals(expected("blk3", 2417239), sig("blk3"));
			res = sign("blk4", "3000000");
			assertTrue(res.toString(), (boolean) res.get("status"));
			assertEquals(expected("blk4", 3000001), sig("blk4"));
			
			//this line's own validation, against the 128x4 capacity
			for(String uses : new String[] {"268435456", "4294967299", "-1", "1.5"}) {
				res = sign("blkbad"+uses.replace("-","m").replace(".","p"), uses);
				assertFalse("keyuses:"+uses+" signed: "+res, (boolean) res.get("status"));
				String error = String.valueOf(res.get("error"));
				assertTrue(res.toString(), error.contains("keyuses must be a whole number") || error.contains("keyuses outside key capacity"));
			}
			
			//a chain tip at the capacity: refused, never leaf 0
			txpow.setBlockNumber(new org.minima.objects.base.MiniNumber(268435456));
			tip.set(tree, new org.minima.database.txpowtree.TxPoWTreeNode(txpow));
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:blkcap");
			res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0x00",
					"txnsign id:blkcap publickey:custom privatekey:"+PRIV).get(0);
			assertFalse(res.toString(), (boolean) res.get("status"));
			assertTrue(res.toString(), String.valueOf(res.get("error")).contains("exhausted"));
		}finally {
			org.minima.system.params.GeneralParams.USE_BLOCK_AS_KEYUSES = oldmode;
			org.minima.system.params.GeneralParams.USE_SQL_COINDB = oldsql;
			tip.set(tree, oldtip);
			MinimaDB.getDB().getUserDB().setLastBlockAsKeyUses(oldlast);
			MinimaDB.getDB().setAllowSaveState(true);
		}
	}
}
