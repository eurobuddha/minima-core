# Witness: replacing a list is different from emptying it

A Witness holds the evidence for a transaction: signatures, coin proofs and script
proofs. Its three lists have separate lifetimes. Imagine a student takes a reference
to the signature list, and the node then reads another Witness into the same object.
Java assigns a new signature list. The student's old list still exists with its old
contents. Calling clearSignatures instead empties the very list the student holds.
These two operations can produce the same visible empty Witness while having very
different effects on its callers.

## Read the source before choosing ownership

The source is [Witness.java](../../../upstream-study/src/org/minima/objects/Witness.java)
at commit `5564b9d78978efcaf23679190d05253961fc6e69`. Its readDataStream replaces the
signature list before reading its count. Only after all signatures finish does it
replace the coin-proof list. Finally it replaces the script-proof list. A truncated
signature leaves the old coin and script lists attached to the Witness.

The existing [Signature implementation](../../src/org/minima/objects/keys/Signature.hpp)
already demonstrates the required C++ ownership pattern. Witness reuses it: a
shared record holds three shared list handles. Copying a native Witness retains the
record. A list getter returns its own shared handle, keeping that list alive even
when the record later points elsewhere. Clear methods mutate the current lists.
This preserves the distinction without retaining every historical list forever.

Production callers now dereference owned handles. Loops retain the handle until
iteration finishes, including loops that invoke callbacks. This protects the list
allocation if its Witness replaces the list. It does not implement Java's
fail-fast iteration when that same list is structurally modified during iteration.

## Bytes written before a failure still count

Witness implements the common Streamable interface and appends each child's output
directly. Consider a second coin proof whose coin ID exceeds the allowed hash size.
The signature list, the coin-proof count and the first complete coin proof have
already been written when the second proof fails. Serializing that second child to
a temporary buffer would lose any of its earlier fields on failure. The child
codecs already implement source-order appending, so Witness calls those methods.
Its serialise convenience method collects the same virtual writer's output.

## The experiment

[Python](../../tools/streamable_differential.py) independently constructs the list
counts and child encodings. The [Java oracle](../../support/StreamableOracle.java)
compiles the pinned source; the [C++ harness](../../support/streamable.cpp) calls the
actual native implementation. Both access objects through Streamable references.

For a new Witness with two entries per list, the test cuts the input after every
byte, using initial offsets zero and two, and also supplies trailing bytes. It
checks the exception class, exact consumed offset, which list identities changed,
how many complete children were appended, and the exact bytes of the partial object.
The expected list contents are calculated independently from child boundaries.
The old lists' complete contents must survive every replacement.

A copied Witness and a real TxPoW holding it through setWitness must observe the
same changes. Additional cases clear the original lists, clear detached old lists,
add duplicate evidence, read duplicate evidence directly, and use zero, negative
and integer-wrapped list counts. Signature and script add methods deduplicate;
coin-proof addition does not. Direct wire reads preserve duplicate entries.

Three failure cases put an invalid second child in each evidence category. The
test checks every preceding byte. Existing file write/append/load, MiniData
conversion, SHA3 hashing and NIO message construction also receive Witness through
the common interface. A passing byte comparison does not authenticate a signature;
cryptographic verification belongs to the existing verification tests.

## Evidence and limits

Both full builds pass. Each interface suite passes 3,447 comparisons and 31,650
assertions with the same transcript and empty native stderr. All six affected
regressions pass in both builds. Results and build logs are saved under
[streamable-witness-work-in-progress](../../validation/streamable-witness-work-in-progress/progress.json).
The initial compilation exposed two missed TxBlock accessor adaptations; they were
fixed. Initial comparison assertions passed, but a Python child-count variable
incorrectly overwrote the report's total case counter. That reporting bug was fixed
and an executed-versus-planned case-count gate was added before final validation.

Witness is one more interface integration, not a completed node. Nullable entries,
retained getScript pointers, complete ScriptProof identity, Java collection APIs,
subclass dispatch, custom streams, allocation failures and concurrent or reentrant
list mutation still require work. The mapped file remains implemented_partial and
its full parity remains not_verified.

## Questions for students

1. At EOF immediately before the coin-proof count, which lists have been replaced?
2. Why does clearing an old list after a successful read leave the new Witness intact?
3. Why must a reader append directly rather than call addSignature or addScript?
4. Why can two correct byte encodings still hide an ownership bug?
