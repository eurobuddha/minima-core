# Lab: archive requests over a real loopback socket

Written before the request tests. Reuse the peer checker's Runner and Responder,
the native MainNetworkFixture and Java NIOClientOracle.install. The production
archive.sendArchiveReq opens the socket and reads the response; the fixture only
supplies the Main/NIO traffic receiver and controls the remote bytes.

Encode an empty IBD using both real codecs. Compare response frames and expected
request frames, including negative, zero, fractional and large start heights.
The responder records every framed request. Change the response type byte to
show that this helper discards it without checking its value. Supply truncated
frames, invalid MiniData lengths, missing IBD fields and trailing payload bytes.
Compare returned IBD bytes, request count and the traffic breakdown. NIOTraffic's
addReadBytes/addWriteBytes update the breakdown, not the total counters. The
four-byte MiniData frame length must not count toward payload traffic.

Test absent Main, network or NIO manager: a request may already have been written
before the missing traffic receiver fails. Use the responder's existing barrier
to drain accepted connections before inspecting the trace. Zero/negative attempt
limits must make no connection. A two-attempt recovery must consume a dropped
reply then a valid reply, with at least ten seconds between attempts. An exhausted
two-attempt case must return null. Exercise the default three-attempt overload too.

Compare results and wire effects exactly. OS/JVM socket exception log prose is
outside this comparison and must not be described as matching. The test uses no
public host and does not shorten production timeouts or sleeps. Read-timeout and
partial-read timing require separate controlled delayed responses; passing a drop
case is not evidence of timeout fidelity.

Both builds pass 37 Java/native comparisons. Two-attempt cases take over ten
seconds per runtime, and default-third-attempt recovery takes over twenty. The
source constants were not shortened. An empty but complete response payload still
adds a zero-byte read-breakdown entry before IBD decoding fails. Missing traffic
receivers can fail after the remote has received the frame, before accounting it.
No production change was needed; all 509 existing archive cases also pass again.
