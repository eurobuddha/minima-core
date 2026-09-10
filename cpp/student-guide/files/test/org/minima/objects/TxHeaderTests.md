# TxHeaderTests — Serializing a block and transaction header

Java source: [test/org/minima/objects/TxHeaderTests.java](../../../../../../../upstream-study/test/org/minima/objects/TxHeaderTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../source-map.json).

## Purpose and logic

Assign a body hash, difficulty, timestamp, block number, and nonce. Check that the body hash is nonzero and that serialization can be read back. This is an encoding test, not a proof that the header satisfies consensus rules.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`TxHeaderTests`](../../../../../../../upstream-study/test/org/minima/objects/TxHeaderTests.java#L22) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 5 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 30](../../../../../../../upstream-study/test/org/minima/objects/TxHeaderTests.java#L30):

```java
assertTrue("tx header should have a body hash", mHeader.getBodyHash() != null);
```

[Source line 31](../../../../../../../upstream-study/test/org/minima/objects/TxHeaderTests.java#L31):

```java
assertTrue("body hash should not be empty", mHeader.getBodyHash().isMore(new MiniData("0x0")));
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.base.MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)
- [org.minima.utils.Crypto](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java)
- [org.minima.utils.json.JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java)

## C++ translation and validation

The mapped native test is [test/org/minima/objects/TxHeaderTests.cpp](../../../../../../test/org/minima/objects/TxHeaderTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The C++ test strengthens the Java smoke check by comparing the decoded serialization and JSON with the original, as well as checking the consumed byte count.

[CMake registration](../../../../../../CMakeLists.txt) registers `TxHeaderTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^TxHeaderTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `aa31ac0521dbbfb245e503fb6c35a84fdf1e84ad846e0f7a025077a9965678dc`.
