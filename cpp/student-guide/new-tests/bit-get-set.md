# Bit positions: a round trip can hide two mistakes

Read the pinned [BITGETTests](../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITGETTests.java) and [BITSETTests](../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITSETTests.java).

Java BitSet numbers bit zero as the least significant bit of the first byte. The test constructs a single set bit with BitSet, converts it to bytes, and asks BITGET for that position. It also constructs all 64 bits set except the chosen bit and expects FALSE. Each loop runs positions 0 through 62; position 63 is not included in these original loops.

BITSET starts with the same two inputs. Clearing the sole set bit yields an empty byte array, whose HexValue string is empty. Filling the only cleared bit yields 0xFFFFFFFFFFFFFFFF. BitSet.toByteArray removes zero bytes from the end; the native function preserves this behavior. It does not promise a fixed output width.

The sibling project's single set-then-get test cannot establish bit numbering: if both functions use the same wrong convention, the round trip still succeeds. The pinned examples provide independently constructed expected bits and strings. We reuse the current BITCOUNT test adapter and actual ConstantExpression, Contract, HexValue and functions. No production algorithm is rewritten for these mappings.

Each mapped test checks a fresh function and registry lookup, then all 126 valid examples with separate result-type and result-value assertions. BITGET checks two wrong argument counts, eight invalid positions and six wrong type combinations. BITSET checks four wrong counts, two invalid positions and nine wrong type combinations. Catching only ExecutionException prevents a generic C++ error from passing a rejection assertion.

Expected successful assertion counts are 272 for BITGET and 271 for BITSET in each runtime. The wrapper calls all three unchanged Java test methods; the C++ counterpart preserves their inputs and expected results. Existing KISS comparisons cover additional boundary positions and fractional inputs. These tests do not establish complete function/API parity. All four registered entries pass in both normal and ASAN/UBSAN builds. Both full C++ builds pass; the Java wrappers execute the original three methods each. The reported assertion counts are confirmed by the source loop bounds and active rejection cases.
