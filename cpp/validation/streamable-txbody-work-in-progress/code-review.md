## Code Review

### Summary

This incremental TxBody port preserves shared field identity, nullable child and ID
owners, source-ordered partial reads, and nested output prefixes. Actual TxPoW
body/list getters and their consumers now retain owners. The expanded Java/native
interface corpus includes independently predicted bytes and child/list identities.
Full builds and interface tests pass in both configurations. See progress.json for
the final affected-regression state. This is a local work-in-progress checkpoint;
it is not approval of complete object or node parity.

### Findings

#### 🟠 MAJOR — Structural ID-list mutation invalidates native iterators

**File:** `cpp/src/org/minima/database/txpowtree/TxPoWTreeNode.cpp:66`

**Problem:** checkFullTxns invokes a callback during iteration over a publicly
retained mutable vector. A callback that appends enough entries to reallocate that
vector leaves the loop's iterator and end dangling. Owning the vector and copying
the current element protects those objects, but does not protect vector iterators.
The source ArrayList has observable checked-iteration behavior. Existing tests do
not cover this schedule; passing them cannot establish its safety.

**Fix:** Reuse the existing checked generic storage in
`cpp/src/support/JSONSequence.hpp`, minimally adapting indexed set/clear and the
body-list consumers. Compare mutation schedules and an actual consumer callback
against Java. Full candidate review and next steps are in next-id-list-notes.md.
This fix takes priority over the next Magic/TxHeader feature work.

#### Resolved during implementation — Child reader can replace its own field

**File:** `cpp/src/org/minima/objects/TxBody.cpp:23`

The four read calls now retain a local child owner until the call returns or
throws. The self-replacing Transaction fixture drops its last parent reference and
then continues reading. Complete and truncated Java/native cases pass, including
the old child's result and the replacement remaining in the parent. Const writer
accessors already return an owned shared_ptr temporary for the call expression.

#### Resolved during validation — Malformed-count loop corrupted case accounting

**File:** `cpp/tools/streamable_differential.py:126`

The original loop reused the comparison counter name. The existing accounting
gate rejected the run even though observations matched. The distinct body_count
variable fixes the counter. Final interface runs execute exactly 7,623 planned
comparisons and 95,664 independent assertions; the rejected report is retained.

### Verdict

🔁 Request changes before production approval.

Save the tested incremental work locally, then fix checked ID-list iteration with
the already inspected implementation. Shared global constant identity, remaining
TxPoW ownership/reader/getter/hash semantics, arbitrary streams and complete node
parity remain explicitly unfinished. No release, deployment or merge is authorized
by this report.
