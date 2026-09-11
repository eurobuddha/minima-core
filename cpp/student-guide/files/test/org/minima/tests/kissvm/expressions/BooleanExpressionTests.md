# BooleanExpressionTests — Boolean operators as expression-tree nodes

Java source: [test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Wrap values in constant expressions, combine them with boolean/comparison operators, and evaluate them in a contract context. Expected truth values check operator behavior; toString checks the displayed expression tree independently.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.java#L24) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 102 |
| [`testToString`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.java#L413) | Compare the displayed representation with explicit expected strings; display equality is distinct from execution correctness. | 14 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 35](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.java#L35):

```java
assertTrue(be.getBooleanValue(ctr).isTrue());
```

[Source line 37](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.java#L37):

```java
assertTrue(be.getBooleanValue(ctr).isFalse());
```

[Source line 103](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.java#L103):

```java
assertThrows(IllegalArgumentException.class, () -> { BooleanExpression be1 = new BooleanExpression(cet, cet, BooleanExpression.BOOLEAN_AND); be1.getValue(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this checkpoint records successful execution of unchanged Java tests and native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.BooleanExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.objects.Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

[Native counterpart](../../../../../../../../test/org/minima/tests/kissvm/expressions/BooleanExpressionTests.cpp) retains every active original method and 116
assertions per runtime. Unchanged Java and native tests pass in normal and
sanitizer builds. All truth-table/equality/order cases, shared Contract fixtures, unary defaults, distinct exception expectations and fourteen display checks remain active.

[Expression/token lab](../../../../../../../new-tests/original-expression-token-tests.md)
explains types, evaluation, exact spelling and source coverage limits. Twelve
supplemental comparisons/twenty-four independent checks pass across the batch;
the expanded base API and full primitive regressions also pass in both builds.
[Evidence](../../../../../../../../validation/original-expression-token-tests-work-in-progress/progress.json).
Full production API, mutable-registry/alias behavior and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `84c7f6fa8175299464b68e668b32bb7220f0fd2a23b028c461db0dad0b382fc8`.
