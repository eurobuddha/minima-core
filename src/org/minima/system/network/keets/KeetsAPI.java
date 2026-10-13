package org.minima.system.network.keets;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import org.minima.system.params.GeneralParams;

/**
 * Minimal client for the keets sidecar loopback API ( 127.0.0.1:MINIMA_PORT+5 ).
 * Auth is the same per-boot token the MSG_KEETS preamble uses, sent as the
 * x-keets-token header. Works for both sidecar hosts : desktop ( KeetsManager
 * spawns the process ) and Android ( the app runs it as a bare-kit worklet with
 * --token getTokenHex() ) - same token, same port derivation.
 */
public class KeetsAPI {

	public static int getApiPort() {
		return GeneralParams.MINIMA_PORT + 5;
	}

	public static byte[] call(String zMethod, String zPath, byte[] zBody) throws Exception {
		URL url = new URL("http://127.0.0.1:" + getApiPort() + zPath);
		HttpURLConnection con = (HttpURLConnection) url.openConnection();
		con.setConnectTimeout(5000);
		//sends can dial a peer then fall back to a mailbox - allow for that
		con.setReadTimeout(120000);
		con.setRequestMethod(zMethod);
		con.setRequestProperty("x-keets-token", KeetsManager.getTokenHex());
		con.setRequestProperty("Connection", "close");

		if (zBody != null) {
			con.setDoOutput(true);
			OutputStream os = con.getOutputStream();
			os.write(zBody);
			os.flush();
			os.close();
		}

		int code = con.getResponseCode();
		InputStream is = (code >= 200 && code < 300) ? con.getInputStream() : con.getErrorStream();
		ByteArrayOutputStream out = new ByteArrayOutputStream();
		if (is != null) {
			byte[] buf = new byte[8192];
			int r;
			while ((r = is.read(buf)) != -1) {
				out.write(buf, 0, r);
			}
			is.close();
		}
		con.disconnect();

		if (code < 200 || code >= 300) {
			throw new Exception("keets api " + zMethod + " " + zPath + " -> HTTP " + code + " " + new String(out.toByteArray(), "UTF-8"));
		}
		return out.toByteArray();
	}

	public static String getStr(String zPath) throws Exception {
		return new String(call("GET", zPath, null), "UTF-8");
	}

	public static String postStr(String zPath, byte[] zBody) throws Exception {
		return new String(call("POST", zPath, zBody), "UTF-8");
	}

	public static String deleteStr(String zPath) throws Exception {
		return new String(call("DELETE", zPath, null), "UTF-8");
	}
}
