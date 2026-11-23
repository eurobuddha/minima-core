package org.minima.system.network.rpc;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.lang.reflect.Field;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.database.userprefs.txndb.TxnDB;
import org.minima.system.params.GeneralParams;

/**
 * An RPC with NO password (2026-11-23): a request with neither Origin nor Sec-Fetch-Site - an <img>, a form, a page in a
 * browser that sends no Fetch-Metadata, but also curl - is served read-only when a page could have sent it without a
 * CORS preflight (a GET, or a POST with no / a simple Content-Type). A POST with X-Minima-Rpc, Authorization or a
 * non-simple Content-Type runs as ADMIN, as before. With a password nothing changes.
 */
public class RpcSimpleRequestTest {

	@Before
	public void setup() throws Exception {
		MinimaDB.createDB();
		Field txndb = MinimaDB.class.getDeclaredField("mTxnDB");
		txndb.setAccessible(true);
		txndb.set(MinimaDB.getDB(), new TxnDB());
		GeneralParams.RPC_AUTHENTICATE 	= false;
		GeneralParams.RPC_PASSWORD 		= "none";
	}

	@After
	public void teardown() {
		GeneralParams.RPC_AUTHENTICATE 	= false;
		GeneralParams.RPC_PASSWORD 		= "none";
	}

	static boolean row(String zID) {
		return MinimaDB.getDB().getCustomTxnDB().getTransactionRow(zID) != null;
	}

	static String post(String zHeaders, String zCommand) throws Exception {
		return RpcHardeningTest.raw("POST / HTTP/1.1\r\nHost: 127.0.0.1:9005\r\n"+zHeaders+"Content-Length: "+zCommand.length(), zCommand);
	}

	@Test
	public void simpleRequestsRunReadOnly() throws Exception {
		String reply = RpcHardeningTest.raw("GET /txncreate%20id:simpleget HTTP/1.1\r\nHost: 127.0.0.1:9005", "");
		assertFalse("a password-less GET ran a WRITE command: "+reply, row("simpleget"));
		assertTrue(reply, reply.contains("X-Minima-Rpc"));
		for(String type : new String[] {"", "Content-Type: text/plain;charset=UTF-8\r\n",
				"Content-Type: application/x-www-form-urlencoded\r\n", "Content-Type: multipart/form-data; boundary=x\r\n"}) {
			post(type, "txncreate id:simplepost");
			assertFalse("a password-less simple POST ("+type.trim()+") ran a WRITE command", row("simplepost"));
		}

		//reads still work by GET (curl http://127.0.0.1:9005/random)
		assertTrue(RpcHardeningTest.raw("GET /random HTTP/1.1\r\nHost: 127.0.0.1:9005", "").contains("\"status\":true"));
	}

	@Test
	public void preflightedRequestsRunAsAdmin() throws Exception {
		post("X-Minima-Rpc: 1\r\n", "txncreate id:customheader");
		assertTrue(row("customheader"));
		post("Content-Type: application/json\r\n", "txncreate id:jsontype");
		assertTrue(row("jsontype"));
	}

	@Test
	public void passwordRpcUnchanged() throws Exception {
		GeneralParams.RPC_AUTHENTICATE 	= true;
		GeneralParams.RPC_PASSWORD 		= "secret";
		String auth = java.util.Base64.getEncoder().encodeToString("minima:secret".getBytes("UTF-8"));
		RpcHardeningTest.raw("GET /txncreate%20id:passwordget HTTP/1.1\r\nHost: 127.0.0.1:9005\r\nAuthorization: Basic "+auth, "");
		assertTrue(row("passwordget"));
	}
}
