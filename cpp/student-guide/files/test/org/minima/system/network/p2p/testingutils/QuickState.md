# QuickState — Named starting states for P2P tests

Java source: [test/org/minima/system/network/p2p/testingutils/QuickState.java](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Each factory builds a fresh P2PState, sets the local address/host flag where applicable, and populates known peers and selected connection maps through QuickInetLists. Separate helpers represent no connections, incoming/outgoing links, non-P2P links, and full link collections.

This is a test fixture helper rather than a standalone test suite.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`stateNoConnections`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java#L10) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`stateOutLinksOnly`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java#L20) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`stateInLinksOnly`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java#L29) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`stateInLinkKnownPeerOnly`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java#L38) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`stateInAndOutLinks`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java#L47) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`stateNoneP2PLink`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java#L58) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`stateFullLinks`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java#L67) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

There are no active assertions in this helper. Its outputs become the inputs to other tests; correctness must be assessed through callers or a dedicated helper test.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.system.network.p2p.P2PState](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/system/network/p2p/testingutils/QuickState.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this helper, preserve its generated values, collection order, defaults, and error behavior. Reuse the corresponding native Minima types and test the helper through its consuming tests.

Compile the eventual helper with its consuming native tests and run those tests after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `15c13f8c44bb798e4ea88d3cf22b61e3e564f4f14221a5f49eba7e37116984ee`.
