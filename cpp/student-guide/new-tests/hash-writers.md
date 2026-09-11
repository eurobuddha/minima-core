# Hash writers: two methods with the same byte format

Follow [MiniData.java](../../../upstream-study/src/org/minima/objects/base/MiniData.java)
and [MiniData.cpp](../../src/org/minima/objects/base/MiniData.cpp). The
[Java fixture](../../support/HashWritersOracle.java), [C++ fixture](../../support/hash_writers.cpp)
and [driver](../../tools/hash_writers_differential.py) exercise the real classes.
[Saved results](../../validation/hash-writers-work-in-progress/normal/hash-writers-results.json)
record 1,848 comparisons and 12,752 independent assertions in each build.

## Writing a hash does not calculate a hash

Both MiniData writers encode a four-byte signed length followed by the stored
bytes. The hash writer checks a separate maximum, initially 64 bytes. Ordinary
writing has no such check. The ordinary reader has its own maximum, initially
512 MiB. These limits are public mutable Java integers, so the native definitions
must also permit assignment. Tests change them and restore them after each case.

For a one-byte value 0x44, both base writers produce 0000000144. Equal bytes from
base objects do not mean the two methods can be substituted for each other.
Java subclasses can override them independently. Calling the base hash writer
must not enter a subclass's ordinary writer. C++ therefore exposes a distinct
virtual writeHashToStream and uses a qualified MiniData::writeDataStream call
inside its base implementation. The existing encoder and length checks are reused.

## Make dispatch observable

The fixture's ordinary override appends A1 and records an ordinary call. Its hash
override appends B2 and records a hash call. Each can return, throw, invoke its
base implementation, or invoke the base and then throw. A 65-byte hash override
can return successfully when it does not call the base; placing a maximum check
outside virtual dispatch would incorrectly reject it.

The driver independently constructs expected frames, call traces and errors.
It covers empty values, 63/64/65-byte boundaries and mutable limits. It also cuts
length headers and payloads short. A rejected length retains the old value;
an accepted but truncated payload exposes a zero-filled replacement plus the
bytes already read. Negative ordinary and hash lengths follow different source
branches. Full Java/C++ error text is compared for these represented cases.

## Failure leaves earlier bytes visible

The actual Transaction link and TxBody PRNG/block-ID writers invoke the hash
method directly on the caller's destination. If an override appends B2 and then
throws, B2 must remain alongside the earlier fields. Serializing a child into a
temporary vector before appending it would lose that partial output. The native
serialise and serialiseHash convenience methods intentionally stage their own
result; the Java fixture models that staging explicitly when comparing them.

Magic work uses ordinary writing. The same subclass installed there must produce
an ordinary trace, even though the field describes proof-of-work difficulty.
The field's source writer determines the dispatch, not its name.

## Keep the invoked child alive

A callback can replace its own parent field while its writer is running. Java
keeps the receiver alive during that call. A C++ raw reference could instead
refer to an object whose last shared owner has just been released. Transaction
now retains its link owner locally until writing finishes. TxBody and Magic
already retain their owned arguments through the full expression.

The fixture drops its standalone strong child reference before calling the
container. The override replaces itself with 0x99 and then accesses its own trace
and mode. Both AddressSanitizer and UndefinedBehaviorSanitizer are enabled in the
sanitized build. Weak parent captures avoid creating a test-only ownership cycle.

## Scope and exercises

Address, Coin, Token, MMRData and TxHeader also call the distinct hash writer.
Their remaining value fields do not gain full Java reference identity from this
change. Hash-reader overriding, arbitrary stream backends, mutable byte-array
aliases, allocation failures, concurrent mutation and full node parity remain
open. Passing this lab does not complete any whole-class parity audit.

Predict the bytes retained after a Transaction link override writes B2 and throws.
Why does an ordinary-only override have no effect on the base hash writer? Why
must changing a parent field leave the old receiver alive until its call returns?
