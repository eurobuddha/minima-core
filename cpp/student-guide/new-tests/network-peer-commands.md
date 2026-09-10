# Lab: commands that act on direct peers

Written before these tests. disconnect and message read raw String fields from
params. They do not use Command.getParam's trimming or blank rejection. Missing
uid/data and wrong types must fail before obtaining Main. An explicit empty uid
is different from an absent uid in message's reply, though both broadcast.

Reuse the existing unstarted NIO and P2P fixtures behind actual Main and
NetworkManager owners. disconnect uid:all queues NIO_DISCONNECTALL; another uid
uses the real NIOManager.disconnect queue operation. message serializes a
MiniString in a GENMESSAGE envelope and uses actual NIOServer routing. Broadcast
reaches only clients with valid greetings; a specified UID can target an invalid
client. Missing recipients still affect recorded outgoing traffic.

p2pstate casts the owned peer manager to P2PManager and calls getStatus(true).
Compare actual state output, null Main/network/peer owners and wrong manager type.
Do not replace getStatus with a preselected response. Metadata, typed raw params,
reply text, exact queued data, packet bytes, traffic and runtime errors are the
observations. Independent byte/recipient assertions should check delivery rules.

No server selector, networking worker or public socket is started. Fake selection
keys represent queue readiness; the real NIOClient sendData body queues bytes.
This validates command-to-component composition, not delivery over a real network.

## Regression follow-up: local IPv6 interfaces

The existing real-interface test found a scoped Java IPv6 hostname whose native
counterpart lacked its interface suffix. P2PEnvironment.numeric previously used
only the OS sockaddr scope field; OpenJDK's BSD enumeration assigns the interface
index for every IPv6 interface address before creating Inet6Address. The cached
OpenJDK source at /private/tmp/minima-jdk26-NetworkInterface.c, lines 1750–1769,
confirms this order. Reuse the existing numeric formatter and interface-index
lookup, then compare the whole Java/native hostname sets with the existing test.
This is a real enumeration test, so available addresses depend on the machine.

## Final results

All 222 programs and 794 operations match Java in both builds (3.55s/5.68s final).
Both full builds pass without observed compiler warnings. The source comparison
exposed missing-NIO exception classification; the existing P2P regression exposed
the IPv6 interface suffix. Both corrections are validated. The initial fixture
setup failure was corrected before accepting any packet-delivery assertions.

All 12 affected regressions pass across phases. Two loopback suites initially
failed because sandbox socket access was restricted; reruns with the required
access pass. Final normal/sanitizer batches passed in 55.91s/56.89s. The review
records each corpus and the earlier phases. No full-node or exhaustive parity
claim follows from these finite tests.
