package org.minima.system.network.keets;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;

import org.minima.objects.base.MiniData;
import org.minima.system.params.GeneralParams;
import org.minima.utils.MinimaLogger;

/**
 * Runs the keets sidecar : a node.js process speaking Hyperswarm / HyperDHT
 * that bridges each peer stream to our own Minima port over loopback.
 *
 * Each bridged connection announces itself with a single MSG_KEETS preamble
 * carrying a per-boot auth token (passed to the child via env - never over
 * the DHT) - see NIOMessage.
 */
public class KeetsManager {

	/**
	 * Per-boot auth token the sidecar must echo in every MSG_KEETS preamble
	 */
	private static final MiniData mToken = MiniData.getRandomData(32);

	public static boolean checkToken(MiniData zToken) {
		return mToken.isEqual(zToken);
	}

	Process mProcess 	= null;
	Thread 	mRunner 	= null;
	boolean mShutdown 	= false;

	public void start() {

		if(GeneralParams.KEETS_SIDECAR.equals("")) {
			MinimaLogger.log("[KEETS] no -keetssidecar path set.. keets disabled");
			return;
		}

		mRunner = new Thread(this::runLoop, "KEETS_SIDECAR");
		mRunner.setDaemon(true);
		mRunner.start();
	}

	private void runLoop() {

		long backoff = 2000;

		while(!mShutdown) {

			try {
				ArrayList<String> cmd = new ArrayList<>();
				cmd.add("node");
				cmd.add(GeneralParams.KEETS_SIDECAR);
				cmd.add("--port");
				cmd.add(""+GeneralParams.MINIMA_PORT);
				cmd.add("--datadir");
				cmd.add(GeneralParams.DATA_FOLDER);
				cmd.add("--topic");
				cmd.add(GeneralParams.TEST_PARAMS ? "minima-testnet-keets-v1" : "minima-mainnet-keets-v1");
				cmd.add("--maxconns");
				cmd.add(""+GeneralParams.KEETS_MAX_CONNS);
				if(!GeneralParams.KEETS_BOOTSTRAP.equals("")) {
					cmd.add("--bootstrap");
					cmd.add(GeneralParams.KEETS_BOOTSTRAP);
				}
				if(!GeneralParams.KEETS_CONNECT.equals("")) {
					cmd.add("--connect");
					cmd.add(GeneralParams.KEETS_CONNECT);
				}

				ProcessBuilder pb = new ProcessBuilder(cmd);
				pb.environment().put("KEETS_TOKEN", mToken.to0xString());
				pb.redirectErrorStream(true);

				MinimaLogger.log("[KEETS] starting sidecar : "+String.join(" ", cmd));
				mProcess = pb.start();

				//A clean start resets the backoff
				backoff = 2000;

				//Relay the sidecar logs
				BufferedReader br = new BufferedReader(new InputStreamReader(mProcess.getInputStream()));
				String line;
				while((line = br.readLine()) != null) {
					MinimaLogger.log("[KEETS] "+line);
				}

				mProcess.waitFor();

			}catch(Exception exc) {
				MinimaLogger.log("[KEETS] sidecar error : "+exc);
			}

			if(mShutdown) {
				break;
			}

			MinimaLogger.log("[KEETS] sidecar exited.. restart in "+backoff+"ms");
			try {Thread.sleep(backoff);} catch (InterruptedException e) {break;}
			if(backoff < 60000) {
				backoff *= 2;
			}
		}
	}

	public void shutdown() {
		mShutdown = true;
		if(mProcess != null) {
			mProcess.destroy();
		}
		if(mRunner != null) {
			mRunner.interrupt();
		}
	}
}
