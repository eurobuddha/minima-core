# TransactionTests — Building a transaction from inputs, outputs, and state

Java source: [test/org/minima/objects/TransactionTests.java](../../../../../../../upstream-study/test/org/minima/objects/TransactionTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../source-map.json).

## Purpose and logic

Start with an empty transaction. Add an input worth 5, then outputs worth 2 and 3. Check sums, validity, JSON field presence, and state port 1. Serialize the result and repeat the output and validity checks after decoding.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testBasicTransaction`](../../../../../../../upstream-study/test/org/minima/objects/TransactionTests.java#L23) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 9 |
| [`testMinimaTransaction`](../../../../../../../upstream-study/test/org/minima/objects/TransactionTests.java#L39) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 16 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 26](../../../../../../../upstream-study/test/org/minima/objects/TransactionTests.java#L26):

```java
assertTrue("A new transaction should be empty", t.isEmpty());
```

[Source line 27](../../../../../../../upstream-study/test/org/minima/objects/TransactionTests.java#L27):

```java
assertTrue("A new transaction should have no inputs", t.getAllInputs().size() == 0);
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.base.MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)
- [org.minima.utils.json.JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java)

## C++ translation and validation

The mapped native test is [test/org/minima/objects/TransactionTests.cpp](../../../../../../test/org/minima/objects/TransactionTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native adaptation preserves the 5→2+3 example, checks all 256 initially absent state ports, and adds an explicit assertion that deserialization consumes the entire byte buffer.

[CMake registration](../../../../../../CMakeLists.txt) registers `TransactionTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^TransactionTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `9a8695307c362e24e588006e1a0db4c634db7d23b39480a40b09b63c401bf744`.
