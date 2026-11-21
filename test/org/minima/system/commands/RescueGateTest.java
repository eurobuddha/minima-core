package org.minima.system.commands;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.Test;
import org.minima.utils.json.JSONObject;

/**
 * rescue (2026-11-21): it posts a MegaMMR resync from the given host and the node halts afterwards - a
 * non-admin companion (0xFF) must not be able to run it. Nothing is resynced here: the gate refuses before the
 * command runs (on the old code the command runs and fails on the missing node).
 */
public class RescueGateTest {

	@Test
	public void nonAdminCannotRescue() {
		JSONObject res = (JSONObject) CommandRunner.getRunner().runMultiCommand("0xFF", "rescue host:127.0.0.1:1").get(0);
		assertFalse((boolean) res.get("status"));
		assertTrue("rescue was not refused for a non-admin caller: "+res,
				String.valueOf(res.get("error")).contains("ADMIN"));
	}
}
