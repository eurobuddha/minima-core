package org.minima.system.network.p2p;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.net.InetSocketAddress;
import java.util.ArrayList;

import org.junit.Test;

/**
 * A published peers list is somebody's file. An entry with a port outside 1..65535 made
 * new InetSocketAddress throw on the P2PSeedPeers thread, and an uncaught exception stops the node.
 */
public class SeedPeersListTest {

	@Test
	public void onlyExactHostPortEntriesWithARealPortAreUsed() {
		ArrayList<String> got = P2PManager.parseSeedPeers(" 1.2.3.4:9001 ,node.example.org:10001,1.2.3.4:70000,x:-1,5.6.7.8:0,:9001,"
				+ "9.9.9.9,9.9.9.9:,9.9.9.9:abc,[::1]:9001,a b:9001,1.2.3.4:9001,evil.example;quit:9001,7.7.7.7:65535,7.7.7.7:999999");
		ArrayList<String> want = new ArrayList<>();
		want.add("1.2.3.4:9001"); want.add("node.example.org:10001"); want.add("7.7.7.7:65535");
		assertEquals(want, got);
		//every entry that is let through makes an address : nothing here can throw
		for(String entry : got) {
			int index = entry.lastIndexOf(":");
			new InetSocketAddress(entry.substring(0,index), Integer.parseInt(entry.substring(index+1)));
		}
	}

	@Test
	public void anEmptyOrMissingListGivesNothing() {
		assertTrue(P2PManager.parseSeedPeers(null).isEmpty());
		assertTrue(P2PManager.parseSeedPeers("").isEmpty());
		assertTrue(P2PManager.parseSeedPeers(",,, ,").isEmpty());
	}

	@Test
	public void aHugeListIsCutAtTheCap() {
		StringBuilder list = new StringBuilder();
		for(int i=0;i<5000;i++) {
			list.append("10.").append(i/250).append(".").append(i%250).append(".1:9001,");
		}
		assertEquals(P2PManager.SEED_PEERS_MAX, P2PManager.parseSeedPeers(list.toString()).size());
	}
}
