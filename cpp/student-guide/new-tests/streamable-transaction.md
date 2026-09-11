# Transaction: lists, cached identities and shared flags

A transaction's serialized bytes do not include everything about the object.
Its transaction ID and three validation flags are cached state. Two references to
one Java Transaction observe the same caches and the same current lists. A C++
translation that copies only the bytes or shares only the lists can still disagree
with Java when another caller changes a flag or recalculates the ID.

## Reusing the established ownership pattern

The [pinned Transaction](../../../upstream-study/src/org/minima/objects/Transaction.java)
uses mutable lists and object references. The native translation reuses the shared
record and owned list handles already tested in Witness and Signature. The record
now includes input/output/state lists, link hash, cached ID and validation flags.
Native flag accessors return lvalues, so `txn.mIsValid() = true` updates the shared
record. This is a C++ syntax adaptation of Java's public field assignment.

List getters return owned handles. A caller can retain the old list safely when a
read assigns a new list to the transaction. The input, output and state lists are
all replaced before the first input count is read. This differs from Witness,
which replaces its lists one category at a time. Completed child reads are appended;
an incomplete child is not inserted. The link hash changes only after its complete
read, followed by a new transaction-ID calculation. Reading leaves the flags alone.

State removal has another observable distinction. Removing a missing port leaves
the list object and its ordering intact. Removing a present port constructs a new
list, removes every matching entry and sorts the survivors. clearState empties the
current list in place. addStateVariable removes the old port before inserting and
sorting the replacement. Direct wire reads may still contain duplicate state ports.

## Why failed hashing changes the ID

Java Crypto.hashObject catches Exception, logs it and returns null. Transaction
assigns that result to its cached ID and then rejects null with
IllegalArgumentException. The old ID is therefore lost from the transaction, but a
caller holding the old ID object retains it. Java Error escapes the hashing method,
so the assignment does not happen and the old ID stays attached.

The native `Crypto::hashObjectOrNull` adapter reuses the existing byte serialization
and SHA3 implementation with that catch/log/null boundary. JavaError and bad_alloc
escape. Other native hashing convenience methods still need their own full failure
behavior review. Transaction's state-size calculation also now logs its source error
message and caught exception; Error and allocation failures escape that catch.

Returning a nullable ID exposes caller behavior that a value-only getter hid.
Wallet/TreeKey signing uses existing nullable pointer overloads. TreeKey verification
now accepts a nullable data pointer and preserves the source check order: an empty
or overlong signature list, or a mismatching root, returns false before reading the
message. A matching final proof reaches the null-message error. The actual TxPoW
checker with no signatures returns true without dereferencing a null transaction ID.
Native value adapters explicitly reject null when a caller actually needs a value.

## What the experiment observes

The existing [Python driver](../../tools/streamable_differential.py) independently
encodes a transaction with duplicate state ports. The [Java oracle](../../support/StreamableOracle.java)
and [native harness](../../support/streamable.cpp) read a different transaction into
it through Streamable. Every byte truncation, two initial offsets, trailing bytes
and malformed count prefixes exercise partial state and failure timing.

For every valid prefix, the independent model predicts each complete child retained,
the current link hash, serialized partial state, state-list byte size and consumed
cursor. It also predicts whether the cached ID should be the old hash or a new SHA3
of the canonical serialization. Copied native handles and a real TxPoW setter must
observe the same flags and bytes. Retained old lists, ID and link must survive.

Separate experiments remove existing/missing ports, replace duplicate ports, clear
state, clear cache flags, and invoke JSON's ID recalculation. Three invalid writers
check the exact output before a bad input, output or link hash. A test subclass
throws IOException, a runtime exception or AssertionError from its writer to test
the real hashing/assignment boundary. A null link also tests a failure in the actual
Transaction writer. Direct checker/TreeKey calls test null-ID rejection order.

The original TransactionTests remains a readable 5-input, 2/3-output example.
Affected object, VM, consensus, transaction database, wallet, send, token and search
workflows are rerun. Saved full SPHINCS signatures are replayed through actual state
installation and KISS VM execution; this run does not regenerate the signing forest.

## Evidence and remaining work

[Checkpoint evidence](../../validation/streamable-transaction-work-in-progress/progress.json)
records both passing full builds and 21 selected CTest suites. Each interface run
passes 3,782 comparisons and 39,522 assertions with an identical transcript.
Both SPHINCS replays pass 268 comparisons and 1,677 assertions. The initial interface check
passed; review then added the nullable verification path and its direct checks.

The class remains implemented_partial, with full parity not_verified. Nullable list
elements, retained StateVariable pointers and child identity, complete Java collection
operations, fail-fast iteration, global shared ZERO_TXPOWID identity, arbitrary
subclasses and custom streams remain open. Value adapters and all downstream
null/error paths still require complete caller audits. The state-size exception
logging implementation follows the source, but its failing child/allocation paths
are not exercised by the current representable fixtures. Concurrency and allocation
failure timing are not certified by these tests.

## Questions for students

1. At EOF before the first input-count byte, which lists and caches have changed?
2. Why does removing a nonexistent state port leave an unsorted list unsorted?
3. Why does IOException erase the cached ID while AssertionError preserves it?
4. Why must an empty witness avoid dereferencing the transaction ID during checking?
