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
	public void aListCutAtTheBoundLosesItsLastPartialEntry() throws Exception {
		//the bound falls right after "1.2.3.4:90" of "1.2.3.4:9001" : a plain cut would use port 90
		StringBuilder list = new StringBuilder("9.9.9.9:9001,");
		while(list.length() < P2PManager.SEED_LIST_MAX_BYTES - 10) {
			list.append(list.length() == P2PManager.SEED_LIST_MAX_BYTES - 11 ? "," : "x");
		}
		list.append("1.2.3.4:9001,5.6.7.8:9001");
		byte[] bytes = list.toString().getBytes("UTF-8");
		assertEquals("1.2.3.4:90", new String(bytes, P2PManager.SEED_LIST_MAX_BYTES - 10, 10, "UTF-8"));
		ArrayList<String> want = new ArrayList<>();
		want.add("9.9.9.9:9001");
		assertEquals(want, P2PManager.parseSeedPeers(P2PManager.cutSeedList(bytes)));
		assertEquals("a list within the bound is untouched", "1.2.3.4:9001,5.6.7.8:9001", P2PManager.cutSeedList("1.2.3.4:9001,5.6.7.8:9001".getBytes("UTF-8")));
		assertEquals("no comma at all : nothing", "", P2PManager.cutSeedList(new byte[P2PManager.SEED_LIST_MAX_BYTES + 5]));
		//the bound is in BYTES : a two-byte character before the cut must not hide that the list was too long
		byte[] multi = ("\u00e9," + list.toString()).getBytes("UTF-8");
		assertTrue(!P2PManager.parseSeedPeers(P2PManager.cutSeedList(multi)).contains("1.2.3.4:90"));
		for(String entry : P2PManager.parseSeedPeers(P2PManager.cutSeedList(multi))) {
			assertTrue(entry, entry.equals("9.9.9.9:9001"));
		}
	}

	@Test
	public void theChoiceIsDrawnFromAllEntriesAndIsNeverMoreThanTheCap() {
		ArrayList<String> all = new ArrayList<>();
		for(int i=0;i<400;i++) {
			all.add("10."+(i/250)+"."+(i%250)+".1:9001");
		}
		assertEquals("every usable entry is parsed, not only the first 50", 400, P2PManager.parseSeedPeers(String.join(",", all)).size());
		java.util.HashSet<String> seen = new java.util.HashSet<>();
		for(int round=0;round<40;round++) {
			ArrayList<String> choice = P2PManager.seedChoice(all);
			assertEquals(P2PManager.SEED_PEERS_MAX, choice.size());
			assertEquals("each once", P2PManager.SEED_PEERS_MAX, new java.util.HashSet<>(choice).size());
			seen.addAll(choice);
		}
		assertTrue("entries past the first 50 are chosen too ( "+seen.size()+" different ones in 40 rounds )", seen.size() > 300);
		assertEquals(3, P2PManager.seedChoice(all.subList(0, 3)).size());
	}

	@Test
	public void aSeedingIsNotAskedForMoreOftenThanItsChecksTake() {
		assertTrue(P2PManager.SEED_PEERS_INTERVAL >= P2PManager.SEED_PEERS_MAX * 10000L);
	}

	@Test
	public void thisMachineAndPrivateNetworksAreNotChecked() throws Exception {
		for(String local : new String[] {"127.0.0.1","10.0.0.1","192.168.1.1","172.16.0.1","169.254.1.1","0.0.0.0"}) {
			assertTrue(local, !P2PManager.seedAddressAllowed(new InetSocketAddress(java.net.InetAddress.getByName(local), 9001)));
		}
		assertTrue(!P2PManager.seedAddressAllowed(InetSocketAddress.createUnresolved("no-such-host.invalid", 9001)));
		assertTrue(!P2PManager.seedAddressAllowed(null));
		assertTrue(P2PManager.seedAddressAllowed(new InetSocketAddress(java.net.InetAddress.getByName("8.8.8.8"), 9001)));
	}

	@Test
	public void aHugeListIsCutAtTheCap() {
		StringBuilder list = new StringBuilder();
		for(int i=0;i<5000;i++) {
			list.append("10.").append(i/250).append(".").append(i%250).append(".1:9001,");
		}
		assertEquals(P2PManager.SEED_PEERS_MAX, P2PManager.seedChoice(P2PManager.parseSeedPeers(list.toString())).size());
	}
}
