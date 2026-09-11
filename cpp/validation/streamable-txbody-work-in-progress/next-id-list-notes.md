# Next required fix: block-ID iterators

The current checkpoint preserves list and element ownership but still stores IDs
in `std::vector<std::shared_ptr<MiniData>>`. This is not Java ArrayList iteration.
`TxPoWTreeNode::checkFullTxns` invokes an `exists` callback during a range loop.
A callback can append to or clear the list returned by `getBlockTransactions`.
Appending can reallocate the vector, invalidating the loop's iterator and end;
copying the current element's shared_ptr does not protect those iterators.
This is a concrete native safety gap, not just missing API surface. Fix before
production approval. The current differential corpus does not exercise it.

## Inspected reuse candidates

- `cpp/src/support/JSONSequence.hpp`: complete implementation read. Generic
  storage already supplies locked copied access, owned iterators and modification
  checks. Array iteration uses current size for hasNext and checks the structural
  change counter in next, matching the existing Java comparison corpus.
- `cpp/src/support/JavaList.hpp`: complete implementation read. Supports fixed
  size checks and set/clear, but exposes ordinary vector iterators. It cannot fix
  this invalidation problem unchanged.
- `cpp/src/org/minima/utils/json/JSONArray.hpp/.cpp` and `JSONObject.hpp`: read
  complete existing JSONSequence callers. Array storage already uses the generic
  iterator; map storage uses the other template branch.
- `cpp/tools/json_collections_differential.py`,
  `cpp/support/JSONCollectionsOracle.java`, `cpp/support/json_collections.cpp`:
  read complete comparison harness, including add/remove during iteration,
  exhausted iterators, retained owners and nonstructural map replacement.
- `cpp/support/shared_json_native.cpp`: read complete concurrent/retained-owner
  stress test. These existing JSON tests must continue passing after adaptation.
- Searched the relevant core and tools/minima-core-cpp sibling trees. The sibling
  TxBody uses a value vector and does not provide checked iteration. Its complete
  header/implementation were read; its codecs and randomness also differ.

## Smallest compatible adaptation to evaluate

Reuse `JSONSequence<std::shared_ptr<MiniData>, true>` for TxBody's ID storage.
Add narrow indexed set and clear operations to the existing generic storage using
its lock/exception conventions. Set is nonstructural; ArrayList.clear increments
the change counter even when already empty. Preserve nullable entries, duplicate
references and owned getters. Use the existing checked range in actual consumers.
Do not introduce a second unrelated iterator framework or use snapshots as a
substitute for the source's observable mutation/failure ordering.

Extend meaningful Java/native tests with mutations before/between/after iterator
steps, same-size set, empty clear, list replacement retaining old iterators, and an
actual consumer callback that changes the block list. Include exact callback order,
exception class and the checkFullTxns cached-flag outcome. Review the pinned Java
consumer and fixture setup before writing that integration test. This checkpoint
records candidate review only; no list adaptation or new iterator test is included.

After that fix, resume Magic and TxHeader using next-header-magic-notes.md.
