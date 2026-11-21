package org.minima.system.network.rpc;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

import org.junit.Test;

/**
 * The RPC POST body: Content-Length counts bytes, and the body may hold non-ASCII text.
 * The old handler read Content-Length CHARACTERS through a reader, so a body with any multi-byte character
 * waited for characters that never came.
 */
public class CMDHandlerBodyTest {

	@Test
	public void nonAsciiBodyReadByByteLength() throws Exception {
		String body    = "send address:0xFF amount:1 state:{\"1\":\"[caf\u00e9 \u4e2d\u6587 \ud83d\ude00]\"}";
		byte[] bytes   = body.getBytes(StandardCharsets.UTF_8);
		String request = "POST / HTTP/1.1\r\nContent-Length: "+bytes.length+"\r\n\r\n";

		byte[] all = new byte[request.length()+bytes.length];
		System.arraycopy(request.getBytes(StandardCharsets.US_ASCII), 0, all, 0, request.length());
		System.arraycopy(bytes, 0, all, request.length(), bytes.length);
		InputStream in = new ByteArrayInputStream(all);

		assertEquals("POST / HTTP/1.1", CMDHandler.readLine(in));
		assertEquals("Content-Length: "+bytes.length, CMDHandler.readLine(in));
		assertEquals("", CMDHandler.readLine(in));
		assertEquals(body, CMDHandler.readBody(in, bytes.length));
	}

	@Test
	public void lineEndings() throws Exception {
		InputStream in = new ByteArrayInputStream("a\r\nb\nc".getBytes(StandardCharsets.UTF_8));
		assertEquals("a", CMDHandler.readLine(in));
		assertEquals("b", CMDHandler.readLine(in));
		assertEquals("c", CMDHandler.readLine(in));
		assertNull(CMDHandler.readLine(in));
	}

	@Test
	public void shortBodyReturnsWhatArrived() throws Exception {
		InputStream in = new ByteArrayInputStream("status".getBytes(StandardCharsets.UTF_8));
		assertEquals("status", CMDHandler.readBody(in, 10));
	}
	
	@Test
	public void namedHostsAreServedOnlyWhenListed() {
		org.minima.system.params.GeneralParams.RPC_ALLOWED_HOSTS = "";
		assertEquals(false, CMDHandler.hostAllowed("othernode.example:9005"));
		org.minima.system.params.GeneralParams.RPC_ALLOWED_HOSTS = "node.example, othernode.example";
		try {
			assertEquals(true, CMDHandler.hostAllowed("OTHERNODE.EXAMPLE:9005"));
			assertEquals(true, CMDHandler.hostAllowed("node.example"));
			assertEquals(false, CMDHandler.hostAllowed("evil.example:9005"));
		}finally {
			org.minima.system.params.GeneralParams.RPC_ALLOWED_HOSTS = "";
		}
	}
}
