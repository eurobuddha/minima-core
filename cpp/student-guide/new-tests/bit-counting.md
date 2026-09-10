# Counting bits: a lookup table is also observable state

Read [BITCOUNT.java](../../../upstream-study/src/org/minima/kissvm/functions/hex/BITCOUNT.java) and [BITCOUNTTests.java](../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java).

## The algorithm

For each input byte, Java converts its signed byte value to an unsigned index with `& 0xFF`, looks up BITSPERBYTE[index], and adds that integer to the total. The return value is a NumberValue. Eight one bits in FF therefore contribute eight; FF8 contains nine one bits after hex decoding pads the leading nibble.

The table reference is final in Java, but its elements are mutable. A native popcount instruction gives the same initial counts while ignoring later table edits. The port now exposes the same 256 entries and reads them in totalBits. Its unsigned accumulator preserves Java's wrapping 32-bit sum, including unusual edited entries.

## Translate the test carefully

The original test independently builds counts for 0..255 by repeated division by two. It compares each generated count with the published table and calls fail only on a mismatch. Those are 256 checked predicates, not 256 successfully executed JUnit assertion calls.

It also executes four constructor/registry assertions, 26 result assertions for thirteen hex inputs, and four exact ExecutionException assertions for missing/wrong arguments. That is 34 successful assertion calls plus 256 table comparisons per runtime. Preserve the thirteen inputs, including odd-length hex strings and sixteen FF bytes.

A supplemental check changes a table entry, calls both totalBits and the function, then uses entries that overflow a signed integer. Comparing the printed Java/C++ observations verifies that the exposed table is used and that overflow wraps. This is separate from the unchanged original tests.

## Reuse and evidence

Reuse the existing native function, Contract, ConstantExpression and INCTests assertion pattern, plus script_value_original_tests.py. The sibling BITCOUNT uses a compiler popcount too; it cannot supply the published mutable table behavior.

Both full builds pass. The original/native test entries and the 4,471-program KISS VM differential suite pass in each build: normal 12.90 seconds and AddressSanitizer/UndefinedBehaviorSanitizer 21.74 seconds. The supplemental observations are 84, 42 and -2147483648 in both runtimes. Neither these small examples nor the table check establish all nullable API and concurrent mutation behavior.
