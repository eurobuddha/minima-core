package org.minima.system.commands;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.utils.json.JSONObject;

/**
 * The non-admin gate (2026-11-21): commands a non-admin caller (0xFF - a non-admin companion, a read-mode RPC
 * user) must not run, because they act with the node's own keys or files.
 */
public class WriteGateTest {

	@BeforeClass
	public static void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
	}
	
	static JSONObject run(String zUser, String zCommand) {
		return (JSONObject) CommandRunner.getRunner().runMultiCommand(zUser, zCommand).get(0);
	}
	
	static boolean adminRefusal(JSONObject zRes) {
		return String.valueOf(zRes.get("error")).contains("ADMIN");
	}
	
	@Test
	public void txnautoNeedsAdmin() {
		//sign:true signs with the node wallet - with txnpost (open) that spends the node's coins
		JSONObject res = run("0xFF", "txnauto id:gatetxn amount:1 address:0x00 sign:true");
		assertTrue("a non-admin caller ran txnauto: "+res, adminRefusal(res));
		assertFalse(MinimaDB.getDB().getCustomTxnDB().getTransactionRow("gatetxn") != null);
		
		//ADMIN still runs it (it fails later here: no wallet in the test)
		assertFalse(adminRefusal(run("0x00", "txnauto id:gatetxn2 amount:1 address:0x00")));
	}
	
	@Test
	public void keetsFileNeedsAdmin() {
		boolean keets = org.minima.system.params.GeneralParams.KEETS_ENABLED;
		org.minima.system.params.GeneralParams.KEETS_ENABLED = true;
		try {
			keetsFileGate();
		}finally {
			org.minima.system.params.GeneralParams.KEETS_ENABLED = keets;
		}
	}
	
	void keetsFileGate() {
		JSONObject res = run("0xFF", "keets action:publish path:/x file:/etc/hosts");
		assertTrue("a non-admin caller used keets file: "+res, String.valueOf(res.get("error")).contains("file: needs ADMIN"));
		res = run("0xFF", "keets action:fetch url:x file:/tmp/keets-gate-test");
		assertTrue(String.valueOf(res.get("error")).contains("file: needs ADMIN"));
		
		//ADMIN passes the gate (it fails later here: no keets sidecar in the test)
		assertFalse(String.valueOf(run("0x00", "keets action:publish path:/x file:/etc/hosts").get("error")).contains("file: needs ADMIN"));
	}
	
	@Test
	public void txnpostBurnNeedsAdmin() {
		CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:burntxn");
		JSONObject res = run("0xFF", "txnpost id:burntxn burn:1");
		assertTrue("a non-admin caller asked the node to burn its own coins: "+res,
				String.valueOf(res.get("error")).contains("burn: needs ADMIN"));
		
		//without burn, and for ADMIN, the gate does not stand in the way (txnpost fails later here: no chain)
		assertFalse(String.valueOf(run("0xFF", "txnpost id:burntxn").get("error")).contains("needs ADMIN"));
		assertFalse(String.valueOf(run("0x00", "txnpost id:burntxn burn:1").get("error")).contains("needs ADMIN"));
	}
	
	/** A TxnRow as the Trading Account (VirtualWallet.hex) serialises it for postfrom data: */
	static String blob(String zID) throws Exception {
		org.minima.database.userprefs.txndb.TxnRow row = new org.minima.database.userprefs.txndb.TxnRow(
				zID, new org.minima.objects.Transaction(), new org.minima.objects.Witness());
		java.io.ByteArrayOutputStream bo = new java.io.ByteArrayOutputStream();
		java.io.DataOutputStream d = new java.io.DataOutputStream(bo);
		row.writeDataStream(d);
		d.flush();
		return new org.minima.objects.base.MiniData(bo.toByteArray()).to0xString();
	}
	
	@Test
	public void postfromCannotOverwriteAnotherDraft() throws Exception {
		CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:victimdraft");
		CommandRunner.getRunner().runMultiCommand("0x00", "txnstate id:victimdraft port:1 value:42");
		
		for(String cmd : new String[] {"postfrom", "signfrom"}) {
			String extra = cmd.equals("signfrom") ? " privatekey:0x01 keyuses:1" : " mine:false";
			JSONObject res = run("0xFF", cmd+" data:"+blob("victimdraft")+extra);
			assertTrue(cmd+" overwrote another draft: "+res, String.valueOf(res.get("error")).contains("already exists"));
			org.minima.database.userprefs.txndb.TxnRow row = MinimaDB.getDB().getCustomTxnDB().getTransactionRow("victimdraft");
			assertTrue(cmd+" removed the victim's draft", row != null);
			assertTrue(cmd+" replaced the victim's draft", row.getTransaction().getStateValue(1) != null);
		}
	}
	
	@Test
	public void tradingAccountPostfromStillPasses() throws Exception {
		//VirtualWallet.post: postfrom data:<fresh id> mine:false, as a non-admin companion
		JSONObject res = run("0xFF", "postfrom data:"+blob("dx0011223344556677")+" mine:false");
		assertFalse(String.valueOf(res.get("error")).contains("already exists"));
		assertFalse(adminRefusal(res));
	}
	
	static boolean confined(JSONObject zRes) {
		return String.valueOf(zRes.get("error")).contains("only use files in the node's base folder");
	}
	
	@Test
	public void nonAdminFilesStayInTheBaseFolder() throws Exception {
		//outside the base folder (no -basefolder in the test: the working folder)
		assertTrue(confined(run("0xFF", "sendview file:/etc/hosts")));
		assertTrue(confined(run("0xFF", "sendpost file:/etc/hosts")));
		assertTrue(confined(run("0xFF", "txnimport id:gateimport file:/etc/hosts")));
		assertTrue(confined(run("0xFF", "multisigread action:view file:../../../../../../etc/hosts")));
		assertTrue(confined(run("0xFF", "multisigread action:post file:/etc/hosts")));
		
		//inside it, and for ADMIN anywhere, as before
		java.io.File basedir = java.nio.file.Files.createTempDirectory("gate-base").toFile();
		String oldbase = org.minima.system.params.GeneralParams.BASE_FILE_FOLDER;
		org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = basedir.getAbsolutePath();
		java.io.File inbase = new java.io.File(basedir, "gate-test-"+System.nanoTime()+".txn");
		try {
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:gatefile");
			assertTrue((boolean) run("0x00", "txnexport id:gatefile file:"+inbase.getName()).get("status"));
			JSONObject res = run("0xFF", "txnimport id:gateimport1 file:"+inbase.getName());
			assertTrue(res.toString(), (boolean) res.get("status"));
			res = run("0x00", "txnimport id:gateimport2 file:"+inbase.getAbsolutePath());
			assertTrue(res.toString(), (boolean) res.get("status"));
		}finally {
			inbase.delete();
			basedir.delete();
			org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = oldbase;
		}
	}
	
	@Test
	public void everyNonAdminFileParamIsConfined() throws Exception {
		//A file outside the base folder that must survive (the reviewer's probe: peers publish truncated it)
		java.io.File outside = java.io.File.createTempFile("gate-outside", ".txt");
		java.nio.file.Files.write(outside.toPath(), "keep".getBytes());
		try {
			String path = outside.getAbsolutePath();
			for(String cmd : new String[] {"peers action:publish", "txnview", "hash"}) {
				JSONObject res = run("0xFF", cmd+" file:"+path);
				assertTrue(cmd+": "+res, confined(res));
			}
			assertTrue("the file outside the base folder was changed",
					"keep".equals(new String(java.nio.file.Files.readAllBytes(outside.toPath()))));
		}finally {
			outside.delete();
		}
	}
	
	/** The reviewer's probes (third review): base folder = the node's data folder, as in every shipped product */
	@Test
	public void dataFolderFilesAreUnreachable() throws Exception {
		java.io.File base = java.nio.file.Files.createTempDirectory("gate-data").toFile();
		java.io.File dbdir = new java.io.File(base, "1.1/databases");
		dbdir.mkdirs();
		java.io.File wallet = new java.io.File(dbdir, "wallet.mv.db");
		java.nio.file.Files.write(wallet.toPath(), "WALLET".getBytes());
		java.io.File token = new java.io.File(base, "gateway-token.txt");
		java.nio.file.Files.write(token.toPath(), "TOKEN".getBytes());
		String oldbase = org.minima.system.params.GeneralParams.BASE_FILE_FOLDER;
		org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = base.getAbsolutePath();
		try {
			for(String cmd : new String[] {"peers action:publish file:", "sendview file:", "txnview file:", "hash file:"}) {
				assertTrue(cmd+" wallet db", confined(run("0xFF", cmd+wallet.getAbsolutePath())));
				assertTrue(cmd+" relative", confined(run("0xFF", cmd+"1.1/databases/wallet.mv.db")));
				assertTrue(cmd+" token", confined(run("0xFF", cmd+"gateway-token.txt")));
				assertTrue(cmd+" dotdot", confined(run("0xFF", cmd+"..txn")));
			}
			assertEquals("WALLET", new String(java.nio.file.Files.readAllBytes(wallet.toPath())));
			assertEquals("TOKEN", new String(java.nio.file.Files.readAllBytes(token.toPath())));
			
			//a .txn that is a folder
			new java.io.File(base, "dir.txn").mkdirs();
			assertTrue(confined(run("0xFF", "sendview file:dir.txn")));
			
			//base folder at the filesystem root
			org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = "/";
			assertTrue(confined(run("0xFF", "sendview file:x.txn")));
			//no base folder
			org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = "";
			assertTrue(confined(run("0xFF", "sendview file:x.txn")));
		}finally {
			org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = oldbase;
		}
	}
	
	/**
	 * The shipped file flows (merchshop PaymentFlow, merchinbox PlainDeliveryFlow, NFTwallet PreparedSend):
	 * sendnosign file:handle.txn -> sendview file:handle.txn -> sendsign file:handle.txn ->
	 * sendview / sendpost file:signedtransaction-N.txn, and a retry re-writes handle.txn. The same plain name
	 * resolves to the same file for a 0xFF caller and an ADMIN caller, so a flow that mixes them still meets.
	 */
	@Test
	public void shippedFlowNamesResolveTheSameForBoth() throws Exception {
		java.io.File base = java.nio.file.Files.createTempDirectory("gate-flow").toFile();
		String oldbase = org.minima.system.params.GeneralParams.BASE_FILE_FOLDER;
		org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = base.getAbsolutePath();
		try {
			for(String name : new String[] {"pay-0123abcd.txn", "signedtransaction-1763700000000.txn", "multispend_1763700000000.txn"}) {
				Command nonadmin = CommandRunner.getRunner().getCommand("sendview file:"+name);
				nonadmin.setMiniDAPPID("0xFF");
				Command admin = CommandRunner.getRunner().getCommand("sendview file:"+name);
				admin.setMiniDAPPID("0x00");
				assertEquals(admin.getFileParam("file").getCanonicalPath(), nonadmin.getFileParam("file").getCanonicalPath());
				assertEquals(new java.io.File(base, name).getCanonicalPath(), nonadmin.getFileParam("file").getCanonicalPath());
			}
			
			//write, read back, re-write (retry) as 0xFF: a txn exported by ADMIN under a plain name, imported twice
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:flowtxn");
			assertTrue((boolean) run("0x00", "txnexport id:flowtxn file:pay-0123abcd.txn").get("status"));
			assertTrue((boolean) run("0xFF", "txnview file:pay-0123abcd.txn").get("status"));
			assertTrue((boolean) run("0x00", "txnexport id:flowtxn file:pay-0123abcd.txn").get("status"));
			assertTrue((boolean) run("0xFF", "txnview file:pay-0123abcd.txn").get("status"));
		}finally {
			for(java.io.File f : base.listFiles()) f.delete();
			base.delete();
			org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = oldbase;
		}
	}
	
	@Test
	public void nonAdminWritesOnlyCreateNewFiles() throws Exception {
		java.io.File base = java.nio.file.Files.createTempDirectory("gate-new").toFile();
		String oldbase = org.minima.system.params.GeneralParams.BASE_FILE_FOLDER;
		org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = base.getAbsolutePath();
		try {
			//An ADMIN flow's file (the unsigned handle.txn between its sendview and its sendsign)
			java.io.File handle = new java.io.File(base, "ms_pay_0123.txn");
			java.nio.file.Files.write(handle.toPath(), "ADMIN".getBytes());
			JSONObject res = run("0xFF", "peers action:publish file:ms_pay_0123.txn");
			assertTrue("a non-admin caller overwrote an existing file: "+res, String.valueOf(res.get("error")).contains("only creates new files"));
			assertEquals("ADMIN", new String(java.nio.file.Files.readAllBytes(handle.toPath())));
			
			//a NEW plain name: created
			assertTrue(run("0xFF", "peers action:publish file:mine-new.txn").toString(), (boolean) run("0xFF", "peers action:publish file:mine-new2.txn").get("status"));
			
			//ADMIN rewrites, as the shipped retry does (miniMall / NFT: the same handle.txn again)
			assertTrue((boolean) run("0x00", "peers action:publish file:ms_pay_0123.txn").get("status"));
			assertTrue((boolean) run("0x00", "peers action:publish file:ms_pay_0123.txn").get("status"));
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:retrytxn");
			assertTrue((boolean) run("0x00", "txnexport id:retrytxn file:nft_send_0123.txn").get("status"));
			assertTrue((boolean) run("0x00", "txnexport id:retrytxn file:nft_send_0123.txn").get("status"));
		}finally {
			for(java.io.File f : base.listFiles()) f.delete();
			base.delete();
			org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = oldbase;
		}
	}
	
	@Test
	public void nonAdminTxnimportNeverReplacesARowCreatedAfterTheGate() throws Exception {
		//The runner's "new id only" check passed, then another caller created the row: the add must not replace it
		Command cmd = CommandRunner.getRunner().getCommand("txnimport id:latecomer data:"+blob("whatever"));
		cmd.setMiniDAPPID("0xFF");
		CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:latecomer");
		CommandRunner.getRunner().runMultiCommand("0x00", "txnstate id:latecomer port:1 value:7");
		try {
			cmd.runCommand();
			org.junit.Assert.fail("the non-admin txnimport replaced the row");
		}catch(CommandException expected) {
			assertTrue(expected.getMessage().contains("already exists"));
		}
		assertTrue(MinimaDB.getDB().getCustomTxnDB().getTransactionRow("latecomer").getTransaction().getStateValue(1) != null);
	}
	
	@Test
	public void windowsDeviceNamesAreNotFiles() {
		for(String name : new String[] {"CON.txn", "con.txn", "NUL.txn", "aux.txn", "PRN.txn", "COM1.txn", "lpt9.txn", "com3.backup.txn"}) {
			assertFalse(name, Command.isPlainTxnName(name));
		}
		assertTrue(Command.isPlainTxnName("console.txn"));
		assertTrue(Command.isPlainTxnName("ms_pay_0123.txn"));
	}
}
