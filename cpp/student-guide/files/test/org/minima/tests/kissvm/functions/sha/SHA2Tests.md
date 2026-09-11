# SHA2Tests — Hashing byte input with SHA-2

Java source: [test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Compare results with values computed in the fixture and the fixed empty-input digest. The bytes passed to a hash matter: hashing a hex display string would be a different operation from hashing the represented bytes.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java#L31) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java#L48) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 9 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java#L100) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 4 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 63](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java#L63):

```java
assertEquals(Result.toString(), ((HexValue) res).toString());
```

[Source line 79](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java#L79):

```java
assertEquals(Result.toString(), ((HexValue) res).toString());
```

[Source line 108](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java#L108):

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
- [org.minima.kissvm.functions.sha.SHA2](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/SHA2.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.utils.Crypto](../../../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java)

## C++ translation and validation

[Mapped C++ counterpart](../../../../../../../../../test/org/minima/tests/kissvm/functions/sha/SHA2Tests.cpp)
now executes all three original methods and 212 assertions per runtime.
Normal and sanitizer builds pass alongside unchanged pinned Java tests.
All 100 random 64-byte trials are retained. Separate fixed-input hash comparisons provide independent expected digests.

The shared fixture reuses native Contract, fresh functions and ConstantExpression;
only ExecutionException satisfies an expected rejection. See the
[original SHA lab](../../../../../../../../new-tests/original-sha-tests.md) for method counts,
logic, source limitations and 46 supplemental comparisons/92 independent checks.
[Saved checkpoint](../../../../../../../../../validation/original-sha-tests-work-in-progress/progress.json).
Full production and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `453d5391eddd038cd08545943855de0f8bc95b9afc70e006ca66f20ee57ef458`.
