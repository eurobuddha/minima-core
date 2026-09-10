# Lab: inspect and control the network owner

Written before the new network-command tests. The source network.java has more
branches than its short help advertises. list is the default; reset clears traffic;
recalculateip asks the existing manager to select a host; restart queues a Main
message; loggingon and loggingoff update MessageProcessor's shared trace state.
Unknown action throws CommandException. Preserve the spelling and case checks.

The list body reads uid even though getValidParams advertises only action. Direct
method tests can exercise the filter, but textual runner calls with uid must fail
parameter validation. Lists contain connecting and connected NIOClientInfo objects;
an exact nonempty UID returns only the first match. Full NetworkManager status is
looked up afterward through Main again, not synthesized from the list.

Reuse MainNetworkFixture and its real NIO client maps, traffic object, peer manager
and Main queue. Bind its currently unused NetworkManager.getUserDB callback to the
real test database before testing getStatus. Observe trace/filter globally: even a
failure at the second logging receiver can leave the first trace update visible.
Seed traffic totals and breakdown separately, because addReadBytes/addWriteBytes do
not increment total counters in this pin. Reset must clear both maps and totals,
with a new start time between the operation's before/after clocks.

Fix synthetic connection times and the initial traffic start time for reproducible
date formatting; validate live reset times before excluding their exact values.
Use the existing configured-host bypass and read-only local interface enumeration
for recalculateip. No interface configuration or actual network restart is needed:
restart is asserted as a queued MAIN_NETRESTART message on an unstarted Main.

Test metadata, missing/blank/null/non-string action and uid, hidden actions, case
sensitivity, missing Main/network/NIO/peer, list filtering and the runner gate.
Compare successful component snapshots plus partial effects after exceptions.
Finite fixtures do not certify asynchronous restart, full startup or all scheduling.

## Final results

All 124 programs and 562 operations match Java in normal and sanitizer builds
(4.53s/3.80s). Both full builds pass without observed compiler warnings. All ten
affected regression groups pass (19.15s/27.27s), including Main, peer commands,
ping, NetworkManager, Command and both runner suites. All 18 JSON reports were
read and contain no failures. The map check passes at 357 native counterparts.

The first 119-program comparison passed before adding empty-list, duplicate-UID,
partial logging-off and exact restart-queue assertions. A duplicate UID in both
maps returns the connecting entry first when filtered; the unfiltered list
retains both entries. The shared tracing test also corrects an earlier lesson's
wording: setFullLogging changes static tracing for all processors, so it is not
a per-subsystem logging switch.
