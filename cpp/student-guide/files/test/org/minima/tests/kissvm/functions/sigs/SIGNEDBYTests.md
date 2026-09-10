# SIGNEDBYTests — Checking whether a key is among the contract signers

Java source: [test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Build a contract with an explicit signature/key list, then ask about a present and an absent key. This membership function is distinct from CHECKSIG, which verifies signature bytes over data.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.java#L29) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.java#L46) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 6 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.java#L87) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 5 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 67](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.java#L67):

```java
assertEquals("TRUE", ((BooleanValue) res).toString());
```

[Source line 79](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.java#L79):

```java
assertEquals("FALSE", ((BooleanValue) res).toString());
```

[Source line 95](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.java#L95):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.sigs.SIGNEDBY](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/SIGNEDBY.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/tests/kissvm/functions/sigs/SIGNEDBYTests.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `ce9ff756e2a32a51753db621adcbc2f50e2a136e7d87e76b7e1e9ebea7eb0e12`.
