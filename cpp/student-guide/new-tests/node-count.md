# Lab: enumerating a peer network

Written before the nodecount comparisons. This command is a graph traversal with
two sets: discovered peers and peers already pinged. Initial defaults are pinged
first; their client counts are ignored. Each later pass snapshots the difference,
adds discoveries, and counts clients only from that pass's successful greetings.
The default mode stops after the source's 6% formula reaches 100%; complete mode
changes the percentage calculation and reports measured totals.

Use the existing real NIO ping transport with loopback servers that send scripted
Greeting frames. Record exactly which server receives each ping, the request wire,
traffic, logs, response and CSV bytes. Java supplies the expected behavior. Include
cycles, duplicate defaults/discoveries, failed/malformed replies, multiple passes,
early exit, empty network, fractional/negative/non-finite client counts and malformed
parameter/peer types. No test should enumerate the public network.

Set iteration order matters because partial traversal can stop after one peer.
Reuse SocketAddressSet and JavaHashMap, preserving the installed JDK's collection
constructor capacity. Compare retained endpoint identity and collision/resizing
cases. Float32 arithmetic and float-to-int saturation are separate from double
client accumulation. Preserve Java numeric parsing, including hexadecimal literals,
suffixes, whitespace, overflow, NaN and Infinity.

CSV creation occurs before complete is parsed. The header is buffered until the
first loop iteration flushes. An empty scan therefore creates an empty file. Later
PrintWriter write failures are swallowed by the source. The native descriptor is
closed by RAII; reproducing the source's leaked writer is not a resource-lifetime
parity claim. Tests must not infer live network accuracy or complete native-node
parity from these finite local graphs.

## Observed checkpoint

Both normal and ASAN/UBSAN builds pass 171 programs and 314 loopback pings per
runtime. Both full builds and four affected regression suites pass. The
source class is not registered in CommandRunner; direct construction tests its
behavior, and runner cases verify that the name remains unknown.

The tests caught a temporary-list lifetime error in discovery, incorrect initial
registration and old malformed-peer exception categories. An embedded-NUL numeric
error required the existing message-preserving JavaTypedException carrier. Empty
MiniData has an empty text encoding in this source, so an empty CSV is compared
with that encoding. Ping updates traffic breakdowns without changing the separate
global byte counters; the assertions preserve this source distinction.

The affected checks include 874 peer-state programs / 26,348 operations, 640
transport serialization/send cases plus the existing real ping cases, 1,265
registry cases, and 31 P2P discovery programs / 84 operations / 39 TCP pings.
All are finite checks on the installed JDK and this native platform. HashSet
identity-hash tree ties, arbitrary Java Float object use, source writer leaks and
all possible decimal-rounding/locale/reference/concurrency behavior remain outside
this checkpoint's certification. The command's estimate is not an independently
verified census of the public network.
