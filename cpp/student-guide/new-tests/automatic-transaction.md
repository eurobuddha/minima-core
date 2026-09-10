# Lab: automatic transaction assembly and failure cleanup

Written before the new comparison. The command creates a custom transaction,
then calls txnaddamount, optionally attaches MMR/script proofs, and optionally
calls txnsign. Read each failure boundary carefully: a child command returning
status false deletes the new row; a proof-builder exception leaves it present.
Parameter errors and duplicate IDs occur before creating the new row.

Use the existing wallet/chain/miner/SQL fixture and real registered child
commands. Compare every stored transaction/witness byte after each operation,
logs, SQL effects, coin locks, and signatures verified against the transaction ID.
Test malformed parameters, noncanonical token IDs, insufficient funds, duplicate
IDs, amount/burn combinations, missing scripts, signing failure and null owners.
Identifiers are concatenated into child command text without quoting in Java;
spaces and separators must be tested rather than silently repaired in C++.

The static createTransaction helper is different from the user command. Despite
its address argument, the source never creates a recipient output. It consumes
the requested amount, creates only change, and signs; the address is unused.
It also checks amount validity before coin search, unlike createBurnTransaction,
and leaves the default link hash untouched. Share the proven burn builder while
preserving those differences and the exact order of database/miner lookups.

Use Java-generated coins, MMR proofs and small TreeKeys. Check static helper
wire, output counts/amounts, default link hash and signature validity. Verify
that the unused address, including null, has no effect. Re-run the existing
burn/witness and funding comparisons after the shared-code adaptation. Recording
SQL does not establish durable storage or real network acceptance.

## What the comparison found

The initial helper assertion assumed a 32-byte all-zero link hash. The source
Transaction initializes it from MiniData.ZERO_TXPOWID, whose value is `0x00`.
The native implementation already preserved that one-byte value; the assertion
was corrected after 260 programs agreed. The original failure is retained.

The normal build passes 414 programs / 1,655 operations and compares 29,925 SQL
call observations. These include real child-command cleanup, proof failures that
retain the row, actual signing, and helper output conservation.

The sanitizer comparison also passes all 414 programs (177.74 seconds, versus
34.16 seconds normal). The existing proof/burn comparison passes 671 programs /
2,734 operations / 41,653 SQL observations in both builds. The normal funding
comparison passes 736 programs / 3,418 operations / 39,016 SQL observations.
See the [saved evidence](../../validation/automatic-transaction-work-in-progress/progress.json).

The sanitizer funding comparison also passes all 736 programs. Both current full
builds/relinks and both 1,265-case runner checks pass. The older shared-JSON full
sanitizer baseline finished with 202/204 initial passes; its two prior failures
have passing reruns retained separately. That composite evidence is not a fresh
full-current-211-suite pass.
