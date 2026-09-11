# GETINIDTests — Reading a transaction input's coin ID

Java source: [test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../../source-map.json).

## Purpose and logic

Construct a transaction with distinguishable inputs. Query the coin ID at each selected index and compare with the corresponding coin field. Reject negative/out-of-range indices, inappropriate value types, and wrong argument counts.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java#L36) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`newSimpleAddress`](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java#L52) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`testValidParams`](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java#L66) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 7 |
| [`testInvalidParams`](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java#L128) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 8 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 109](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java#L109):

```java
assertEquals(in1.getCoinID(), ((HexValue) res).getMiniData());
```

[Source line 120](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java#L120):

```java
assertEquals(in2.getCoinID(), ((HexValue) res).getMiniData());
```

[Source line 164](../../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.java#L164):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; the checkpoint below records successful execution of unchanged Java tests and their native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.txn.input.GETINID](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/input/GETINID.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Address](../../../../../../../../../../../upstream-study/src/org/minima/objects/Address.java)
- [org.minima.objects.Coin](../../../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java)
- [org.minima.objects.CoinProof](../../../../../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java)
- [org.minima.objects.ScriptProof](../../../../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java)
- [org.minima.objects.Token](../../../../../../../../../../../upstream-study/src/org/minima/objects/Token.java)
- [org.minima.objects.Transaction](../../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniData](../../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)
- [org.minima.objects.mmr.MMRProof](../../../../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java)

## C++ translation and validation

[Mapped native counterpart](../../../../../../../../../../test/org/minima/tests/kissvm/functions/txn/input/GETINIDTests.cpp) runs all three annotated methods and
15 original assertions per runtime, and preserves newSimpleAddress.
Unchanged Java tests and their C++ counterparts pass in normal and sanitizer builds.
Both valid witness coin proofs are retained. Supplemental distinct coin IDs distinguish the witness lookup from transaction input IDs.

[Transaction getter lab](../../../../../../../../../new-tests/original-getter-tests.md) explains
coin selection, token units, witness IDs, shared-handle mutations and source limitations.
Nineteen supplemental comparisons and 38 independent checks use fixed rich KISS
fixtures, including distinct witness IDs and a fractional scaled amount.
[Checkpoint evidence](../../../../../../../../../../validation/original-getter-tests-work-in-progress/progress.json).
Full getter API, arbitrary mutation/concurrency and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `08e19891e2f996cb1d8aacf425f7decbd61116de7f84f242a71c543147c855f6`.
