# Lab: reset coordinates restore and archive import

Written before the tests. Read pinned backup/reset.java: the key-creation gate
runs before required archivefile/action validation. chainsync forwards an import
command. seedsync quotes the phrase but leaves paths and keys/keyuses as raw text.
restore first runs restore shutdown:false; it checks that result's Boolean status,
reopens the sync databases through Main, and then requests archive import.

The outer result remains successful when archive import returns status:false.
Only the initial restore failure becomes a CommandException containing the whole
nested result JSON. Do not improve quoting, propagate additional failures, add
rollback, or bypass CommandRunner; those would change the source behavior.

Reuse CommandResult.first/status and the existing real-H2 restore fixture. Add
scoped recording command prototypes for archive and optionally restore, following
the wallet-wrapper fixtures. Keep the actual parser/runner in the call path so
spaces, quotes, semicolons, JSON values and malformed parameters remain observable.
The recording commands report their complete parsed text, params and caller ID.
They may return controlled failures or malformed status values. Restore the original
registry after every request. Recording archive does not implement archive import.

Record Main.restoreReadyForSync through its Java override boundary and optionally
invoke the real method. Compare missing/invalid parameters, metadata/factory,
key-gate order, all actions, nested result handling, and readiness failure order.
For integration cases, use a real encrypted backup, the real restore command and
real restoreReadyForSync with H2. Verify replaced wallet rows/use increments,
reopened stores and processor creation before the recorded archive call. Stop
only test-owned workers and close old/new test connections after observations.

Archive import and real chain synchronization are still absent; these tests prove
the wrapper and its restore/reopen integration, not a complete reset workflow.
Source behavior after unsuccessful archive import must remain visible as partial
state. Exact exception categories matter; helpful JVM cast/null prose may require
a narrowly documented normalization, never normalization of success/failure.

## Results

Normal and ASAN/UBSAN comparisons each pass 224 cases, including four real H2
restore/reopen cases. Both complete builds succeed. Source preferences and six
relevant history rows survive restoration; key uses become 261 and 356. The real
sync-readiness method replaces the wallet and starts a processor before archive
execution. A recorded archive failure leaves the restored state and outer success
intact, demonstrating the source's partial-success behavior.

The detailed [evidence](../../validation/reset-work-in-progress/progress.json)
records regression scope. Passing these checks does not supply the missing archive
command or certify the full API. The mapped reset file remains implemented_partial
and parity not_verified until all of its dependencies and workflows are validated.
