# Streamable — one contract, many byte formats

**Teaching status:** Authored source, translation and test explanation. Full integration remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java interface](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java) · [C++ interface](../../../../../../src/org/minima/utils/Streamable.hpp) · [C++ implementation](../../../../../../src/org/minima/utils/Streamable.cpp)

## What the interface promises

Streamable declares two operations. writeDataStream appends an object's fields to
a supplied stream. readDataStream reads those fields into the existing object.
The interface defines no fields, wire format, rollback, stream ownership or closing
policy. Each implementing class determines its own byte layout and failure state.

The C++ class has two corresponding pure virtual methods. It uses the existing
byte-vector output and span/offset input conventions. A virtual destructor permits
safe ownership through a base pointer. The small serialise convenience method
collects bytes by calling the virtual writer; it lets the existing generic native
callers use a Streamable reference without knowing its concrete class.

## Reuse and integration

MiniByte, MMRProof, HORSTSignature, FORSSignature and SPHINCSSignature now inherit
from Streamable. The four proof/signature implementations retain their existing
writer and in-place reader bodies. MiniByte appends its stored byte and reuses
ByteIO's byte reader. Its static factory calls the same in-place reader.

The [MMR/signature follow-up](../../../../../../student-guide/new-tests/streamable-mmr-signatures.md)
adds MMRData, MMREntryNumber, MMREntry, MMRProofChunk, SignatureProof and Signature,
preserving each reader's assignment timing and Signature's retained-list behavior.

MiniFile's generic reader already selects readDataStream when available. Given a
Streamable reference, that call now dispatches to the actual object's reader.
Crypto, MiniData conversion and NIOManager similarly use the convenience serializer,
which dispatches to the actual writer. The interface has real callers and concrete
implementations; it is not an empty inventory entry.

The sibling DataStream was inspected but not substituted: its two-byte string
length and read-boundary behavior differ from these already-tested codecs.

## Why failed reads matter

A Java reader may change some fields before EOF. For example, a proof can replace
its block time and clear its chain before reading the next count. A signature can
replace one nested object before another nested read fails. Decoding into a new
object and assigning only on success would hide those changes. The interface
therefore exposes the existing in-place methods directly.

## Understanding the tests

The [interface lab](../../../../../../student-guide/new-tests/streamable-interface.md)
compares calls through Java and C++ base references. It checks prefixed output,
every truncation of small encoded objects, cursor advancement, trailing bytes and
partial reserialization. A synthetic writer appends some bytes and then throws,
showing that the common interface preserves partial output and exceptions.

Successful file write/append/load, MiniData conversion, single/pair hashes and
network-message construction use the actual generic helpers. Independent byte
concatenation and SHA3 calculations check their outputs. Both complete builds
pass. Each expanded interface suite passes 1,299 comparisons and 6,290 assertions with
matching transcripts and empty native stderr; all six affected regressions pass.

## Remaining work

The pinned source declares 44 explicit implementing classes, including the nested
MMRProofChunk. Eleven are integrated; 33 still need native interface adoption and
behavior review. Inherited subclasses also require validation when their bases
join the interface. The commented P2PSwapLink sketch is not an active implementer.

Vector/span backends do not prove arbitrary Java stream, blocking I/O, custom
subclass, allocation-failure, concurrency or full-node parity. Existing individual
codec tests remain useful evidence, but do not prove all common-interface behavior.

For students: explain why assigning a completely deserialized temporary can differ
from reading into an existing object. What state and cursor observations would
reveal that difference after a truncated input?

Pinned Java SHA-256: `c88b27270aec6207289dab7fabb149904ce0e2d286e1cf3356a066e046c4a933`.
[Mapping](../../../../../../source-map.json) · [Limitations](../../../../../../REVIEW.md).
