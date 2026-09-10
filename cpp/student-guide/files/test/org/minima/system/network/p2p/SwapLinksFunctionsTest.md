# SwapLinksFunctionsTest — Peer connection transitions and address-response secrets

Java source: [test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Prepare an incoming or outgoing connection message and a small P2P state. Disconnect it and inspect the appropriate link collection. Process greetings to move a connection out of the non-P2P list. Check a request secret is echoed and that an incorrect response secret leaves the local address unchanged.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`setUp`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L22) | Create fresh fixture objects used by later tests; setup is preparation, not an assertion. | 0 |
| [`testOnDisconnectedKnownPeer`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L28) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testOnDisconnectedInLink`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L36) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testOnDisconnectedOutLink`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L43) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testOnDisconnectedNoneP2PLink`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L51) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testProcessGreetingInLink`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L58) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 2 |
| [`testProcessGreetingOutLink`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L73) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 2 |
| [`testProcessRequestIPMsg`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L86) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testProcessResponseIPMsgIncorrectSecret`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L94) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 32](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L32):

```java
assertFalse(state.getKnownPeers().isEmpty());
```

[Source line 40](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/SwapLinksFunctionsTest.java#L40):

```java
assertTrue(state.getInLinks().isEmpty());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.base.MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.system.network.minima.NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java)
- [org.minima.system.network.p2p.messages.P2PGreeting](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java)
- [org.minima.system.network.p2p.testingutils.QuickState](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java)
- [org.minima.system.params.GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java)
- [org.minima.utils.json.JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java)
- [org.minima.utils.messages.Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/system/network/p2p/SwapLinksFunctionsTest.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `ff19ae52a123c97c0978a6a517a78e99a46f3f45c913bdd56e6721656f018247`.
