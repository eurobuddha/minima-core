# STATETests — Reading a current transaction state value

Java source: [test/org/minima/tests/kissvm/functions/state/STATETests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/state/STATETests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Populate transaction state ports with several value types, read them back, and compare type and text. State is indexed by explicit port numbers, which need not be the same as array offsets.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/state/STATETests.java#L32) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/state/STATETests.java#L49) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 9 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/state/STATETests.java#L98) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 7 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 75](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/state/STATETests.java#L75):

```java
assertEquals(Value.getValue(States.get(i).toString()).toString(), ((BooleanValue) res).toString());
```

[Source line 79](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/state/STATETests.java#L79):

```java
assertEquals(Value.getValue(States.get(i).toString()).toString(), ((HexValue) res).toString());
```

[Source line 106](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/state/STATETests.java#L106):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; the checkpoint below records successful execution of unchanged Java tests and their native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.state.STATE](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/STATE.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.StateVariable](../../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

[Mapped native counterpart](../../../../../../../../../test/org/minima/tests/kissvm/functions/state/STATETests.cpp)
executes all three active source methods and 139 assertions per runtime.
The unchanged Java tests and native counterparts pass in normal and sanitizer builds.
The original fixture inserts 64 state variables into a real Transaction. Both the runtime type and parsed text are asserted for every port; missing values and invalid arguments must throw ExecutionException.

The production function, StateVariable, Transaction and Contract are reused.
[State test lab](../../../../../../../../new-tests/original-state-tests.md) explains the
fixture logic, exact successful-path counts, shared runner and coverage limits.
[Checkpoint results](../../../../../../../../../validation/original-state-tests-work-in-progress/progress.json).
Full state API, aliasing and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `c1572031b3cc58bc6c4e66870119e7e504d24d44976bc47e43c7c960a8cbc04a`.
