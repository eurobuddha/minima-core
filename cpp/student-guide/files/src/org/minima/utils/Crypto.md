# Crypto — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/Crypto.cpp) · [C++ header](../../../../../../src/org/minima/utils/Crypto.hpp)

## Purpose

This is a small hashing facade. Its default digest is SHA3-256, while hashSHA2 explicitly selects SHA-256. Object hashes depend on the exact serialized bytes, including length prefixes and the order of fields and objects.

## Follow the algorithm

1. Create constants representing the largest 256-bit unsigned value, and lazily provide a shared Crypto instance.
2. hashSHA2 and hashData instantiate the corresponding digest, feed the entire input byte array, finalize into a digest-sized output buffer, and return it.
3. hashObject writes one Streamable object into a byte stream, flushes it, hashes the resulting bytes with the default digest, and wraps those digest bytes in MiniData.
4. hashObjects and hashAllObjects write objects consecutively into one stream before hashing. They do not hash each object separately and then concatenate hashes.
5. The Java methods catch Exception, call MinimaLogger, and return null when hashing or serialization fails. Error subclasses and failures from the logger are different paths.

## How the C++ translation works

The native facade reuses MiniData SHA2/SHA3 and wire::join. Template methods accept objects exposing serialise(), so stream serialization becomes a native byte vector. The singleton is a function-local static. MAX_HASH and MAX_VAL are present; the Java mutable MAX_VALDEC field is not exposed by this header. The native templates return values and allow failures to propagate, so Java catch/log/null behavior is not fully implemented.

## Understanding the tests

The inspected object fixture creates a TxHeader whose body hash is hashObject(MiniData("0x1234")) in both languages. The object driver compares header wire bytes and JSON for multiple timestamps and super-parent layouts; an incorrect hash changes those outputs. The fast-stream Java fixture also hashes active buffer bytes through Crypto.hashData after each operation, comparing against the native digest. These are concrete hash integration checks. They do not directly exercise every facade overload or serialization failure.

- [Java header/hash fixture](../../../../../../support/ObjectOracle.java)
- [Native header/hash fixture](../../../../../../support/objects.cpp)
- [Object comparison driver](../../../../../../tools/object_differential.py)
- [Direct Java hashData use in the stream fixture](../../../../../../support/FastByteArrayOracle.java)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The strongest missing facade tests would use a controlled Streamable whose write fails after writing a prefix, a null input, and a logger that itself fails. They should compare returned null versus escaped exception and recorded logs. For normal inputs, add an explicit assertion that hashAllObjects(a,b) hashes serialise(a)||serialise(b), preserving order.

## Check your understanding

Why does hashing the raw bytes0x1234 differ from hashing a serialized MiniData that contains those bytes? What extra bytes does the serializer contribute?

Pinned Java SHA-256: `fcbd60a6f3fd72897c0820d25a14b3b2a28c57179ac43de358254705de2b4df3`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).

## Nullable Transaction hashing

The new hashObjectOrNull adapter preserves the pinned hashObject catch/log/null boundary for Transaction. It reuses the existing serializer and SHA3 implementation, lets JavaError and allocation failures escape, and logs caught exceptions. Other native hash convenience APIs remain under review. See the [Transaction lab](../../../../../new-tests/streamable-transaction.md).
