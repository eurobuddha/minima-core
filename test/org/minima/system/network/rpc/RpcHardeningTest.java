package org.minima.system.network.rpc;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.Base64;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.system.Main;
import org.minima.system.commands.CommandRunner;
import org.minima.system.network.NetworkManager;
import org.minima.system.params.GeneralParams;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;

/**
 * RPC hardening (2026-11-21). No node is started: Main and NetworkManager are allocated without their
 * constructors, so the rpc command can stop / start only the RPC server, on a free loopback port.
 * Uses only APIs the unfixed code also has, so these tests prove the defects there.
 */
public class RpcHardeningTest {

	static sun.misc.Unsafe unsafe() throws Exception {
		Field f = sun.misc.Unsafe.class.getDeclaredField("theUnsafe");
		f.setAccessible(true);
		return (sun.misc.Unsafe) f.get(null);
	}

	static int freePort() throws Exception {
		try(ServerSocket ss = new ServerSocket(0)) {
			return ss.getLocalPort();
		}
	}

	NetworkManager network;
	Field mainInstance;

	@Before
	public void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());

		Main main = (Main) unsafe().allocateInstance(Main.class);
		network   = (NetworkManager) unsafe().allocateInstance(NetworkManager.class);
		Field net = Main.class.getDeclaredField("mNetwork");
		net.setAccessible(true);
		net.set(main, network);
		mainInstance = Main.class.getDeclaredField("mMainInstance");
		mainInstance.setAccessible(true);
		mainInstance.set(null, main);

		GeneralParams.RPC_PORT 			= freePort();
		GeneralParams.RPC_BIND_HOST 	= "127.0.0.1";
		GeneralParams.RPC_ENABLED 		= false;
		GeneralParams.RPC_AUTHENTICATE 	= false;
		GeneralParams.RPC_PASSWORD 		= "none";
	}

	@After
	public void teardown() throws Exception {
		network.stopRPC();
		mainInstance.set(null, null);
		GeneralParams.RPC_ENABLED 		= false;
		GeneralParams.RPC_AUTHENTICATE 	= false;
		GeneralParams.RPC_PASSWORD 		= "none";
		GeneralParams.RPC_BIND_HOST 	= "127.0.0.1";
	}

	static JSONObject run(String zCommand) {
		JSONArray res = CommandRunner.getRunner().runMultiCommand(zCommand);
		return (JSONObject) res.get(0);
	}

	@Test
	public void enableWithoutPasswordKeepsThePassword() {
		//As if started with -rpcenable -rpcpassword secret
		GeneralParams.RPC_ENABLED 		= true;
		GeneralParams.RPC_AUTHENTICATE 	= true;
		GeneralParams.RPC_PASSWORD 		= "secret";

		run("rpc enable:false");
		assertTrue("rpc enable:false switched authentication off", GeneralParams.RPC_AUTHENTICATE);

		JSONObject res = run("rpc enable:true");
		assertTrue((boolean) res.get("status"));
		assertTrue("rpc enable:true switched authentication off", GeneralParams.RPC_AUTHENTICATE);
		assertEquals("secret", GeneralParams.RPC_PASSWORD);
	}

	@Test
	public void firstEnableNeedsAPassword() {
		JSONObject res = run("rpc enable:true");
		assertFalse("RPC was enabled with no password", (boolean) res.get("status"));
		assertFalse(GeneralParams.RPC_ENABLED);

		res = run("rpc enable:true password:first");
		assertTrue((boolean) res.get("status"));
		assertTrue(GeneralParams.RPC_AUTHENTICATE);
		assertEquals("first", GeneralParams.RPC_PASSWORD);
	}
	
	@Test
	public void rpcServerBindsLoopbackByDefault() throws Exception {
		network.startRPC();
		Field f = NetworkManager.class.getDeclaredField("mRPCServer");
		f.setAccessible(true);
		HTTPServer server = (HTTPServer) f.get(network);
		for(int i=0;i<50 && server.mServerSocket == null;i++) {
			Thread.sleep(100);
		}
		assertTrue("the RPC listens on "+server.mServerSocket.getInetAddress()+", not loopback",
				server.mServerSocket.getInetAddress().isLoopbackAddress());
	}
	
	/** One POST through a real CMDHandler on a loopback socket pair; returns the whole HTTP reply */
	static String post(String zUser, String zPassword, String zCommand) throws Exception {
		try(ServerSocket ss = new ServerSocket(0, 5, InetAddress.getLoopbackAddress());
			Socket client = new Socket(InetAddress.getLoopbackAddress(), ss.getLocalPort())) {
			Thread handler = new Thread(new CMDHandler(ss.accept()));
			handler.start();
			
			byte[] body = zCommand.getBytes(StandardCharsets.UTF_8);
			String auth = Base64.getEncoder().encodeToString((zUser+":"+zPassword).getBytes(StandardCharsets.UTF_8));
			OutputStream out = client.getOutputStream();
			out.write(("POST / HTTP/1.1\r\nAuthorization: Basic "+auth+"\r\nContent-Length: "+body.length+"\r\n\r\n")
					.getBytes(StandardCharsets.US_ASCII));
			out.write(body);
			out.flush();
			
			InputStream in = client.getInputStream();
			ByteArrayOutputStream reply = new ByteArrayOutputStream();
			byte[] buf = new byte[4096];
			int len;
			while((len = in.read(buf)) != -1) {
				reply.write(buf, 0, len);
			}
			handler.join(10000);
			return new String(reply.toByteArray(), StandardCharsets.UTF_8);
		}
	}
	
	@Test
	public void readUserCannotRunWriteCommands() throws Exception {
		GeneralParams.RPC_AUTHENTICATE 	= true;
		GeneralParams.RPC_PASSWORD 		= "secret";
		JSONArray users = new JSONArray();
		JSONObject reader = new JSONObject();
		reader.put("username", "reader");
		reader.put("password", "readpw");
		reader.put("mode", "read");
		users.add(reader);
		MinimaDB.getDB().getUserDB().setRPCUsers(users);
		
		String reply = post("reader", "readpw", "txncreate id:readertxn");
		assertFalse("a read RPC user ran a WRITE command: "+reply,
				MinimaDB.getDB().getCustomTxnDB().getTransactionRow("readertxn") != null);
		
		//A read command still works for the read user, and the minima user still writes
		assertTrue(post("reader", "readpw", "random").contains("\"status\":true"));
		post("minima", "secret", "txncreate id:writertxn");
		assertTrue(MinimaDB.getDB().getCustomTxnDB().getTransactionRow("writertxn") != null);
	}
	
	/** Raw request through a real CMDHandler: the given head, the body, then the client stops sending */
	static String raw(String zHead, String zBody) throws Exception {
		try(ServerSocket ss = new ServerSocket(0, 5, InetAddress.getLoopbackAddress());
			Socket client = new Socket(InetAddress.getLoopbackAddress(), ss.getLocalPort())) {
			Thread handler = new Thread(new CMDHandler(ss.accept()));
			handler.start();
			OutputStream out = client.getOutputStream();
			out.write((zHead+"\r\n\r\n"+zBody).getBytes(StandardCharsets.UTF_8));
			out.flush();
			client.shutdownOutput();
			client.setSoTimeout(20000);
			InputStream in = client.getInputStream();
			ByteArrayOutputStream reply = new ByteArrayOutputStream();
			byte[] buf = new byte[4096];
			int len;
			try {
				while((len = in.read(buf)) != -1) {
					reply.write(buf, 0, len);
				}
			}catch(java.net.SocketException closed) {
				//the handler died and the socket was reset
			}
			handler.join(20000);
			return new String(reply.toByteArray(), StandardCharsets.UTF_8);
		}
	}
	
	@Test
	public void hugeContentLengthIsRefusedNotAllocated() throws Exception {
		//2 GB: the old handler did new char[2000000000] - an OutOfMemoryError nothing caught
		String reply = raw("POST / HTTP/1.1\r\nContent-Length: 2000000000", "");
		assertTrue("no refusal for a 2 GB Content-Length: '"+reply+"'", reply.startsWith("HTTP/1.1 413"));
	}
	
	@Test
	public void negativeAndShortContentLengthAsBefore() throws Exception {
		//negative: the handler fails and closes without a reply, as it always did
		assertEquals("", raw("POST / HTTP/1.1\r\nContent-Length: -5", ""));
		//longer than the body that arrives: what arrived runs
		assertTrue(raw("POST / HTTP/1.1\r\nContent-Length: 50", "random").contains("\"command\":\"random\""));
	}
	
	@Test
	public void readUserCannotQuitTheNode() throws Exception {
		//The old handler set its halt flag for "quit" before the read-mode gate: the JVM (this test) would die
		GeneralParams.RPC_AUTHENTICATE 	= true;
		GeneralParams.RPC_PASSWORD 		= "secret";
		JSONArray users = new JSONArray();
		JSONObject reader = new JSONObject();
		reader.put("username", "reader");
		reader.put("password", "readpw");
		reader.put("mode", "read");
		users.add(reader);
		MinimaDB.getDB().getUserDB().setRPCUsers(users);
		
		//Catch the halt instead of dying (JDK 11 still honours a SecurityManager for Runtime.halt)
		final java.util.concurrent.atomic.AtomicBoolean halted = new java.util.concurrent.atomic.AtomicBoolean();
		SecurityManager old = System.getSecurityManager();
		System.setSecurityManager(new SecurityManager() {
			@Override public void checkPermission(java.security.Permission p) { }
			@Override public void checkPermission(java.security.Permission p, Object c) { }
			@Override public void checkExit(int status) {
				halted.set(true);
				throw new SecurityException("halt blocked by the test");
			}
		});
		try {
			String reply = post("reader", "readpw", "quit");
			assertTrue(reply, reply.contains("You need ADMIN privileges"));
			Thread.sleep(500);
			assertFalse("a read user's refused quit halted the node", halted.get());
		}finally {
			System.setSecurityManager(old);
		}
	}
	
	@Test
	public void rpcThatCannotListenIsReported() {
		//192.0.2.1 (TEST-NET) is no address of this machine: the bind fails and the RPC stays down
		GeneralParams.RPC_BIND_HOST = "192.0.2.1";
		JSONObject res = run("rpc enable:true password:firstpw");
		JSONObject rpc = (JSONObject) res.get("response");
		assertEquals("the reply did not say the RPC is down: "+rpc, false, rpc.get("listening"));
		assertTrue(String.valueOf(rpc.get("error")).contains("Cannot listen"));
	}
	
	@Test
	public void blankPasswordChangesNothing() throws Exception {
		GeneralParams.RPC_ENABLED 		= true;
		GeneralParams.RPC_AUTHENTICATE 	= true;
		GeneralParams.RPC_PASSWORD 		= "secret";
		network.startRPC();
		
		JSONObject res = run("rpc enable:true password:\"\"");
		assertFalse("a blank password was accepted", (boolean) res.get("status"));
		assertEquals("secret", GeneralParams.RPC_PASSWORD);
		Field f = NetworkManager.class.getDeclaredField("mRPCServer");
		f.setAccessible(true);
		assertTrue("the refused command stopped the running RPC", f.get(network) != null);
	}
	
	@Test
	public void openRpcIsNamedInTheRefusal() {
		//started with -rpcenable and no -rpcpassword
		GeneralParams.RPC_ENABLED = true;
		JSONObject res = run("rpc enable:true");
		assertFalse((boolean) res.get("status"));
		assertTrue(String.valueOf(res.get("error")), String.valueOf(res.get("error")).contains("WITHOUT a password"));
	}
	
	@Test
	public void foreignHostIsRefused() throws Exception {
		//DNS rebinding: a page on evil.example rebinds its name to 127.0.0.1 - the browser sends Host: evil.example
		String reply = raw("POST / HTTP/1.1\r\nHost: evil.example:9005\r\nContent-Length: 20", "txncreate id:rebindx");
		assertTrue("a request for Host evil.example ran: "+reply, reply.startsWith("HTTP/1.1 403"));
		assertFalse(MinimaDB.getDB().getCustomTxnDB().getTransactionRow("rebindx") != null);
		
		//this machine's names and IP literals, and no Host at all, are served as before
		assertTrue(raw("POST / HTTP/1.1\r\nHost: 127.0.0.1:9005\r\nContent-Length: 6", "random").contains("\"status\":true"));
		assertTrue(raw("POST / HTTP/1.1\r\nHost: localhost:9005\r\nContent-Length: 6", "random").contains("\"status\":true"));
		assertTrue(raw("POST / HTTP/1.1\r\nHost: 192.168.1.20:9005\r\nContent-Length: 6", "random").contains("\"status\":true"));
		assertTrue(raw("POST / HTTP/1.1\r\nHost: [::1]:9005\r\nContent-Length: 6", "random").contains("\"status\":true"));
		assertTrue(raw("POST / HTTP/1.1\r\nContent-Length: 6", "random").contains("\"status\":true"));
	}
}
