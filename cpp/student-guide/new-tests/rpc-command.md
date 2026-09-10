# Lab: RPC configuration and user records

Written before implementing the RPC command tests. Read the pinned rpc.java body
in order. The enable branch wins over action: parse enable, stop the existing
server, sleep one second, store RPC_ENABLED, then parse/store the optional password
and authentication flag, and finally start or stop the server. Disabling calls
stop twice. A bad password can leave enabled changed after the first stop. An
interrupted sleep must leave the configuration unchanged after that stop.

The source advertises ssl but never reads it. The valid parameter list contains
password twice. The constructor captures the current RPC port in its short help;
full help reads the current port when called. Preserve these observable details.

User management uses the actual UserDB array. Adding requires authentication,
rejects exact username minima, lowercases mode and accepts read/write only. It
allows duplicate names and modifies the same array. Removing builds a replacement
array that excludes every matching name; malformed rows can fail before replacement.
Listing returns the actual array, including extra users' passwords. The default
password is masked as *** in the response. Use synthetic passwords only in tests.

Compare direct parameters, runner permissions, source error order, duplicate users,
malformed arrays/rows, array identity, retained replies and all configuration flags.
Use the existing Main fixture and actual NetworkManager/HTTPServer for successful
start/stop checks on temporary local ports, with explicit shutdown at teardown.
A loopback request should reach the existing CMDHandler and Authorizer. Observe
partial state after failure rather than judging only a final successful report.

The native command reuses Main's sleep service through a callback with a real
one-second default. A test can throw the native InterruptedException category;
Java tests interrupt the calling thread itself. This tests propagation at the
sleep boundary, not a complete implementation of Java Thread.interrupt semantics.
Full-node startup, durable preferences and exhaustive concurrent configuration
changes remain separate validation requirements.

## Current results and remaining work

Both full builds pass. All 258 current programs / 590 operations match Java,
and all 11 selected regressions pass in normal and ASAN/UBSAN builds (85.74s /
127.35s). All 22 reports were read and saved. The comparison exposed and fixed
JsonDB's wrong exception category for malformed arrays. It also corrected an
initial test expectation: blank action is rejected by getParam before dispatch.

This is work in progress. The current suite does not successfully start an RPC
listener. Before the live enable/authentication tests, review shared flags/password
and mutable user arrays accessed by request workers. Preserve source array
aliasing while defining safe native access; a passing sequential test does not
establish concurrent safety.

[Corpus](../../tools/rpc_commands_corpus.py) ·
[Scoped results](../../validation/rpc-work-in-progress/progress.json)
