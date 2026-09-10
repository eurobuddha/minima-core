# Lab: composing transaction commands

Written before the tests for createfrom, constructfrom, postfrom and signfrom.

These four files are coordinators. They prepare arguments and call smaller
transaction commands. Think of a laboratory worksheet: the sequence of steps,
the values passed between steps, and what happens after a failed step all matter.
Reusing the existing command runner preserves its parsing and validation rules.

`createfrom` selects funds from an address. `constructfrom` lists specific input
coins and adds recipient/change outputs. Both export an unsigned transaction and
remove the temporary row. `postfrom` imports a row and posts it. `signfrom` imports
one, signs using a supplied key, optionally posts, then exports and removes it.

Do not assume cleanup always runs. Java has no enclosing finally block here.
createfrom checks the funding command status; constructfrom checks each input
status. On those failures they delete and throw. Other child statuses are often
ignored. postfrom inserts before parsing mine; signfrom inserts before parsing
its key, key-use count and post option. Wrongly typed late parameters can leave a row. A nonempty string such as `bad`
is interpreted as false by getBooleanParam; it is not a Boolean parsing error.

Two experiments distinguish orchestration from integration. First, subclasses
record calls to the existing public runCommand(String) overload and return
controlled results or throw. Compare exact call order/text, outer replies,
exceptions, and stored rows against Java. This does not implement a fake wrapper:
the inherited source wrapper body executes unchanged. Second, let the same
recording overload call the actual CommandRunner. Observe real transaction rows,
exported bytes, scripts, signing, cleanup and child failures in an offline fixture.
No live node or wallet is needed for those tests.

Random 32-byte transaction IDs cannot match across processes. Check their shape,
then assign consistent labels. For a serialized TxnRow, inspect its initial
four-byte string length and replace only the verified ID bytes. Everything after
that field must remain byte-for-byte equal. Preserve fixed IDs supplied by tests.
Runtime-specific null/cast diagnostics can be compared by exception class;
explicit command error messages must still match.

Boundary cases: empty/doubled comma separators, whitespace-only tokens, invalid
addresses, fractional/negative amounts, token burns, malformed serialized rows,
ID collisions, missing keys, bad booleans, and every child-call failure position.
Check that signfrom mutates the exported response after deleting the row, and
that errors copying a posted TxPoW are logged and swallowed only in its small
try/catch block. Exact parser input matters: do not silently escape scripts or
remove the source's space after `fromaddress:`.

One real-child experiment produces an empty createfrom export: the source space
in `fromaddress: 0x01` triggers a parser reply whose status does not activate the
wrapper’s cleanup/error branch. The ignored proof-builder failure also does not
prevent export. This is pinned behavior, not evidence of a valid funded transfer.

The typed child-status checks distinguish null (NullPointerException) from a
non-Boolean value (ClassCastException). An empty child-result list raises
IndexOutOfBoundsException. A rejection with a null error becomes a nullable
CommandException message, and the public runner must serialize `error:null`.

A further review case combines failed row decoding with a wrongly typed ID.
Java evaluates the ID argument before invoking setID on the null receiver, so
ClassCastException wins over NullPointerException. C++ must parse the argument
into a local before checking the receiver. Both postfrom and signfrom now follow
that order; the baseline experiment records the original mismatch.

C++ what() exposes a null-terminated view. It cannot report the stored length of
an error containing an embedded NUL. The native exception therefore retains its
complete optional string; getMessage supplies it to the JSON runner. Compare
null, empty, Unicode and embedded-NUL error strings separately.

## Evidence

Both normal and ASAN/UBSAN builds pass 659 programs and 2,060 child calls (14.29s/58.31s). All five selected checks pass per build, including the command, runner, runner-error and source-mapping regressions. Both full C++ builds pass without compiler warnings. The raw observations and reports are written in each build directory. The two review baselines are `wallet-wrapper-id-order-baseline.json` and `wallet-wrapper-null-character-baseline.json` in the normal build directory. These reproduce the receiver-order and embedded-NUL defects before their fixes.

Full native Main startup, durable SQL and complete network integration remain
separate unfinished work. The offline exports are not asserted to be valid funded
network transactions.
