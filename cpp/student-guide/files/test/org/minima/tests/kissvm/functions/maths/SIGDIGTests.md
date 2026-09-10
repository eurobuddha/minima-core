# SIGDIGTests — Rounding to a requested number of significant digits

Java source: [test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Compare the resulting BigDecimal numerically with the expected MiniNumber. Significant digits count meaningful digits across the whole number, not merely digits after the decimal point.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.java#L29) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.java#L46) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 3 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.java#L74) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 19 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 65](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.java#L65):

```java
assertEquals(0, mn1.getAsBigDecimal().compareTo(mn2.getAsBigDecimal()));
```

[Source line 82](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.java#L82):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations. Execution evidence for the mapped counterpart is recorded below.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.number.SIGDIG](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/SIGDIG.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)

## C++ translation and validation

The [mapped C++ test](../../../../../../../../../test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.cpp) preserves all 89 active assertions per runtime. It uses the existing native function, Contract and ConstantExpression classes. The [Java wrapper](../../../../../../../../../support/SIGDIGOriginalTests.java) calls all three unchanged pinned test methods through the [existing runner](../../../../../../../../../tools/script_value_original_tests.py). Both native and original-test CTest entries pass in normal and sanitizer builds.

Read the [worked lab](../../../../../../../../new-tests/power-significant-digits.md) for exact assertion accounting, parameter order and the limits of these examples.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `2205e1ed05ae266c2142b1d0fbe833a1286b53c7b9a79c52193f3fe77ab899e1`.
