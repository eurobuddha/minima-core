# Lab: a command reply can be null

Written before these tests. ping returns null when the required host contains no
colon. That is different from an empty object, a failed command reply or an
exception inside ping. The native JSONValue already represents object-or-null;
reuse it as the no-argument Command.runCommand return type. Existing command
bodies can keep constructing the same JSONObject replies. String-taking helper
overloads have a different role and keep their existing result types.

Follow the original runner carefully. It catches exceptions from runCommand,
adds the returned reference to its result array, and then reads status OUTSIDE
the catch. A null result therefore causes a null-pointer exception to escape;
it must not become the runner's normal error JSON. An absent/wrong/null status
also fails at that later stage. Test event order, logs, stop behavior, permissions
and invalid-parameter gates using the existing runner error fixture.

Direct ping tests should observe null for no-colon host text before looking up
Main. Blank/missing/wrong-type input fails parameter validation first. A colon
selects the first split; port text uses Java integer parsing and error wording.
Successful transport tests should use an isolated loopback peer and the actual
NIO ping implementation, checking greeting fields and the request frame. Never
contact a public peer for a command test. This lab does not certify full startup.

## Observed results

Both full builds pass. The 1,680 runner-error cases and 95 ping programs plus nine
loopback cases pass in normal and sanitizer builds. The new direct assertions
confirm null replies, stopped follow-up execution, exact request bytes and
extradata:null from a successfully decoded greeting. Both normal and sanitizer
ping reports are read and contain zero failures. All 35 selected normal
regressions pass in 181.54 seconds; all 35 sanitizer regressions pass in 762.10 seconds.

The initial new fixture did not compile because native JSONObject has no remove
method. Its missing-status probe now uses the existing entries and put operations
to build the same field set. The first attempted comparison used the old binary
and was rejected; only results after the successful rebuild are accepted. This
is why a test run is evidence only when the intended fixture actually compiled.
