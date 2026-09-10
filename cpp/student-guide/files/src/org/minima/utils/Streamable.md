# Streamable — logic and a future translation

**Teaching status:** Authored source algorithm and proposed validation; native counterpart remains unported.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/utils/Streamable.java](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java) | `src/org/minima/utils/Streamable.cpp` (planned) |

## Teacher's worked explanation

Streamable is a two-method Java interface. It says that an implementing object
can write its representation to a DataOutputStream and read its representation
from a DataInputStream. Both operations may throw IOException. The interface
contains no fields, byte layout, loop, or default implementation.

**Its mapped C++ counterpart is still absent.** Many native objects already have
serialization methods, but that does not mean this interface's translation or
all substitutable stream behavior has been completed.

### What the interface promises—and what it leaves to each class

`writeDataStream(zOut)` writes an object's fields to the supplied stream. The
implementing class determines the exact order and encoding. `readDataStream(zIn)`
changes an existing object by reading those fields back. It is not a factory
method returning a fresh object.

For example, Transaction writes its input list, output list, state list and link
hash, with counts encoded as MiniNumber values. A caller using only Streamable
does not need to know those fields to ask the object to serialize itself. This
is the role of an interface: state a common operation while letting each class
supply the behavior.

The interface alone does not promise atomic reading, rollback after failure,
automatic stream closing, or identity preservation for nested fields. Those
questions require reading the implementing method. Transaction clears lists
before reading new contents, so a failed read can leave partially changed state.
A native implementation that decodes into a temporary and replaces the object
only after complete success would provide different failure behavior.

### What the current C++ code does

Native objects commonly expose `serialise()` returning a vector of bytes,
`readDataStream(span, offset)` for an in-place read, or a static `deserialise()`
that returns a new value. ByteIO.hpp supplies common byte composition and
read-boundary helpers. MiniFile's templated reader checks whether an object has
an in-place method; otherwise it assigns a newly deserialized value.

These are useful existing building blocks, but their signatures are not a
complete common native Streamable contract. Static creation and in-place mutation
must be distinguished, especially when callers retain references to existing
objects or inspect state after an exception. Simply adding an empty `.cpp` file
would fill a mapping slot without solving that behavior.

### How to test an interface translation

An interface has no standalone algorithm to run. Its tests should exercise
representative implementations **through the common interface or adapter** and
check that callers can rely on the promised operations.

Existing object differential tests compare many native wire formats against
Java. They are evidence for those objects, not a passing test of this missing
mapped interface. Future Streamable validation should write different object
types through the native common path, compare exact Java bytes, read into existing
objects, and check offset advancement and partially changed state on truncated
input. A stream that fails after a chosen byte count can reveal whether writes
and exceptions occur in the same order. Closing and ownership must be observed
where an implementing class or caller actually specifies them.

Do not invent a universal wire layout for Streamable: the source interface does
not define one. Reuse the proven serializers and preserve each class's format.

**Exercise:** explain why `object = T::deserialise(bytes)` can differ from
`object.readDataStream(bytes, offset)` even if both produce identical objects
when the input is valid. Use a truncated transaction and a retained reference
as your counterexample.

## Dependencies and reading map


Pinned Java SHA-256: `c88b27270aec6207289dab7fabb149904ce0e2d286e1cf3356a066e046c4a933`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
