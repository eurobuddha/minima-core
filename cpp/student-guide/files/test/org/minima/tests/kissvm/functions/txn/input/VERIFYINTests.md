# VERIFYINTests — Comparing a transaction input with requested coin details

Java source: [test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../../source-map.json).

## Purpose and logic

Construct input coins and ask whether an indexed coin matches address, amount, and token ID. Change one requested field at a time to produce FALSE. Include scaled-token and invalid-index cases. A mismatch is a boolean outcome; malformed arguments can raise an exception. 

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java#L34) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`newSimpleAddress`](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java#L50) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`testValidParams`](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java#L64) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 26 |
| [`testInvalidParams`](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java#L306) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 8 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 124](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java#L124):

```java
assertEquals(true, ((BooleanValue) res).isTrue());
```

[Source line 168](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java#L168):

```java
assertEquals(true, ((BooleanValue) res).isFalse());
```

[Source line 299](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.java#L299):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.txn.input.VERIFYIN](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/input/VERIFYIN.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Address](../../../../../../../../../../../upstream-study/src/org/minima/objects/Address.java)
- [org.minima.objects.Coin](../../../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java)
- [org.minima.objects.ScriptProof](../../../../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java)
- [org.minima.objects.Token](../../../../../../../../../../../upstream-study/src/org/minima/objects/Token.java)
- [org.minima.objects.Transaction](../../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniData](../../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)
- [org.minima.objects.base.MiniString](../../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/tests/kissvm/functions/txn/input/VERIFYINTests.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `8a1a3804008539d76628add80077a016cbe398d1a33f87ce9c36dac4eb545e67`.
