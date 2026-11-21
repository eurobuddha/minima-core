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
	
	/**
	 * Round-5 review: multisigread action:spend ends with an ADMIN txnexport to the caller's file name, so a 0xFF
	 * caller could replace an admin flow's unsigned .txn with a spend paying itself. nodecount truncated one.
	 */
	@Test
	public void multisigreadSpendAndNodecountOnlyCreateNewFiles() throws Exception {
		java.io.File base = java.nio.file.Files.createTempDirectory("gate-ms").toFile();
		String oldbase = org.minima.system.params.GeneralParams.BASE_FILE_FOLDER;
		java.util.List<java.net.InetSocketAddress> oldnodes = org.minima.system.network.p2p.params.P2PParams.DEFAULT_NODE_LIST;
		org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = base.getAbsolutePath();
		org.minima.system.network.p2p.params.P2PParams.DEFAULT_NODE_LIST = new java.util.ArrayList<>();
		try {
			java.io.File handle = new java.io.File(base, "ms_pay_0123.txn");
			java.nio.file.Files.write(handle.toPath(), "ADMIN".getBytes());
			
			JSONObject res = run("0xFF", "multisigread action:spend coinid:0x01 amount:1 address:0x02 file:ms_pay_0123.txn");
			assertTrue("multisigread spend may overwrite an existing file: "+res, String.valueOf(res.get("error")).contains("only creates new files"));
			assertEquals("ADMIN", new String(java.nio.file.Files.readAllBytes(handle.toPath())));
			
			//a new name passes the gate and reaches the claim (a test tree tip holding the coin): a chain that fails
			//after the claim (txnoutput: a bad address) releases the file; a good one writes it; then it is taken
			withTestCoin("0x01", () -> {
				JSONObject r = run("0xFF", "multisigread action:spend coinid:0x01 amount:1 address:0xZZ file:ms_new_0123.txn");
				assertTrue(r.toString(), (boolean) r.get("status"));
				org.minima.utils.json.JSONArray chain = (org.minima.utils.json.JSONArray) r.get("response");
				assertFalse("the chain did not fail: "+r, (boolean) ((JSONObject) chain.get(chain.size()-1)).get("status"));
				assertFalse("the failed spend left its claimed file", new java.io.File(base, "ms_new_0123.txn").exists());
				
				r = run("0xFF", "multisigread action:spend coinid:0x01 amount:1 address:0x02 file:ms_new_0123.txn");
				chain = (org.minima.utils.json.JSONArray) r.get("response");
				assertTrue(r.toString(), (boolean) ((JSONObject) chain.get(chain.size()-1)).get("status"));
				assertTrue(new java.io.File(base, "ms_new_0123.txn").length() > 0);
				r = run("0xFF", "multisigread action:spend coinid:0x01 amount:1 address:0x02 file:ms_new_0123.txn");
				assertTrue(r.toString(), String.valueOf(r.get("error")).contains("only creates new files"));
			});
			
			//nodecount (not registered as a command, run directly): a non-admin caller never truncates a file
			Command nc = new org.minima.system.commands.network.nodecount();
			nc.getParams().put("file", "ms_pay_0123.txn");
			nc.setMiniDAPPID("0xFF");
			try {
				nc.runCommand();
				org.junit.Assert.fail("nodecount opened an existing file for a non-admin caller");
			}catch(CommandException expected) {
				assertTrue(expected.getMessage().contains("only creates new files"));
			}
			assertEquals("ADMIN", new String(java.nio.file.Files.readAllBytes(handle.toPath())));
		}finally {
			for(java.io.File f : base.listFiles()) f.delete();
			base.delete();
			org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = oldbase;
			org.minima.system.network.p2p.params.P2PParams.DEFAULT_NODE_LIST = oldnodes;
		}
	}
	
	/** Test stand-in in place of an open command: claims its file:, then fails (a write that broke half way) */
	static class ClaimThenFail extends Command {
		ClaimThenFail() {
			super("hash", "test stand-in");
		}
		@Override
		public java.util.ArrayList<String> getValidParams() {
			return new java.util.ArrayList<>(java.util.Arrays.asList("file", "data"));
		}
		@Override
		public JSONObject runCommand() throws Exception {
			getNewFileParam("file");
			throw new CommandException("the write failed");
		}
		@Override
		public Command getFunction() {
			return new ClaimThenFail();
		}
	}
	
	static java.io.File tempBase(String zName) throws Exception {
		java.io.File base = java.nio.file.Files.createTempDirectory(zName).toFile();
		org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = base.getAbsolutePath();
		return base;
	}
	
	static void dropBase(java.io.File zBase, String zOld) {
		for(java.io.File f : zBase.listFiles()) f.delete();
		zBase.delete();
		org.minima.system.params.GeneralParams.BASE_FILE_FOLDER = zOld;
	}
	
	/** Round-5 review: a failed non-admin write removes the empty file it claimed, so the retry is not refused */
	@Test
	public void aFailedNonAdminWriteReleasesItsClaim() throws Exception {
		String oldbase = org.minima.system.params.GeneralParams.BASE_FILE_FOLDER;
		java.io.File base = tempBase("gate-release");
		Command real = CommandInjectionTest.swap("hash", new ClaimThenFail());
		try {
			for(int attempt=0; attempt<2; attempt++) {
				JSONObject res = run("0xFF", "hash file:retry_0123.txn data:0x01");
				assertEquals("attempt "+attempt+": "+res, "the write failed", res.get("error"));
				assertFalse("the failed write left its claimed file", new java.io.File(base, "retry_0123.txn").exists());
			}
		}finally {
			CommandInjectionTest.swap("hash", real);
			dropBase(base, oldbase);
		}
	}
	
	/** Round-5 review: the non-admin claims of txnexport, sphincs sign and sendnosign */
	@Test
	public void nonAdminTxnexportSphincsAndSendnosignClaim() throws Exception {
		String oldbase = org.minima.system.params.GeneralParams.BASE_FILE_FOLDER;
		java.io.File base = tempBase("gate-claims");
		try {
			java.io.File handle = new java.io.File(base, "ms_pay_0123.txn");
			java.nio.file.Files.write(handle.toPath(), "ADMIN".getBytes());
			
			//txnexport run as a non-admin caller (a WRITE command on this line - the command's own check)
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:claimtxn");
			for(String name : new String[] {"ms_pay_0123.txn", "tx_new_0123.txn", "tx_new_0123.txn"}) {
				Command exp = CommandRunner.getRunner().getCommand("txnexport id:claimtxn file:"+name);
				exp.setMiniDAPPID("0xFF");
				java.io.File f = new java.io.File(base, name);
				boolean existed = f.exists();
				try {
					exp.runCommand();
					assertFalse("txnexport overwrote "+name, existed);
					assertTrue(f.length() > 0);
				}catch(CommandException refused) {
					assertTrue(name+": "+refused.getMessage(), existed && refused.getMessage().contains("only creates new files"));
				}
			}
			assertEquals("ADMIN", new String(java.nio.file.Files.readAllBytes(handle.toPath())));
			
			//sphincs sign (open): an existing name needs WRITE rights; its file param is claimed (SPHINCS signing
			//itself does not fit the test JVM's heap, so the claim is checked on the command's own param)
			JSONObject res = run("0xFF", "sphincs action:sign privatekey:0x01 data:0x01 file:ms_pay_0123.txn");
			assertTrue(res.toString(), String.valueOf(res.get("error")).contains("only creates new files"));
			oneClaimPerName("sphincs action:sign privatekey:0x01 data:0x01 file:sig_new_0123.txn");
			assertEquals("ADMIN", new String(java.nio.file.Files.readAllBytes(handle.toPath())));
			
			//sendnosign: an existing name needs WRITE rights; its file param is claimed (one caller wins a name)
			res = run("0xFF", "sendnosign address:0x01 amount:1 file:ms_pay_0123.txn");
			assertTrue(res.toString(), String.valueOf(res.get("error")).contains("only creates new files"));
			oneClaimPerName("sendnosign address:0x01 amount:1 file:sn_new_0123.txn");
			assertEquals("ADMIN", new String(java.nio.file.Files.readAllBytes(handle.toPath())));
		}finally {
			dropBase(base, oldbase);
		}
	}
	
	/** Two non-admin calls of zCommand: the first claims its file:, the second is refused it */
	static void oneClaimPerName(String zCommand) throws Exception {
		Command first = CommandRunner.getRunner().getCommand(zCommand);
		first.setMiniDAPPID("0xFF");
		Command second = CommandRunner.getRunner().getCommand(zCommand);
		second.setMiniDAPPID("0xFF");
		assertTrue(first.getNewFileParam("file").exists());
		try {
			second.getNewFileParam("file");
			org.junit.Assert.fail("two non-admin callers claimed one name: "+zCommand);
		}catch(CommandException expected) {
			assertTrue(expected.getMessage().contains("only creates new files"));
		}
	}
	
	interface Check { void run() throws Exception; }
	
	/** Runs zCheck with a test tree tip holding one unspent coin of id zCoinID (address 0x03, 10 Minima) */
	static void withTestCoin(String zCoinID, Check zCheck) throws Exception {
		boolean oldsql = org.minima.system.params.GeneralParams.USE_SQL_COINDB;		//the test tree node clears it
		org.minima.database.txpowtree.TxPowTree tree = MinimaDB.getDB().getTxPoWTree();
		Field tip = org.minima.database.txpowtree.TxPowTree.class.getDeclaredField("mTip");
		tip.setAccessible(true);
		Object oldtip = tip.get(tree);
		try {
			org.minima.database.txpowtree.TxPoWTreeNode node =
					new org.minima.database.txpowtree.TxPoWTreeNode(new org.minima.objects.TxPoW());
			node.addCoinToNode(new org.minima.objects.Coin(new org.minima.objects.base.MiniData(zCoinID),
					new org.minima.objects.base.MiniData("0x03"), new org.minima.objects.base.MiniNumber(10),
					org.minima.objects.base.MiniData.ZERO_TXPOWID));
			tip.set(tree, node);
			zCheck.run();
		}finally {
			tip.set(tree, oldtip);
			org.minima.system.params.GeneralParams.USE_SQL_COINDB = oldsql;
		}
	}
	
	/** Round-6 review: a rewrite fills the existing file in place (same file key) - never delete + re-create */
	@Test
	public void aRewriteKeepsTheSameFile() throws Exception {
		String oldbase = org.minima.system.params.GeneralParams.BASE_FILE_FOLDER;
		java.io.File base = tempBase("gate-inplace");
		try {
			java.nio.file.Path handle = new java.io.File(base, "inplace_0123.txn").toPath();
			java.nio.file.Files.write(handle, "BEFORE".getBytes());
			Object key = java.nio.file.Files.readAttributes(handle, java.nio.file.attribute.BasicFileAttributes.class).fileKey();
			CommandRunner.getRunner().runMultiCommand("0x00", "txncreate id:inplacetxn");
			assertTrue((boolean) run("0x00", "txnexport id:inplacetxn file:inplace_0123.txn").get("status"));
			assertFalse("BEFORE".equals(new String(java.nio.file.Files.readAllBytes(handle))));
			assertEquals("txnexport replaced the file instead of writing it in place", key,
					java.nio.file.Files.readAttributes(handle, java.nio.file.attribute.BasicFileAttributes.class).fileKey());
		}finally {
			dropBase(base, oldbase);
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
