# Lab: RPC commands through a real listener

Written before implementing this experiment. The existing sequential RPC command
suite proves state changes and failures but never successfully starts a listener.
Reuse the actual Main/NetworkManager test setup, production HTTPServer/CMDHandler,
Authorizer, UserDB and RPCClient. Start with unstarted node workers and an ephemeral
RPC port. This isolates RPC lifecycle integration without claiming full-node startup.

First run rpc enable:true with a synthetic password. The command stops the old
listener, sleeps one second, updates settings and starts the new listener. Wait
for a real bound port before sending requests. Compare missing, wrong and correct
credentials. Then add/list/remove users through authenticated HTTP requests, not
by directly patching the preference array. The pinned CMDHandler reads the mode
but does not pass it into its CommandRunner call; compare the observed read-user
command behavior rather than assuming the mode name enforces a restriction.

Rotate the default password by issuing rpc enable:true through that same listener.
The handler must finish its response even though it stopped its old listening
socket. Observe the new listener, reject the old default password and retain the
extra-user records. Enabling without password disables default authentication but
retains both the old password and the extra-user array. Existing users can still
make a header necessary: test the actual CMDHandler/Authorizer combination.

Disable through RPC and verify the request completes and the listening port closes.
Also make a live enable operation fail on a malformed password after it stops the
listener. Its enabled flag may already have changed; do not infer a live socket
from the flag alone. Finally restart and shut down cleanly. Never use quit in this
fixture because the source handler terminates the process.

Compare exact HTTP response bytes using the existing cmd_handler_differential
socket helper, which validates Date shape and byte length before replacing only
the clock value. Validate ephemeral ports before normalizing those observations.
Use RPCClient on successful calls too. Keep each process's logging streams stable
for the whole fixture; swapping a C++ stream buffer while background logging runs
would introduce a test race. Retain and report the real one-second command pause.


## Initial integration finding

The native listener returned 401 for a missing header, then closed the connection
without a response when a wrong header reached Authorizer. Its diagnostic was
"Authorizer requires MinimaDB binding". The production callback had no default;
earlier isolated server fixtures supplied it themselves. Java directly reads
MinimaDB.getDB().getUserDB(). Reuse that existing native lookup as Authorizer's
default callback, retaining fixture overrides. Looking up the current database
on each call also avoids retaining an obsolete UserDB after database replacement.
No callback assignment is needed each time MinimaDB.createDB replaces its owner.
The [captured failure](../../validation/rpc-live-work-in-progress/initial-binding-failure.json)
shows why a passing isolated server test was insufficient.

The next case replaces MinimaDB while the listener stays active. A previously
registered extra user must then be rejected, because the new UserDB has no such
record; the default user still uses the same global password. Re-add the user
through RPC and continue the lifecycle. This distinguishes looking up the current
database from accidentally capturing the original UserDB in a startup callback.

## Observed results

Both normal and ASAN/UBSAN builds pass two lifecycles (LF and CRLF), each with
41 matching Java/C++ observations. Each runtime performs 25 raw HTTP requests,
two actual RPCClient calls and eight real one-second pauses per lifecycle.
The database-replacement case passes without installing an Authorizer callback
in the fixture. [Saved results](../../validation/rpc-live-work-in-progress/progress.json)
record scope and the separate full regression suites still running.

This experiment reuses [MainFixture](../../support/MainFixture.hpp), extracted
from main_runtime, and the existing MainNetworkFixture/Oracle. SQL has only a
constructor binding here; node workers stay unstarted. Passing RPC transport
does not establish durable preferences or complete native-node startup.
