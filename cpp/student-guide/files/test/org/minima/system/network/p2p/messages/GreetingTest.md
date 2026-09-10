# GreetingTest — The exact JSON representation of a P2P greeting

Java source: [test/org/minima/system/network/p2p/messages/GreetingTest.java](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/messages/GreetingTest.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Build greetings for both a peer accepting inbound connections and one not accepting them. Compare their JSON strings with fixed examples, then decode and compare the resulting objects. Optional fields are part of the protocol shape.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`setUp`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/messages/GreetingTest.java#L18) | Create fresh fixture objects used by later tests; setup is preparation, not an assertion. | 0 |
| [`testFromJSON`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/messages/GreetingTest.java#L35) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 2 |
| [`testToJson`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/messages/GreetingTest.java#L44) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 2 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 38](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/messages/GreetingTest.java#L38):

```java
assertEquals(newGreetingNotAcceptingCon, greetingNotAcceptingCon);
```

[Source line 41](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/messages/GreetingTest.java#L41):

```java
assertEquals(newGreetingAcceptingCon, greetingAcceptingCon);
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.utils.json.JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java)

## C++ translation and validation

The mapped native test is [test/org/minima/system/network/p2p/messages/GreetingTest.cpp](../../../../../../../../../test/org/minima/system/network/p2p/messages/GreetingTest.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native test reuses the two literal expected JSON strings and checks object equality after decoding each greeting.

[CMake registration](../../../../../../../../../CMakeLists.txt) registers `GreetingTest` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^GreetingTest$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `5a9215a1abd80f25cec1f9405b59195b6acaed83f695f2f70ae4ece9b18cd2cf`.
