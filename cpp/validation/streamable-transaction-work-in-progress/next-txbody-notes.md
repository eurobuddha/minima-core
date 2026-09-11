# Next source review: TxBody

Read complete pinned `upstream-study/src/org/minima/objects/TxBody.java`, current
`cpp/src/org/minima/objects/TxBody.hpp/.cpp`, sibling
`/Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/objects/TxBody.hpp/.cpp`,
and current `cpp/src/org/minima/objects/TxPoW.hpp/.cpp` during Transaction validation.
Located object/consensus fixture sites; a complete dependency/caller/test review
and explicit reuse report are still required before editing TxBody.

Observed source distinctions:

- Java replaces PRNG and difficulty only after each complete factory read.
- It reads into the existing four transaction/witness child objects. Null child
  failure must occur at its position, after preceding mutations/consumption.
- Only after all four children read does it replace mTxPowIDList, before its count.
  Current C++ clears a vector, which does not model retained old-list ownership.
- Java public fields can be reassigned or null. Current PRNG/difficulty/list value
  fields cannot represent every source state. Child pointers already model null.
- Current native writer stages each child's complete serialization, so callers
  cannot observe the Java nested writer prefix on a failed child.
- TxPoW currently owns an optional TxBody value and exposes references to it;
  complete retained-body and copied containing-object semantics remain open.

The sibling uses timestamp-seeded mt19937_64 for PRNG, generic MiniData framing
instead of the dedicated bounded hash methods, pointer readers and a long count
with reserve. Its vector overload ignores the caller's offset and delegates to
an unbounded pointer reader. It does not supply an in-place body reader or JSON.
These are concrete reasons to retain the current proven codecs and random-data
helper. Do not transplant that sibling implementation.

Next review must trace all public body-field access, getTxBody ownership and
getBlockTransactions callers, plus the random generator and child codecs. Existing
Transaction/Witness shared records and owned getter handles are available proven
building blocks. No TxBody implementation was changed in this checkpoint.

Additional caller inventory: complete Java TxPoW constructor/accessor segment
(lines 120-240) confirms getBlockTransactions returns the actual body list, or a new
empty list when no body exists. Current C++ returns a vector copy. Located consumers
in TxBlock, TxPoWTreeNode, scanchain, burn, TxPoWSearcher, TxPoWProcessor, TxPoWChecker
and NIOMessage. Their complete methods still need review before changing this API.
Native NIOMessage case 21 also uses getTxBody().value().resetRandomPRNG(). Fixture
field assignment is widespread (consensus, wallet, archives, RAM/hashmap and tree).
Changing public field representation must include these callers and their tests.
