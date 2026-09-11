# Reading scalar values into existing objects

MiniData, MiniNumber and MiniString now join Streamable using their existing
codecs. These readers have different failure behavior, despite sharing an interface.

MiniData validates the length before allocating a new zero-filled array. It then
fills whatever bytes are available; EOF can leave a partly filled array visible.
Its expected-size and hash readers apply their source checks before replacement.
The numeric BigInteger cache is deliberately not reset by reads in the Java source.
C++ retains that cache too; comparisons and shifts use it. Atomic shared-pointer
access avoids introducing a race in concurrent read-only numeric access. Concurrent
mutation of bytes and all Java array-reference aliases remain outside this stage.

MiniNumber constructs its whole decimal before replacement, including the existing
64-digit rounding. Its wire reader does not apply the constructor's monetary range
check. MiniString also waits for its complete byte array, then reuses the existing
Java-compatible UTF-8 replacement behavior. The source's mutable charset setting
and complete null/reference APIs still need work.

The common-interface suite compares every small-payload truncation, offsets,
partial output, rejection ordering, cached/uncached numeric observations and actual
file/hash/network-message callers. Malformed UTF-8 and extreme number encodings
exercise the existing decoders through the new in-place entry points. Both complete
builds pass. Each interface suite passes 2,095 comparisons and 12,220 assertions,
with matching transcripts and empty native stderr. All nine affected regressions
pass in both builds. See the [saved evidence](../../validation/streamable-scalars-work-in-progress/progress.json).
Fourteen of 44 explicit source implementers now use the interface; 30 remain.
Full class and node parity remain unverified.

For students: why does EOF after a length prefix alter MiniData but leave MiniString
unchanged? How can the serialized bytes change while getDataValue returns its old
number? Which source line determines when a value becomes visible?

The first differential run found redundant sign bytes leaking back into number
output: Java read 32 bytes of FF as -1 and wrote one FF; C++ retained all 32.
Both BigInt byte factories now call the existing normalise method. New cases
cover redundant positive/negative sign padding and sign bytes that must remain,
through both MiniNumber and MMREntryNumber. The regression compares Java output
and independently constructs the minimal signed integer representation.

Review also found that repeatedly erasing the first sign byte made normalization
quadratic for large entry-number coefficients. The helper now scans the redundant
prefix and erases it once. Four 64 KiB padding cases check zero, -1, +128 and -129,
including sign bytes that must remain. The loop structure establishes linear
work; the tests check the resulting values rather than imposing a timing threshold.
