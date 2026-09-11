# Lab: MegaMMR commands and stateful integrity checks

Written before the new command tests. Read both pinned megammr.java and
megammrsync.java: import depends on the second class's wallet search and proof
export helpers. Reuse the current MegaMMR, MegaMMRBackup, MegaMMRIBD, SyncData,
IBD, wallet, coinexport/coinimport, archive socket and file implementations.
The sibling MegaMMR hashes and serializes a different schema and cannot replace
the pinned-format implementation already in this port.

Check command metadata and parameter gates first. Unknown actions in these two
classes return the initial successful reply. Info does not require -megammr,
while export/import do. Integrity is an undocumented action and has no enable gate.
Export creates its destination before taking the read lock and making the IBD.
Compare actual output bytes and files after failure, not just the response.

checkMegaMMR mutates its argument: it adds IBD blocks into the supplied MegaMMR,
then finalizes it and checks every remaining coin proof. A second call on the same
object can fail because its block time already advanced. Exercise incremental and
nonincremental heights, invalid IBD alignment, corrupted coin data, and partial
changes before failure. The final integrity response assumes a nonempty cascade;
an otherwise acceptable genesis-only backup can fail at that later dereference.

For megammrsync distinguish mydetails from getMyDetails. The command reports
address rows with a nonzero public key; the helper collects tracked addresses and
all key rows, including their original ordering. Search walks the current tree
backward, then MegaMMR only after a tree node was visited. A missing tree tip skips
MegaMMR entirely. Matching spent coins hide older unspent copies, and accepted
coins are deep copies. Check address and state-variable matches and duplicates.

Use existing real loopback Responder and Runner to check request frames, ignored
reply type, decoded MegaMMRIBD, traffic, malformed replies and missing receivers.
The timeout is 180000 ms for connection and reads, despite the source's misleading
three-second comment. There is no retry in this helper.

Resync stops key creation before checking its host, probes the archive once, then
chooses phrase before backup before existing-wallet reset. A phrase branch sets
key uses; the existing-wallet branch does not. The backup branch preserves the
source's unquoted nested command text. Both import and resync poll the real
processor every 250 ms without adding a new timeout. Coin-import failures are
ignored. Resync writes peer data after saving other databases, preserving malformed
peer rejection, duplicates and tokenization. Full live workflows, delayed socket
timeouts and resource-failure boundaries need separate evidence.

Observed results: both full builds compile the new commands. Each configuration
passes 579 command/helper comparisons and ten loopback cases. The first discrepancy
was an observer that mislabeled CommandException. The second was a production IBD
index-category mismatch after an empty-tree export had already written its file;
getTreeRoot/getTreeTip now preserve Java's index exceptions. Failed reports remain
in [saved evidence](../../validation/mega-commands-work-in-progress).

The wallet-selection fixture uses synthetic public/private key rows in real H2;
it does not assert those rows can sign. New command methods are compiled, but live
import/resync and successful proof recovery remain required. Whole-object vector
serialization before buffered output also leaves precise partial-write behavior
unverified for failures during large exports.
