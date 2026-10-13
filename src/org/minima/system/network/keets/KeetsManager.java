package org.minima.system.network.keets;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Base64;

import org.minima.objects.base.MiniData;
import org.minima.system.Main;
import org.minima.system.params.GeneralParams;
import org.minima.utils.MinimaLogger;
import org.minima.utils.json.JSONArray;
import org.minima.utils.json.JSONObject;
import org.minima.utils.json.parser.JSONParser;

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

	/**
	 * The per-boot token as 0x hex - for hosts that run the sidecar themselves
	 * instead of via this manager ( Android starts it as a bare-kit worklet )
	 */
	public static String getTokenHex() {
		return mToken.to0xString();
	}

	Process mProcess 	= null;
	Thread 	mRunner 	= null;
	Thread 	mPoller 	= null;
	boolean mShutdown 	= false;

	public void start() {

		//The inbox poller drains messages into MDS events - ONLY when MDS is
		//actually on, or the messages would be consumed with no listener. Without
		//MDS, apps poll 'keets action:inbox' and ack themselves. The sidecar API
		//is at MINIMA_PORT+5 whether WE spawn the sidecar ( desktop ) or the host
		//app runs it as a worklet ( Android )
		if(GeneralParams.MDS_ENABLED) {
			mPoller = new Thread(this::pollLoop, "KEETS_INBOX");
			mPoller.setDaemon(true);
			mPoller.start();
		}

		if(GeneralParams.KEETS_SIDECAR.equals("")) {
			MinimaLogger.log("[KEETS] no -keetssidecar path set.. sidecar not spawned by this node");
			return;
		}

		mRunner = new Thread(this::runLoop, "KEETS_SIDECAR");
		mRunner.setDaemon(true);
		mRunner.start();
	}

	/**
	 * Drain the sidecar message inbox and re-emit each message as an MDS event :
	 *
	 *   { "event":"KEETS", "data":{ id, from, ts, payload (base64), via,
	 *                               received, message (utf8 decode) } }
	 *
	 * Messages are acked ( deleted from the sidecar inbox ) once the event is
	 * posted - a MiniDapp that wants history must persist them ( service.js + SQL,
	 * see the reference chat dapp ). Messages arriving while this node is OFF wait
	 * at the keets mailboxes, so nothing is lost across restarts.
	 */
	private void pollLoop() {

		while(!mShutdown) {

			try {Thread.sleep(5000);} catch (InterruptedException e) {break;}

			try {
				String inbox = KeetsAPI.getStr("/messages");
				JSONArray msgs = (JSONArray) new JSONParser().parse(inbox);
				for(Object mo : msgs) {
					JSONObject msg = (JSONObject) mo;

					//Convenience utf8 decode for text payloads
					try {
						byte[] raw = Base64.getDecoder().decode((String)msg.get("payload"));
						msg.put("message", new String(raw, "UTF-8"));
					}catch(Exception exc) {
						msg.put("message", "");
					}

					MinimaLogger.log("[KEETS] message from:"+msg.get("from")+" via:"+msg.get("via"));
					Main.getInstance().PostNotifyEvent("KEETS", msg);

					//Acked - the event is the hand-off
					KeetsAPI.deleteStr("/message/"+msg.get("id"));
				}
			}catch(Exception exc) {
				//Sidecar down / restarting - quiet, try again next poll
			}
		}
	}

	private void runLoop() {

		long backoff = 2000;

		while(!mShutdown) {

			try {
				ArrayList<String> cmd = new ArrayList<>();
				cmd.add(GeneralParams.KEETS_NODE);
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
				//Lets an Electron binary act as the node runtime ( no effect on a real node binary )
				pb.environment().put("ELECTRON_RUN_AS_NODE", "1");
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
		if(mPoller != null) {
			mPoller.interrupt();
		}
	}
}
