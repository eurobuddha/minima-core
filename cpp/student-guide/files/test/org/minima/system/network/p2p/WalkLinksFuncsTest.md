# WalkLinksFuncsTest — Filtering peer walks and respecting link capacity

Java source: [test/org/minima/system/network/p2p/WalkLinksFuncsTest.java](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/WalkLinksFuncsTest.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Remove five known addresses from a ten-address map and check that five remain. Repeat with an empty source map. Then deliver a returned walk to a state whose outgoing slots are full and check that no connection messages are emitted.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`setUp`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/WalkLinksFuncsTest.java#L18) | Create fresh fixture objects used by later tests; setup is preparation, not an assertion. | 0 |
| [`tearDown`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/WalkLinksFuncsTest.java#L22) | Empty active body: the method establishes no behavior, even if comments describe a scenario. | 0 |
| [`testRemoveIPsInBFromA`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/WalkLinksFuncsTest.java#L26) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testRemoveIPsInBFromAEmptyA`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/WalkLinksFuncsTest.java#L33) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testOnReturnedWalkMsgOutLinksFull`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/WalkLinksFuncsTest.java#L40) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 30](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/WalkLinksFuncsTest.java#L30):

```java
assertEquals(5, ret.size());
```

[Source line 37](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/WalkLinksFuncsTest.java#L37):

```java
assertTrue(ret.isEmpty());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.system.network.p2p.messages.P2PWalkLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java)
- [org.minima.system.network.p2p.testingutils.QuickInetLists](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickInetLists.java)
- [org.minima.system.network.p2p.testingutils.QuickState](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java)
- [org.minima.utils.messages.Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java)

## C++ translation and validation

Native counterpart: [WalkLinksFuncsTest.cpp](../../../../../../../../test/org/minima/system/network/p2p/WalkLinksFuncsTest.cpp).

The mapped test executes all three source methods and three assertions. Removing a five-address list from ten peers leaves five results; removing from an empty map leaves an empty result; a returned walk cannot request more outgoing links when the state already has five and the target is five. It reuses the real map, QuickState, P2PWalkLinks and WalkLinksFuncs implementations.

The source checks sizes and emptiness. It does not assert the complete remaining address sequence or every state transition. Keep those distinctions when interpreting a passing run.

Both normal and sanitizer builds pass the original test suite (13 active methods, 15 assertions per runtime) and the helper corpus (1,302 comparisons, 16,100 independent checks per build). These are batch totals, not assertions attributed individually to this file. The [P2P lab](../../../../../../../new-tests/original-p2p.md) records inputs, results and limits. Full production parity remains not_verified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `788f90f47f7187ad7ba760f2d7d58e6d00d262a367daf3c3ad870af7dc5b43e4`.
