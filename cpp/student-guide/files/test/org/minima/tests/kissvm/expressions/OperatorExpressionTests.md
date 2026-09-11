# OperatorExpressionTests — Arithmetic and bitwise expression semantics

Java source: [test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Build operator nodes from constant values and evaluate combinations of operands. The tests compare results and displayed expressions and exercise invalid operations. Do not replace exact decimal arithmetic with host floating point merely because small examples agree.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.java#L25) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 166 |
| [`testToString`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.java#L401) | Compare the displayed representation with explicit expected strings; display equality is distinct from execution correctness. | 12 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 72](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.java#L72):

```java
assertEquals(5, ((NumberValue) (new OperatorExpression(ce5, ce0, OperatorExpression.OPERATOR_ADD)).getValue(ctr)).getNumber().getAsLong());
```

[Source line 73](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.java#L73):

```java
assertEquals(5, ((NumberValue) (new OperatorExpression(ce0, ce5, OperatorExpression.OPERATOR_ADD)).getValue(ctr)).getNumber().getAsLong());
```

[Source line 102](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.java#L102):

```java
assertThrows(ExecutionException.class, () -> { (new OperatorExpression(ce1, ce0, OperatorExpression.OPERATOR_DIV)).getValue(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this checkpoint records successful execution of unchanged Java tests and native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.expressions.OperatorExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.objects.Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)

## C++ translation and validation

[Native counterpart](../../../../../../../../test/org/minima/tests/kissvm/expressions/OperatorExpressionTests.cpp) retains every active original method and 178
assertions per runtime. Unchanged Java and native tests pass in normal and
sanitizer builds. All numeric extrema, fractional remainders, hex operations, typed casts and distinct exception expectations remain active. MiniNumber limit constants and decimal constructor reuse existing decimal/arithmetic support.

[Expression/token lab](../../../../../../../new-tests/original-expression-token-tests.md)
explains types, evaluation, exact spelling and source coverage limits. Twelve
supplemental comparisons/twenty-four independent checks pass across the batch;
the expanded base API and full primitive regressions also pass in both builds.
[Evidence](../../../../../../../../validation/original-expression-token-tests-work-in-progress/progress.json).
Full production API, mutable-registry/alias behavior and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `175772e68640f95286105a45fb05e48a4c0a896657688ebdb9c0de822c8c06be`.
