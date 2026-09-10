# BITSETTests — Changing one bit in a byte sequence

Java source: [test/org/minima/tests/kissvm/functions/maths/BITSETTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITSETTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Use a value, position, and boolean choice to set or clear a bit and compare the resulting hex. The file also contains comments questioning historical implicit conversion; comments must not be counted as successful validation.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITSETTests.java#L29) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITSETTests.java#L46) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 6 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITSETTests.java#L96) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 15 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 66](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITSETTests.java#L66):

```java
assertEquals("", ((HexValue) res).toString());
```

[Source line 85](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITSETTests.java#L85):

```java
assertEquals("0xFFFFFFFFFFFFFFFF", ((HexValue) res).toString());
```

[Source line 104](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITSETTests.java#L104):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. The mapped Java and native tests now execute these source expectations successfully in both builds.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.hex.BITSET](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITSET.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The mapped counterpart reuses the BITCOUNT adapter, actual function factory/registry, ConstantExpression and Contract parameter checks. It preserves the original 63 loop positions, both bit patterns, expected type/string values and all rejection arguments. The native fixture constructs the same byte arrays as Java BitSet; it does not use BITGET or BITSET to calculate expected values.

All three unchanged Java methods and the native counterpart pass in normal and ASAN/UBSAN builds. There are 271 successful assertion calls per runtime: four constructor/registry assertions, 252 valid-value assertions and 15 typed rejection assertions. Each rejection catches only ExecutionException. This count is distinct from the static call-site counts above.

- [Native test](../../../../../../../../../test/org/minima/tests/kissvm/functions/maths/BITSETTests.cpp)
- [Java wrapper](../../../../../../../../../support/BITSETOriginalTests.java)
- [Comparison runner](../../../../../../../../../tools/script_value_original_tests.py)
- [Bit experiment](../../../../../../../../new-tests/bit-get-set.md)

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `fee148c83a23192f7f758ecb2cdeac2f7cf6a924a5569227656e4c595baa64e4`.
