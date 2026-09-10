# Lab: saved incentive IDs and HTTP failures

Written before testing the new counterpart. incentivecash optionally stores a UID,
saves UserDB, then reads the current UID. An empty stored UID skips the request. Supplying an empty or whitespace-only UID fails in getParam before saving; it does not clear the stored UID. Nonempty UID
is trimmed by getParam when supplied, then concatenated directly into the fixed HTTPS URL and sent with PUT. The returned
text is parsed as JSON and cast to JSONObject; null is accepted, arrays/scalars
are rejected. A failed request or parse does not undo the saved ID.

Reuse the actual UserDB serializer, MinimaDB save path, RPCClient PUT and JSON
parser. Compare in-memory preferences, saved file bytes, a fresh UserDB reload,
requests, replies, logs and runner behavior. Capture saved bytes at the request
boundary to prove ordering, rather than inferring it from final state alone.

The fixture records the original HTTPS URL and routes it to a local HTTP server.
Java does this through URLStreamHandlerFactory; the native fixture overrides a
protected transport method and calls its real RPCClient default with the routed
URL. This tests the command and real PUT/response handling without calling the
public incentive service. Public-service TLS and account behavior are separate
from this local experiment. No credentials or real account IDs are used.

Include missing, empty, malformed and raw special-character UIDs; repeated reads;
wrong parameter and stored-value types; saving disabled; a failed save; HTTP status
errors; malformed JSON; null/array/scalar responses; and runner permission checks.
A successful temporary preference-file reload proves this file workflow, not a
durable SQL backend or a complete native node.

## Observed results

Normal and ASAN/UBSAN comparisons pass 106 programs, 207 operations and 79 actual
loopback PUT requests per runtime. They compare exact preference bytes and fresh
reloads. Directory-blocked saving logs the source file-open error and continues;
HTTP/parse errors leave the saved UID in place. Supplied blank IDs fail before
saving, while missing/null stored IDs return an empty UID without a request.

The source URL uses version 1.1.2, not the full micro version 1.1.2.8. Its JSON
parser accepts raw control characters in strings. Initial incorrect assertions
for these cases are retained alongside the native tab-URL and file-logging
mismatches. C++ now preserves tabs in the raw target and logs direct-save I/O
errors through MinimaLogger. The existing RPC suite also adds tab-path cases.

Read the [saved reports](../../validation/incentive-cash-work-in-progress) and
[command lesson](../files/src/org/minima/system/commands/base/incentivecash.md).
The native code adds a protected requestPUT boundary for testing. Production uses
the exact source origin and actual RPCClient. This experiment does not certify
public-service access, complete TLS/provider behavior, late streaming-write
failures, durable SQL or a complete native node.
