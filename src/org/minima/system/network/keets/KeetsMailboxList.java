package org.minima.system.network.keets;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.bouncycastle.crypto.params.Ed25519PublicKeyParameters;
import org.bouncycastle.crypto.signers.Ed25519Signer;
import org.minima.objects.base.MiniData;
import org.minima.system.params.GeneralParams;
import org.minima.utils.MinimaLogger;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;
import org.minima.utils.json.parser.JSONParser;

/**
 * The signed list of keets mailboxes (store-and-forward nodes) this node hands its sidecar.
 *
 * Fetched from GeneralParams.KEETS_MAILBOX_URL, verified with the Ed25519 public key baked in
 * GeneralParams.KEETS_MAILBOX_SIGNKEY, cached at data/keets/mailboxes.json. A new fleet
 * mailbox joins the network by re-signing the published file - no app release. Any failure
 * falls back to the cached copy, then to "" ( the sidecar then uses its baked-in defaults ).
 *
 * File   : {"v":1,"ts":<ms>,"mailboxes":["<64-hex>",...],"sig":"<128-hex>"}
 * Signed : "1\n<ts>\n<mailboxes sorted, joined by ,>"   ( plain RFC 8032 Ed25519 )
 * Maintained with keets/tools/mailboxes-sign.js.
 */
public class KeetsMailboxList {

	public static final String CACHE_FILE = "mailboxes.json";

	private static final int  TIMEOUT_MS = 5000;
	private static final int  MAX_BYTES  = 64 * 1024;

	/**
	 * @return comma separated 64-hex mailbox pubkeys for the sidecar's --mailboxes, or "" to use its defaults
	 */
	public static String load(File zDataFolder) {
		File dir   = new File(zDataFolder, "keets");
		File cache = new File(dir, CACHE_FILE);

		JSONObject cached = null;
		try {
			if(cache.exists()) {
				cached = parseAndVerify(new String(Files.readAllBytes(cache.toPath()), StandardCharsets.UTF_8), GeneralParams.KEETS_MAILBOX_SIGNKEY);
			}
		}catch(Exception exc) {
			MinimaLogger.log("[KEETS] cached mailbox list ignored : "+exc.getMessage());
		}

		JSONObject fetched = null;
		String url = GeneralParams.KEETS_MAILBOX_URL.trim();
		if(!url.equals("")) {
			try {
				fetched = parseAndVerify(fetch(url), GeneralParams.KEETS_MAILBOX_SIGNKEY);
				if(cached != null && ts(fetched) < ts(cached)) {
					MinimaLogger.log("[KEETS] fetched mailbox list is older than the cached one - ignored");
					fetched = null;
				}
			}catch(Exception exc) {
				MinimaLogger.log("[KEETS] mailbox list fetch failed ( "+exc.getMessage()+" ) - using "+(cached != null ? "cached list" : "sidecar defaults"));
			}
		}

		JSONObject use = fetched != null ? fetched : cached;
		if(use == null) {
			return "";
		}
		if(fetched != null) {
			try {
				dir.mkdirs();
				Files.write(cache.toPath(), use.toJSONString().getBytes(StandardCharsets.UTF_8));
			}catch(Exception exc) {
				MinimaLogger.log("[KEETS] could not cache mailbox list : "+exc.getMessage());
			}
		}
		String list = String.join(",", keys(use));
		MinimaLogger.log("[KEETS] mailbox list : "+keys(use).size()+" mailboxes ( "+(fetched != null ? "fetched" : "cached")+", ts "+ts(use)+" )");
		return list;
	}

	/**
	 * Parse the JSON document and verify its signature. Throws on anything invalid.
	 */
	public static JSONObject parseAndVerify(String zJson, String zPubKeyHex) throws Exception {
		JSONObject doc = (JSONObject) new JSONParser().parse(zJson);
		Object v = doc.get("v");
		if(v == null || ((Number)v).longValue() != 1) {
			throw new Exception("unsupported list version");
		}
		long ts = ts(doc);
		if(ts <= 0) {
			throw new Exception("bad ts");
		}
		List<String> keys = keys(doc);
		if(keys.isEmpty()) {
			throw new Exception("empty mailbox list");
		}
		String sighex = (String) doc.get("sig");
		if(sighex == null || !sighex.matches("[0-9a-fA-F]{128}")) {
			throw new Exception("bad sig");
		}
		byte[] pub = new MiniData(zPubKeyHex).getBytes();
		if(pub.length != 32) {
			throw new Exception("bad sign key");
		}
		byte[] msg = canonical(ts, keys).getBytes(StandardCharsets.UTF_8);
		byte[] sig = new MiniData(sighex).getBytes();
		Ed25519Signer signer = new Ed25519Signer();
		signer.init(false, new Ed25519PublicKeyParameters(pub, 0));
		signer.update(msg, 0, msg.length);
		if(!signer.verifySignature(sig)) {
			throw new Exception("signature INVALID");
		}
		return doc;
	}

	/** The signed string : "1\n<ts>\n<sorted keys joined by ,>" - identical to mailboxes-sign.js canonical() */
	public static String canonical(long zTs, List<String> zKeys) {
		List<String> sorted = new ArrayList<>(zKeys);
		Collections.sort(sorted);
		return "1\n"+zTs+"\n"+String.join(",", sorted);
	}

	private static long ts(JSONObject zDoc) {
		Object ts = zDoc.get("ts");
		return ts instanceof Number ? ((Number)ts).longValue() : -1;
	}

	/** lower-case 64-hex keys, no 0x, sorted ; throws on a malformed entry */
	private static List<String> keys(JSONObject zDoc) {
		List<String> out = new ArrayList<>();
		Object arr = zDoc.get("mailboxes");
		if(!(arr instanceof JSONArray)) {
			throw new IllegalArgumentException("no mailboxes array");
		}
		for(Object o : (JSONArray)arr) {
			String k = String.valueOf(o).toLowerCase();
			if(k.startsWith("0x")) {
				k = k.substring(2);
			}
			if(!k.matches("[0-9a-f]{64}")) {
				throw new IllegalArgumentException("bad mailbox key "+o);
			}
			out.add(k);
		}
		Collections.sort(out);
		return out;
	}

	private static String fetch(String zUrl) throws Exception {
		HttpURLConnection conn = (HttpURLConnection) new URL(zUrl).openConnection();
		conn.setConnectTimeout(TIMEOUT_MS);
		conn.setReadTimeout(TIMEOUT_MS);
		conn.setRequestProperty("User-Agent", "minima-keets/"+GeneralParams.MINIMA_PORT);
		try {
			if(conn.getResponseCode() != 200) {
				throw new Exception("HTTP "+conn.getResponseCode());
			}
			try (InputStream in = conn.getInputStream()) {
				ByteArrayOutputStream buf = new ByteArrayOutputStream();
				byte[] b = new byte[4096];
				int n;
				while((n = in.read(b)) > 0) {
					buf.write(b, 0, n);
					if(buf.size() > MAX_BYTES) {
						throw new Exception("list larger than "+MAX_BYTES+" bytes");
					}
				}
				return new String(buf.toByteArray(), StandardCharsets.UTF_8);
			}
		}finally {
			conn.disconnect();
		}
	}
}
