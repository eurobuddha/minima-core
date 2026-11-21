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
			assertTrue(res.toString(), String.valueOf(res.get("error")).contains("keyuses must be a whole number"));
		}
	}
	
	@Test
	public void smallCountSignsExactlyAsBefore() throws Exception {
		JSONObject res = sign("small", "5");
		assertTrue(res.toString(), (boolean) res.get("status"));
		
		//What the base code did: createDefault, setUses(5), sign the transaction id
		org.minima.database.userprefs.txndb.TxnRow row = MinimaDB.getDB().getCustomTxnDB().getTransactionRow("small");
		MiniData txid = row.getTransaction().getTransactionID();
		TreeKey key = TreeKey.createDefault(new MiniData(PRIV));
		key.setUses(5);
		Signature expected = key.sign(txid);
		Signature got = row.getWitness().getAllSignatures().get(0);
		assertEquals(MiniData.getMiniDataVersion(expected).to0xString(), MiniData.getMiniDataVersion(got).to0xString());
	}
}
