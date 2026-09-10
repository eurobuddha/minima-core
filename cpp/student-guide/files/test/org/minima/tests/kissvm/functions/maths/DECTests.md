# DECTests — Subtracting one from an exact number

Java source: [test/org/minima/tests/kissvm/functions/maths/DECTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/DECTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Check decrement near zero and with long fractional tails, including −0.99999999999999999. Exact expected strings expose precision loss that binary floating point could introduce.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/DECTests.java#L28) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/DECTests.java#L45) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 33 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/DECTests.java#L177) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 4 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 57](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/DECTests.java#L57):

```java
assertEquals("-2", ((NumberValue) res).toString());
```

[Source line 68](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/DECTests.java#L68):

```java
assertEquals("-1.99999999999999999", ((NumberValue) res).toString());
```

[Source line 185](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/DECTests.java#L185):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations. The execution evidence for the mapped test is recorded in the lab linked below.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.number.DEC](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/DEC.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The [mapped native test](../../../../../../../../../test/org/minima/tests/kissvm/functions/maths/DECTests.cpp) reuses the ABSTests assertion pattern and actual native function objects. Its value table preserves all eleven active source examples; its four rejection checks require ExecutionException. Source comments and the empty domain block add no tests.

The [increment/decrement lab](../../../../../../../../../student-guide/new-tests/increment-decrement.md) records the build and execution results. The [existing comparison runner](../../../../../../../../../tools/script_value_original_tests.py) executes the unchanged pinned Java methods and this native counterpart. The successful path has 30 assertions per runtime: 4 factory/registry, 22 value/type, and 4 rejection assertions. The source-navigation counts above also include fail() branches, so they are not successful execution totals.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `6381369ec81fea0a27484412c3e89ba03713abfa1b73ceac4e3c992f969d49cd`.
