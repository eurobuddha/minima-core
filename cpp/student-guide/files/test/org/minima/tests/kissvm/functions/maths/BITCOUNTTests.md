# BITCOUNTTests — Counting set bits

Java source: [test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Apply the function to hex bit patterns and compare a numeric population count. Each1 bit contributes one independently of its position. Invalid types and parameter counts are a separate part of the contract.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java#L28) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java#L45) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 40 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java#L214) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 4 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 74](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java#L74):

```java
assertEquals("0", ((NumberValue) res).toString());
```

[Source line 85](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java#L85):

```java
assertEquals("1", ((NumberValue) res).toString());
```

[Source line 222](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.java#L222):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; execution evidence is recorded below.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.hex.BITCOUNT](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITCOUNT.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The [mapped native test](../../../../../../../../../test/org/minima/tests/kissvm/functions/maths/BITCOUNTTests.cpp) preserves the thirteen source hex examples, four constructor checks, exact ExecutionException rejections, and 256 independently computed table comparisons. This means 34 successful JUnit assertion calls plus 256 conditional predicates per runtime.

The [Java wrapper](../../../../../../../../../support/BITCOUNTOriginalTests.java) calls the three unchanged source methods. A separate invocation changes table entries and compares the helper/function results and signed overflow with C++. See the [worked lesson](../../../../../../../../new-tests/bit-counting.md). The original/native test entries, supplemental table observations and KISS VM differential suite pass in normal and sanitizer builds.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `e3de43f9e52c5c475e478e3d328db01db8be1957bafb350529f743ea339f1319`.
