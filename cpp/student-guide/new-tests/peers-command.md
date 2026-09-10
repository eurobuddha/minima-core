# Lab: peer discovery command, queues and files

Written before the comparison. `peers` uses existing P2P state; it does not
itself admit a remote node. Adding or fetching peers queues forced checks.
The checker later decides whether each address is usable.

Read the pinned command and preserve its ordering. Disabled P2P or slave mode
returns before reading action parameters. `forcecheck` queues work before
reading max. Peer limits are inclusive: max zero can return one peer. Empty
list reports zero in `list`, but one in `forcecheck`, because Java splitting an
empty string returns one element and only `list` corrects it.

Reuse the Main/network fixture with unstarted workers. Compare real queue
messages, host/port, force flag, duplicates and partial queues after a later
invalid port. Use actual RPCClient against a loopback HTTP server for add/fetch.
Use actual MiniFile writes in a temporary folder for publish, including defaults,
replacement and failure. Inspect resulting file bytes as well as JSON replies.

Shuffle results must be validated as bounded subsets of known peers; unrelated
random streams cannot be compared position by position. Check that a copy was
shuffled and the known-peer set was unchanged. For singleton lists, malformed
owners and an unresolved singleton, compare exact replies and log bodies.

Retain failures and final results. These finite cases do not prove live peer
admission, public DNS behavior, concurrent scheduling or full native-node parity.

## What the comparison found

A fixture snapshot dereferenced a temporary shared pointer in a range loop. The
vector could be destroyed before iteration. Retaining the copied peer list in a
local variable fixes its lifetime; the initial normal and sanitizer failures are
saved. Production getPeersList already retained its copy.

An empty peer set then reached haveAnyPeers' database fallback. The fixture had
not bound that service because older network tests did not use it. Binding the
existing MinimaDB P2P database completes this fixture path. The initial failure
is retained; no replacement database implementation was introduced.

Final results: 271 programs, 1,355 operations and 48 HTTP requests per runtime
pass in normal and ASAN/UBSAN builds (3.79s and 5.27s). The queue assertions
explicitly check duplicate submissions, the first queued address surviving a
later bad port, and forcecheck posting before malformed max is rejected.

All affected comparisons also pass in both builds: 222 network-command programs /
794 operations, 124 network-status programs / 562 operations, and 1,265 runner
cases. The full normal build/relink passes. See the [saved evidence](../../validation/peers-command-work-in-progress/progress.json).
