# TxBody: who owns a child while it is reading?

A TxBody groups a normal transaction and witness, a burn transaction and witness,
random bytes, difficulty and a list of referenced transaction IDs. Its wire format
is only part of its behavior. Java callers can retain, replace or null its fields.
A failed read can leave a mixture of new fields, partly changed children and old lists.

## Reusing the existing implementations

The port keeps the current MiniData/MiniNumber codecs, OS-backed random-data helper,
and Transaction/Witness readers. TxBody now inherits the existing Streamable
interface. Its shared Data record follows the pattern already used by Transaction
and Witness. Public-field accessors return lvalues for assignment, so native
`body.mTransaction() = child` changes the field shared by all copied body handles.
The extra parentheses are C++ syntax; ownership follows the Java reference.

Every body field is a nullable owned pointer. The ID list owns nullable pointers to
MiniData objects. It can retain duplicate references and null entries. A reader
assigns a newly decoded ID only after that ID finishes reading. A caller holding an
old list or ID keeps that object alive after the body replaces its current list.

The sibling TxBody was inspected, but its unbounded pointer readers, different count
handling and timestamp-seeded generator do not supply the pinned behavior. Reusing
the current tested codecs preserves their checks rather than importing those gaps.

## Walk through a failed read

1. Read a complete PRNG using the bounded hash codec, then replace the PRNG object.
2. Read a complete difficulty with ordinary MiniData framing, then replace it.
3. Invoke each of the four existing child readers in source order. Do not replace
   those child objects with successfully decoded temporaries.
4. After all four child reads finish, allocate a new ID list before reading its count.
5. Append each completely decoded ID. A later incomplete ID is not appended.

An EOF at the beginning of the first transaction can therefore replace PRNG and
difficulty and clear that transaction's lists, while leaving the other children
and the old ID list intact. An EOF at the ID count has already replaced the ID list.
The independent Python model predicts these different states for every small-vector
truncation and checks the consumed cursor and surviving bytes.

## Why keeping the child alive matters

The reentrant-reader fixture installs a Transaction subclass whose reader replaces
`parent.mTransaction` with a different object and then continues reading. Java's
current receiver remains alive. C++ must retain a local shared_ptr for the duration
of that call; dereferencing the field without retaining an owner could destroy the
reader while it is executing. The test checks successful and truncated reads,
including bytes produced by the original child and the replacement left in the body.

ID loops similarly copy each shared_ptr into the loop variable. Replacing a list
entry must not silently replace the object that the current Java loop variable names.
The [iteration follow-up](txbody-id-iteration.md) replaces vector traversal with the existing checked storage and tests actual consumer callbacks. The complete inherited collection API remains open.

## Writers, JSON and randomness

Writers append each child directly to the supplied buffer. Four injected child
writers append byte 42 and throw IOException. The independent expectation includes
all prior body fields and that final byte. Null fields/ID entries and oversized
hashes check the failure prefix of the real body writer.

JSON calls Transaction.toJSON, which recalculates its ID. A null witness can fail
only after the preceding transaction has recalculated its ID. The fixtures retain
old ID objects and check exactly which IDs changed before each null-field failure.

resetRandomPRNG calls the real random-data helper. The test checks the 32-byte length
and replacement identity, then normalizes only those independently random bytes for
comparison. That shape check is not a statistical or cryptographic randomness audit.

## Real containing objects and tests

TxPoW now returns owned body and block-ID-list handles. A retained body remains usable
after clearBody. With no body, each getBlockTransactions call creates a distinct
empty list. Actual chain, checker, search, network and command consumers retain the
list while using it. The fixture also reads with aliased main/burn child objects.

The existing object and transaction-check suites remain relevant. The expanded
[interface driver](../../tools/streamable_differential.py) uses the actual
[Java oracle](../../support/StreamableOracle.java) and
[native harness](../../support/streamable.cpp), plus independent wire predictions.
[Checkpoint evidence](../../validation/streamable-txbody-work-in-progress/progress.json)
records the ownership checkpoint results. The initial run's observations matched but a loop overwrote
the driver case counter; its accounting gate rejected that report. The corrected
counter and added JSON/reentrancy cases pass both final common-interface runs (7,623 comparisons and 95,664 assertions per runtime); broader checkpoint regressions are tracked separately.

## Remaining work and exercises

This is still an implemented_partial port with parity not_verified. Full Java
inherited collection APIs, arbitrary custom streams/subclass interactions,
allocation failures, concurrency and shared Crypto.MAX_HASH identity remain open.
TxPoW's complete shared-record and nullable getter/hash behavior remains under review.
Native H2 is an existing external AOT dependency, outside C++ sanitizer instrumentation.

Predict the field/list identities after an EOF at each numbered read stage above.
Then explain why a child that removes its own parent reference must finish on the
original receiver, and why a successful wire round trip would miss that bug.
