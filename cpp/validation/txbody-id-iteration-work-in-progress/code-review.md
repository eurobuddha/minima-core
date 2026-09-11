## Code Review

### Summary

The change replaces unsafe vector traversal of exposed TxBody ID lists with the
existing checked JSONSequence implementation. It retains shared list/element
ownership and the source's distinction between structural changes and indexed
replacement. Actual checkFullTxns callbacks and retained iterator owners are tested
against pinned Java. Both full builds, 1,400 new comparisons per build, existing JSON
checks and 7,623 Streamable comparisons per build pass. All 23 affected normal regressions also pass, including native node/archive startup.
The sanitizer directly checks the iterator/consumer cases and shared JSON storage.
The 23 broader suites were run in the normal build for this change; the preceding
ownership baseline separately passed those suites under sanitizer.

### Findings

#### Resolved MAJOR — Callback-driven list mutation invalidated vector iterators

**Files:** `cpp/src/org/minima/objects/TxBody.hpp`,
`cpp/src/org/minima/database/txpowtree/TxPoWTreeNode.cpp`

The owned list now uses JSONSequence's checked range. Its iterator owns storage
and copies the current element under its existing mutex. A later vector
reallocation cannot invalidate a pointer retained by the loop. The next operation
checks structural modification according to the existing array-iterator rules.
All affected native ID consumers use this range; they still own the original list
and each current ID across callbacks.

#### Checked — Set, clear and early loop completion differ

**File:** `cpp/src/support/JSONSequence.hpp`

Set checks its index, returns an owned copy of the old element and does not advance
the structural counter. Clear advances that counter even on an empty list. The
existing iterator implementation is reused unchanged. Tests include shortened
lists whose new size equals the cursor, empty clear, exhausted next, null entries,
and current-element identity after replacement. Their expected Java behavior is
not reduced to “every mutation throws.”

#### Checked — Consumer result caching is observed, not inferred

**Files:** `cpp/support/TxBodyIDsOracle.java`, `cpp/support/txbody_ids.cpp`,
`cpp/tools/txbody_ids_differential.py`

The fixture uses real TxPoWTreeNode and body ID lists. The database existence
boundary is controlled by the existing Java-subclass/native-callback approach.
After the first traversal, a new ID is appended and a second callback always
returns false. This distinguishes cached success from unsuccessful traversal.
Callbacks, exceptions, retained/current lists and second-call observations are
compared. Twenty-one hand-derived cases additionally check each runtime against
explicit expected results. Byte-based removal is a fixture helper invoking real
structural removal; it does not claim full ArrayList.remove(Object) API parity.

### Verdict

✅ Approve with suggestions for the documented incremental scope.

The reviewed implementation resolves the identified native iterator invalidation
path. The recorded normal integration and targeted sanitizer results pass. This is not a full ArrayList, TxBody, TxPoW or node
parity approval. No additional production defect was found in the reviewed change;
remaining inherited APIs, custom subclasses, allocation failures and arbitrary
concurrent schedules remain outside this evidence.
