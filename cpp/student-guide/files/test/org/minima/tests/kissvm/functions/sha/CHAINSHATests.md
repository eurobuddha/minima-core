# CHAINSHATests — Checking proof-function argument handling

Java source: [test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

The file named CHAINSHATests imports PROOF and resolves the name PROOF. Its old successful chain-hash examples are commented out. Active constructor and invalid-parameter checks must be distinguished from those inactive examples.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.java#L28) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.java#L45) | Empty active body: the method establishes no behavior, even if comments describe a scenario. | 0 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.java#L198) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 20 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 206](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.java#L206):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; the checkpoint below records successful execution of the unchanged Java tests and their native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.sha.PROOF](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/PROOF.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

[Mapped C++ counterpart](../../../../../../../../../test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.cpp)
now executes all three original methods and 24 assertions per runtime.
Normal and sanitizer builds pass alongside unchanged pinned Java tests.
The class tests PROOF. The valid body is commented out; all 20 invalid rows reject on arity. Four supplemental proof fixtures exercise actual five-argument evaluations.

The shared fixture reuses native Contract, fresh functions and ConstantExpression;
only ExecutionException satisfies an expected rejection. See the
[original SHA lab](../../../../../../../../new-tests/original-sha-tests.md) for method counts,
logic, source limitations and 46 supplemental comparisons/92 independent checks.
[Saved checkpoint](../../../../../../../../../validation/original-sha-tests-work-in-progress/progress.json).
Full production and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `c4885e24cf36c7a00996d1dbc22f0dd07e45983e09ba843ee733fdae9a8276e3`.
