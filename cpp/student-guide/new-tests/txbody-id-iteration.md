# Block IDs: why owning a list is not enough

TxBody stores the IDs of transactions referenced by a block. Java exposes this
list directly. A caller can keep it, replace an entry, add an ID or clear it while
another method is traversing it. This lesson follows the
[body ownership lab](streamable-txbody.md).

## The bug found during review

The first ownership change used a shared_ptr to a vector of shared_ptr IDs. This
kept the list and each current ID alive. It did not keep a C++ vector iterator
valid when a callback appended entries and caused the vector to reallocate.
Continuing through that old iterator could access freed storage.

The real `TxPoWTreeNode.checkFullTxns` method provides a useful example. For each ID,
it asks the transaction database whether the transaction exists. The native method
expresses that query as an `exists` callback. Java permits a database subclass to
override the corresponding method. Either callback can modify the original list.

## Reuse the checked storage already in the project

The implementation reuses [JSONSequence](../../src/support/JSONSequence.hpp), the
generic storage already used by JSONArray and JSONObject. Its name reflects its
first callers; its element type is a template parameter. TxBody supplies
`shared_ptr<MiniData>` and selects the array iteration rules.

The storage owns a vector, a mutex and a structural-change counter. An iterator
owns the storage state and remembers the counter from its creation. Access copies
the current element while holding the lock. No pointer into vector storage escapes
to the traversal. Existing [JSON collection tests](../../tools/json_collections_differential.py)
and the native shared-JSON stress test remain relevant regression checks.

Only indexed `set` and `clear` needed to be added to that shared implementation.
Adding, removing and clearing are treated as structural changes; replacing an
existing entry with set is not.
Set validates the index, returns the previous object and replaces the entry without
changing the structural counter. Clear changes the counter even for an empty list.
The existing add, remove and iterator operations are reused. Body writers and
actual chain, search, network and command consumers use its checked range.

## Java does not check for changes at every step

`hasNext` compares the cursor with the current list size. `next` checks the saved
structural counter before returning an element. These are different operations.

Consider two IDs, A and B. After next returns A, the cursor is one. If the callback
removes B, the list size also becomes one. hasNext now returns false, so the loop
finishes without calling next and without throwing. If the callback appends C
instead, hasNext returns true and the following next throws
ConcurrentModificationException. Replacing B with C using set does not change the
counter; the next iteration sees C normally.

Despite its name, this exception can occur within one thread: a callback changes
the list before its iterator resumes.

Clearing a one-element list after visiting its element leaves cursor one and size
zero. Since they differ, hasNext returns true and next throws. This is why a rule
such as “all structural changes always throw immediately” would be an incorrect
translation, even if it avoided C++ memory errors.

## Test the actual consumer and its cached result

The [Java oracle](../../support/TxBodyIDsOracle.java) uses the real TxPoW body list
and TxPoWTreeNode, with a TxPoWDB subclass controlling the existence query. The
[native harness](../../support/txbody_ids.cpp) calls their native counterparts.
The [comparison driver](../../tools/txbody_ids_differential.py) records callback
order, exceptions, retained list contents and the body's current list.

checkFullTxns returns false immediately when an ID is missing. A modification made
inside that last callback may therefore never reach another iterator step. On
successful completion, the method caches success. To distinguish that cache from
a failed traversal, the test appends a new ID before a second call, whose callback
always returns false. A cached success bypasses that callback; an unsuccessful
first traversal must query again. This tests the method's observable behavior,
including the source's existing caching policy.

Separate list programs cover empty/exhausted iterators, invalid indices, nullable
and duplicate entries, nonstructural replacement, mutation of a retained MiniData,
and iterators kept after replacing the list or releasing the body. In the release
case, the iterator itself must keep the storage alive. Twenty-one hand-derived
boundary programs also check each runtime against explicit expected observations.

## Evidence and limits

[Checkpoint evidence](../../validation/txbody-id-iteration-work-in-progress/progress.json)
records the current build and comparison state. The initial source-only run passed
all 21 boundary expectations; that alone does not verify the C++ implementation.
Both full builds pass all 1,400 Java/native comparisons (42 independent boundary
checks), the existing JSON checks and 7,623 Streamable comparisons (95,664 assertions).
All 23 affected normal regressions also pass, including native node/archive startup.
The new sanitizer run directly tests iterator/consumer and shared-storage behavior;
the earlier ownership baseline separately passed all 23 broad sanitizer regressions.

This change fixes the identified iterator invalidation path. It does not certify
all inherited ArrayList APIs, iterator.remove, sublists, arbitrary subclass or
allocation-failure behavior, or every multithreaded schedule. Locking collection
access does not make every TxBody field or MiniData object thread-safe. The full
457-file port remains incomplete.

For practice, predict the callback trace and second-call result when A's callback
appends C and returns false. Then compare it with a callback that removes B and
returns true. Explain which source operation makes their outcomes different.
