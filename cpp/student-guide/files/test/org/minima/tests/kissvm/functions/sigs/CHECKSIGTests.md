# CHECKSIGTests — Verifying a cryptographic signature

Java source: [test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Construct keys, signed data, and signature objects and compare acceptance for matching versus changed inputs. A cryptographic verification test must use the actual signature implementation, not a mock returning TRUE.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.java#L30) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.java#L47) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 24 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.java#L170) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 35 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 69](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.java#L69):

```java
assertEquals("TRUE", ((BooleanValue) res).toString());
```

[Source line 82](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.java#L82):

```java
assertEquals("TRUE", ((BooleanValue) res).toString());
```

[Source line 178](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.java#L178):

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
- [org.minima.kissvm.functions.sigs.CHECKSIG](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/CHECKSIG.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.objects.keys.TreeKey](../../../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java)

## C++ translation and validation

[Mapped native counterpart](../../../../../../../../../test/org/minima/tests/kissvm/functions/sigs/CHECKSIGTests.cpp)
executes all three active source methods and 55 assertions per runtime.
Unchanged Java tests and their C++ counterparts pass in normal and sanitizer builds.
Both real TreeKey(seed,16,2) objects, 32/64-byte seeds and messages, eight valid/mismatched combinations and all 35 rejection checks are retained.

[Signature test lab](../../../../../../../../new-tests/original-signature-tests.md) explains
membership versus cryptographic verification, fixture logic and source limitations.
Fourteen supplemental comparisons and 28 independent checks cover fixed-input
public keys, signature bytes and verification of both writers' signatures by both
runtimes. [Checkpoint evidence](../../../../../../../../../validation/original-signature-tests-work-in-progress/progress.json).
Full signature API, mutation/concurrency and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `81b8bd2a88eeb9126689dc022fe5e8c678202b266c0045ff3d825dc346c1fa96`.
