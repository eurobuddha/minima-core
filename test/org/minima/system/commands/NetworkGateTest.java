package org.minima.system.commands;

import static org.junit.Assert.assertTrue;

import org.junit.BeforeClass;
import org.junit.Test;
import org.minima.database.MinimaDB;
import org.minima.utils.json.JSONObject;

/**
 * A non-admin caller - a read-mode RPC user, a companion app (0xFF) - may not add a webhook (every block, txpow
 * and MiniDapp event posted to a URL of its choosing) nor connect the node to a peer or drop one (2026-11-23).
 */
public class NetworkGateTest {

	@BeforeClass
	public static void setup() {
		MinimaDB.createDB();
	}

	@Test
	public void nonAdminCannotAddWebhooksOrChangePeers() {
		for(String cmd : new String[] {"webhooks action:add hook:http://127.0.0.1:1/x", "webhooks action:clear",
				"connect host:127.0.0.1:1", "disconnect uid:x"}) {
			JSONObject res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0xFF", cmd).get(0);
			assertTrue(cmd+" ran for a non-admin caller: "+res, String.valueOf(res.get("error")).contains("ADMIN"));
		}
	}
}
