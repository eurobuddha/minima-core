# Magic: values, shared objects, and interrupted streams

This lab follows [Magic.java](../../../upstream-study/src/org/minima/objects/Magic.java)
and its [native counterpart](../../src/org/minima/objects/Magic.cpp).
The [Java fixture](../../support/MagicOracle.java), [C++ fixture](../../support/magic.cpp)
and [driver](../../tools/magic_differential.py) call the actual implementations.
Validation results belong to the [checkpoint evidence](../../validation/magic-work-in-progress/progress.json).

## Begin with the arithmetic

A miner can propose a capacity, but the next block moves gradually toward it.
For each capacity, first clamp the proposal between half and twice the current
value. Then apply the fixed lower limit. Finally compute
`(16383 × current + proposal) / 16384` with the existing MiniNumber arithmetic.
For example, current KISS operations 1024 and proposal 2048 produce 1024.0625.
The returned object's desired values reset to the defaults. They are separate from
its newly calculated current values.

The work threshold uses integer arithmetic. Larger thresholds permit easier work,
so its hard limit is an upper bound. The tests compute independent expectations
with Python Decimal and integers, in addition to comparing Java and C++ results.
They include negative capacity inputs, zero work, leading zero bytes and wide work
values. Reading a stored value does not automatically clamp it.

## A reference is more than a copied number

Java's eight public fields contain object references. Two fields can refer to the
same MiniNumber; replacing one field does not replace the other. Mutating that
MiniNumber through its stream reader, however, changes what both references see.
Default current and desired fields share the public default objects across Magic
instances. A final reference fixes which object is referenced; it does not make
that object's contents immutable.

C++ uses the already established shared record pattern from TxBody. Copies of a
Magic wrapper retain the same record. Each child has its own shared owner, and
lvalue field accessors replace the appropriate owner. Getters return an owner,
which keeps a child alive after a field or the original Magic variable is replaced.
The fixture also installs the Magic through the real TxPoW setter and checks that
changes remain visible there. A separate new Magic record does not acquire those
changes merely because an older record was retained.

## Read failure is observable state

The source reads eight factories in order. A factory must finish before its result
replaces its field. If the first three numbers finish but the following work value
is truncated, those three fields have changed and the other five retain their old
objects. This differs from rejecting the whole object without changing anything.

The driver cuts a valid stream at every byte, with zero and nonzero starting
offsets. It independently computes the completed field boundaries, expected
remaining fields, retained old values and final offset. It then writes the partial
object back out. Malformed lengths distinguish EOF, IOException,
NumberFormatException and NegativeArraySizeException. A 65-byte work value is
accepted because Magic uses ordinary MiniData, whose limit differs from a hash
field's 64-byte limit.

## Write failure must retain its prefix

Writers append directly through Streamable. If a later field is null, all earlier
field bytes remain in the destination. A test child can append one byte and throw
an IOException; that byte must remain too. A second version replaces its own field
while writing. Retaining the invoked child through the call prevents C++ from
using a destroyed object. Sanitizer validation exercises these lifetime cases.

## Why the comparison has four local booleans

Java's checkSame evaluates all four current comparisons before combining them.
If the first comparison is false and a later field is null, the method still
throws. One chained C++ `&&` expression would skip that later comparison. The
fixture crosses unequal and null positions in both operands. Desired fields are
ignored; work equality compares bytes, including leading zeros.

## Run the actual demonstration

`minima_magic --demo` calls the translated Magic.main. It performs all 86,400
updates, refreshes the desired KISS value every 1,000 updates, and prints every
50 updates. The driver runs the pinned Java main separately and compares all
1,729 output lines byte for byte. `--skip-demo` is an explicitly limited quick
check, not evidence that the full demonstration passed.

## What remains outside this evidence

The containing TxHeader and TxPoW still have unfinished ownership and stream
behavior. Non-stream MiniNumber/MiniData subclass overrides, arbitrary stream
backends, allocation failures and concurrent mutation need further work. These
checks do not establish complete node parity or completion of the 457-file port.

Exercise: after reading only the first two complete fields, which getter owners
must differ from the saved old owners? Why can a saved old third field remain
usable after the Magic variable itself is released?

## Current measured checkpoint

Both focused suites pass 326 programs and 2,412 independent assertions. Both
common-interface suites pass 7,705 comparisons and 96,172 assertions across
24 explicit source classes. All 17 selected normal regressions and four selected
sanitizer regressions pass. The first full native demonstrations each timed out at 1,200 seconds. Their
reports are preserved. Replays use saved checkpoint binaries and a 7,200-second
limit per runtime, retaining partial output. Their final reports must be checked
separately before claiming demo parity. The saved
initial failures explain two corrected fixture observations, not production fixes.
