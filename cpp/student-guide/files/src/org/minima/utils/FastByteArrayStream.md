# FastByteArrayStream — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/FastByteArrayStream.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/FastByteArrayStream.cpp) · [C++ header](../../../../../../src/org/minima/utils/FastByteArrayStream.hpp)

## Purpose

This stream avoids allocating an entire expected message size immediately. It keeps a backing buffer plus a count of valid bytes. Capacity and logical length are different: newly allocated unused space must not appear in toByteArray.

## Follow the algorithm

1. The constructor asks ByteArrayOutputStream for at most 256KiB of initial storage. Negative requested sizes are rejected by the inherited constructor.
2. writeData returns immediately for a negative offset, before dereferencing the input array. Otherwise it computes count+len with Java int arithmetic.
3. If more capacity is needed, grow by the larger of 256KiB and len. Growth happens before the final array-copy bounds validation; a failing copy can therefore leave a larger capacity.
4. System.arraycopy copies into the buffer at count, supporting overlapping ranges, and count is increased only after a successful copy. Out-of-memory during growth logs details, requests GC, and becomes IllegalArgumentException.
5. The inherited ordinary write/reset/readout methods retain ByteArrayOutputStream behavior. writeData itself is unsynchronized, so inheriting synchronized methods does not make every operation thread-safe.

## How the C++ translation works

The native class implements inherited stream methods explicitly using a byte vector, logical count, and recursive mutex for ordinary operations. The fast path retains unsigned 32-bit wraparound to emulate Java int arithmetic, preserves the negative-offset no-op and growth-before-validation order, and uses memmove for overlap. Normal write uses a separate doubling-capacity policy. Reset keeps capacity; close/flush are no-ops. UTF-8 and high-byte string conversion reuse native compatibility helpers.

## Understanding the tests

The driver sends 190 operation sequences, including initial capacities around 262144 and copy lengths crossing that boundary. After every action, both fixtures emit result/error, size, capacity, and a hash of active bytes. This catches a port that returns the right bytes but grows at the wrong time. Cases include null arrays, negative offsets/lengths, self-buffer copies, reset, close, output, text conversion, and signed-int overflow. Exceptions are reduced to ERROR; comparison uses the complete emitted sequence.

- [Java fast-stream fixture](../../../../../../support/FastByteArrayOracle.java)
- [Native fast-stream fixture](../../../../../../support/fast_bytes.cpp)
- [Fast-stream differential driver](../../../../../../tools/fast_bytes_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The fixture explicitly excludes out-of-memory and other charset overloads. The native OOM path uses a shorter std::clog message and has no Java GC equivalent, so it must not be described as full failure parity. Proposed fault-injection tests should examine logger failure and state after allocation failure without allocating uncontrolled amounts of memory.

## Check your understanding

Why can a negative offset and null source return successfully from writeData, while an ordinary write with the same arguments fails? Explain how a failed copy could change capacity but leave size unchanged.

Pinned Java SHA-256: `2ebf21c1eb6ba1e879039dd93e11fd3bfbe8be29f6a06eb759d582f81753c87c`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
