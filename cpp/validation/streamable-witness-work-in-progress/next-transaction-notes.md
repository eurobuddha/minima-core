# Next integration: Transaction (not changed in this checkpoint)

Pinned Transaction.java and current Transaction.hpp/.cpp have been read, including
all codec, state-list, cached-ID and monotonic-flag methods. The sibling
/Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/objects/Transaction.hpp/.cpp
has also been read. It cannot replace the current implementation: its validity
check requires outputs instead of inputs, inputsBalance demands equality rather
than allowing burn, computeCoinID hashes raw ID plus a fixed-width integer rather
than two Streamable objects, state insertion omits sorting, and pointer-only
readers lack span bounds and use a 65,536-byte hash limit instead of 64.

Current native Transaction uses direct vectors. Source replaces all three lists
before any count read; removeStateVariable replaces the state list only if it finds
a matching port, removes all matches, and sorts. Source clearState mutates the
current list. Current reader/erase-remove mutate existing allocations instead.
Writers currently buffer children. calculateStateSize omits source exception
logging and catches JavaError/bad_alloc too broadly. calculateTransactionID needs
review against Crypto's source catch-and-null boundary and later null-ID rejection.

Unlike Witness, Transaction has three public mutable boolean cache fields. Shared
container ownership must include these flags and the cached ID, rather than sharing
only lists. Native direct callers were located in TxPoWChecker.cpp, consensus.cpp,
checker_logs.cpp, SharedCheckerFixture.hpp, transaction_commands.cpp and
wallet_commands.cpp. Complete caller/list-return API review is still required
before choosing a minimal adaptation. Do not add reference members whose copy
assignment cannot rebind, or silently turn Java aliases into independent snapshots.

TxBody and TxnRow retain shared Transaction/Witness owners. TxBody reads into its
existing child objects; TxnRow creates each new child at the source assignment
point. TxPoW has both shared and value setter overloads. Witness copy alias behavior
is now tested through its real TxPoW setter. Apply equally meaningful observations
to Transaction, including monotonic flags, retained old lists and derived ID timing.
