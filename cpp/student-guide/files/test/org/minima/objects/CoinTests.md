# CoinTests — Keeping the fields of an unspent coin together

Java source: [test/org/minima/objects/CoinTests.java](../../../../../../../upstream-study/test/org/minima/objects/CoinTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../source-map.json).

## Purpose and logic

Create a coin with an ID, address, amount, and token ID. The first method checks token ID, amount and address getters. The other two methods exercise stream calls, but only assert that the original coin remains non-null. They do not establish that every decoded field retains its meaning.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testCoin`](../../../../../../../upstream-study/test/org/minima/objects/CoinTests.java#L21) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 3 |
| [`testReadAndWriteDataStream`](../../../../../../../upstream-study/test/org/minima/objects/CoinTests.java#L50) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |
| [`testStaticReadAndWriteDataStream`](../../../../../../../upstream-study/test/org/minima/objects/CoinTests.java#L78) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |

There are **five assertions on the successful path**: three getter checks and one non-null check in each stream method. Each stream method also contains a failing assertion in its IOException catch, giving seven source assertion call sites. The disabled resetCoinID call and its commented assertion are not executed tests.

## Concrete test evidence

[Source line 39](../../../../../../../upstream-study/test/org/minima/objects/CoinTests.java#L39):

```java
assertTrue("Token id should equal", c.getTokenID().equals(tokenId));
```

[Source line 40](../../../../../../../upstream-study/test/org/minima/objects/CoinTests.java#L40):

```java
assertTrue("Amount should equal", c.getAmount().equals(two));
```

`testCoin` creates ID `0xfff`, address `0xffffffff`, amount 2 and token ID from `123`. The token-ID and address checks use MiniData’s overridden `equals`, which delegates to byte-content comparison. MiniNumber does not override `Object.equals`, so the amount check succeeds because Java returns the exact supplied `two` object. A native numeric-equality assertion alone would be weaker than that reference check; this is another reason to review ownership before calling the original test a faithful port.

The instance stream method writes a 12-unit coin and calls `readDataStream` on that same coin. Its single success assertion is only `assertNotNull(c)`. The static stream method calls `Coin.ReadFromStream` and **discards the returned coin**, then checks the original `c` for non-null. Both fail if an IOException reaches their catch. Consequently, successful execution would show that these calls completed, but the assertions would miss an incorrect decoded amount or ID. No new execution result is claimed here.

The [coin ownership lab](../../../../../new-tests/coin-ownership.md) is a separate, stronger experiment: it compares bytes/JSON after deliberate shared-object mutations and checks deep-copy isolation. It does not create this missing original-test counterpart.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.base.MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/objects/CoinTests.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `61eb820b54524dafe46bd73725019e7ee8e045bd50f7684731cdce0fbb57ebe6`.
