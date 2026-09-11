# TxHeader: shared references and run-length decoding

The [source class](../../../upstream-study/src/org/minima/objects/TxHeader.java)
and [C++ class](../../src/org/minima/objects/TxHeader.cpp) describe the portion of a
TxPoW retained after its body is discarded. The [Java fixture](../../support/TxHeaderOracle.java),
[native fixture](../../support/txheader.cpp) and [driver](../../tools/txheader_differential.py)
call the actual implementations. Results are recorded in the
[checkpoint](../../validation/txheader-work-in-progress/progress.json).

## Read the source's assignments literally

Each public field holds an object reference. Copying a header reference shares
that header; retaining getBodyHash keeps the previous child alive after replacement.
The C++ shared record and owned field accessors reuse the TxBody/Magic pattern.
An array has its own identity, separate from its entries. Replacing the array
must not invalidate an older retained array. A fresh header initializes a distinct
empty MiniData for every slot; a decoded run shares one MiniData across its slots.

MAIN_NET and TEST_NET are replaceable public references. Existing headers keep
their previous chain object when MAIN_NET is replaced. Mutating that previous
object affects all headers still pointing to it. Three other defaults refer to
fixed global objects: Crypto.MAX_HASH, MiniNumber.ZERO and MiniData.ZERO_TXPOWID.
The C++ compatibility references now address owned mutable objects; their shared
accessors let TxHeader retain those actual objects. Other source constants and
value convenience boundaries still need their own identity review.

## Run length and array length are different limits

A run consists of a MiniByte count and one length-framed hash. The decoder's outer
loop asks whether it has filled the configured number of cascade levels. Its
inner loop assigns every entry in the decoded run to the actual retained array.
If the configured limit is four but a six-entry array receives a run of six,
the source succeeds and fills all six entries. With a four-entry array, it assigns
four entries and then throws on the fifth. Rejecting the run in advance would
lose those observable assignments. A zero-length run consumes its encoded hash
but advances no array slots, so later input must still supply progress or EOF.

The tests cross null, empty, short and long arrays with these runs. They cut each
represented stream at every byte, both at offset zero and after a prefix. Python
independently models completed fields, stream offsets, partial array contents,
field identities and the entire pairwise array-alias matrix. Java/C++ equality
alone would not catch a shared mistaken expectation.

## Factories finish before field replacement

Five fields precede the array. Five more follow it, including nested Magic.
A child factory must complete before replacing its field. Truncated nested Magic
therefore leaves the old Magic in the header even though earlier header fields
have changed. The retained header alias must observe the same partial state.

## A writer keeps its receiver alive

A nested writer can append one byte, replace its own parent field, and throw.
Earlier bytes and the child's byte remain in the destination. Direct Streamable
and hash-writer calls preserve those prefixes. Owned arguments keep the invoked
child alive through its call; the fixture drops its separate strong reference
before invoking the parent. Numeric, data, Magic and superparent hash overrides
exercise the different dispatch paths. A null leading parent also participates
in the source's run-marker logic; blanket null prevalidation would be different.

The converter catches and logs IOException only. Null input and array-bound
exceptions escape. Dedicated converter cases check these categories, successful
round trips, ignored trailing bytes and captured log categories.

## What this does not prove

The native TxPoW still exposes several reference/value convenience boundaries
that do not fully model retained Java child references. Arbitrary stream backends,
non-stream scalar subclass overrides, allocation failures and concurrent mutation
remain open. This is an incremental behavior correction, not another newly mapped
Java file and not certification of complete class or node parity.

Exercise: after a run of six is read into a four-slot array, which entries share
an object and where has the stream stopped? Why does the answer differ from
checking the run length before assigning anything?
