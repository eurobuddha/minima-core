package org.minima.system.network.keets;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.security.SecureRandom;
import java.util.Arrays;

import org.bouncycastle.crypto.params.Ed25519PrivateKeyParameters;
import org.bouncycastle.crypto.params.Ed25519PublicKeyParameters;
import org.bouncycastle.crypto.signers.Ed25519Signer;
import org.junit.Test;
import org.minima.objects.base.MiniData;
import org.minima.system.params.GeneralParams;

public class KeetsMailboxListTest {

	static final String K1 = "7f84f689d4477395d7c4a2493852a56fe646a8d05a94ce8b0a1baae34ac5d1df";
	static final String K2 = "9161e165df60ee780776363143701a82a2e8fb3792a0eea58577b1e4adbb35fd";

	Ed25519PrivateKeyParameters priv = new Ed25519PrivateKeyParameters(new SecureRandom());
	String pub = new MiniData(((Ed25519PublicKeyParameters) priv.generatePublicKey()).getEncoded()).to0xString().substring(2);

	String sign(long ts, String... keys) {
		byte[] msg = KeetsMailboxList.canonical(ts, Arrays.asList(keys)).getBytes(StandardCharsets.UTF_8);
		Ed25519Signer s = new Ed25519Signer();
		s.init(true, priv);
		s.update(msg, 0, msg.length);
		return new MiniData(s.generateSignature()).to0xString().substring(2);
	}

	String doc(long ts, String sig, String... keys) {
		StringBuilder sb = new StringBuilder("{\"v\":1,\"ts\":"+ts+",\"mailboxes\":[");
		for(int i=0;i<keys.length;i++) { sb.append(i>0?",":"").append("\"").append(keys[i]).append("\""); }
		return sb.append("],\"sig\":\"").append(sig).append("\"}").toString();
	}

	@Test
	public void validSignatureAccepted() throws Exception {
		// keys given unsorted + 0x-prefixed : the canonical form sorts and strips
		String json = doc(1700000000000L, sign(1700000000000L, K2, K1), "0x"+K2.toUpperCase(), K1);
		assertEquals(1700000000000L, ((Number) KeetsMailboxList.parseAndVerify(json, pub).get("ts")).longValue());
	}

	@Test
	public void tamperedListRejected() {
		String json = doc(1700000000000L, sign(1700000000000L, K1), K2);
		try { KeetsMailboxList.parseAndVerify(json, pub); fail("tampered list verified"); } catch(Exception e) { assertTrue(e.getMessage().contains("INVALID")); }
	}

	@Test
	public void wrongKeyAndJunkRejected() {
		String json = doc(1700000000000L, sign(1700000000000L, K1), K1);
		try { KeetsMailboxList.parseAndVerify(json, K2); fail("wrong key verified"); } catch(Exception e) {}
		try { KeetsMailboxList.parseAndVerify(doc(1700000000000L, sign(1700000000000L, "zz"), "zz"), pub); fail("bad key accepted"); } catch(Exception e) {}
		try { KeetsMailboxList.parseAndVerify("{\"v\":2}", pub); fail("v2 accepted"); } catch(Exception e) {}
	}

	@Test
	public void offlineUsesCacheAndNoCacheMeansDefaults() throws Exception {
		File data = Files.createTempDirectory("keetsmb").toFile();
		String oldUrl = GeneralParams.KEETS_MAILBOX_URL, oldKey = GeneralParams.KEETS_MAILBOX_SIGNKEY;
		try {
			GeneralParams.KEETS_MAILBOX_SIGNKEY = pub;
			GeneralParams.KEETS_MAILBOX_URL = "http://127.0.0.1:9/mailboxes.json"; // nothing listens : fetch fails fast
			assertEquals("", KeetsMailboxList.load(data));
			new File(data, "keets").mkdirs();
			Files.write(new File(data, "keets/"+KeetsMailboxList.CACHE_FILE).toPath(), doc(5L, sign(5L, K1, K2), K1, K2).getBytes(StandardCharsets.UTF_8));
			assertEquals(K1+","+K2, KeetsMailboxList.load(data));
			// a cache signed by someone else is ignored
			Files.write(new File(data, "keets/"+KeetsMailboxList.CACHE_FILE).toPath(), doc(6L, new String(new char[128]).replace('\0', '0'), K1).getBytes(StandardCharsets.UTF_8));
			assertEquals("", KeetsMailboxList.load(data));
		} finally {
			GeneralParams.KEETS_MAILBOX_URL = oldUrl;
			GeneralParams.KEETS_MAILBOX_SIGNKEY = oldKey;
		}
	}
}
