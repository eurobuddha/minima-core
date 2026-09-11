# MySQL archives: test data that survives the process

The command recording tests compare decisions and SQL calls. This lab goes further:
it runs actual Java and C++ nodes with real H2 files and a disposable MySQL server.
Both normal and sanitizer builds passed 3,202 assertions across 17 node processes
each (81.039s and 343.945s respectively), with no reported native sanitizer errors. See the [saved evidence](../../validation/mysql-live-archive-work-in-progress/progress.json).

Reuse matters here. The existing Node helper supplies solo, noconnect startup,
command input, response parsing and graceful shutdown. ArchiveCommandOracle and
its native counterpart create the same deterministic blocks and export them using
the already translated archive command. The test adds workflows, not another
implementation of MySQL or a pretend chain processor.

A 257-block fixture crosses MySQL's 250-block read limit and the raw reader's
256-block batch boundary. Its blocks also carry coins with known addresses and
state. After update, direct SQL reads check every stored block's height and exact
bytes. A first update to an empty database starts after the oldest local block,
so the source skips that block. Fixmissing repairs it; a second repair demonstrates
that the accepted endfix parameter is unused. Address history and coin indexing
exercise the same stored blocks through different command paths.

Raw exports are decompressed and compared with the concatenation of independently
framed source objects. A zero or negative maximum still writes the first block
because the source checks the limit after writing. H2 export contains real SQL;
the other runtime imports that file, then a raw re-export proves all block and
cascade bytes survived. Comparing file existence or a success flag alone would
miss missing records or changed data.

Database fixtures need not be valid consensus chains. The large synthetic archive
is used to test storage. Actual resync uses a valid genesis from a real saved node.
Both runtimes run chain resync, nested reset, and seed replacement; the opposite
runtime then opens the saved node and checks chain identity and key-use counters.
Signing and rejecting a changed message demonstrates that reopened wallet keys
work, not merely that public-key strings were saved. These checks do not yet prove
long-chain or cascade resync.

All node directories are temporary, all keys are disposable test keys, and the
MySQL fixture uses a random loopback port and tmpfs storage. The test never touches
user node directories or an existing database. Failure reports retain the stage
and process output so a shutdown or persistence defect can be reproduced.

For students: explain why 257 blocks reveal boundaries that three blocks cannot.
Then explain why successful import and successful consensus resync require
different fixtures. Finally, predict which key-use counters should change after
one signature following chain resync versus seed replacement.

The first run stopped on a mistaken test expectation, retained in the evidence.
CommandRunner trims the outer spaces inside the quoted phrase parameter; BIP39
then collapses spaces and returns uppercase words. The test now expects exactly
those source behaviors. This was a test correction, not a change to seed derivation.
