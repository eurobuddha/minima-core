# QuickState — Named starting states for P2P tests

Java source: [test/org/minima/system/network/p2p/testingutils/QuickState.java](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickState.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

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

Native counterpart: [QuickState.cpp](../../../../../../../../../test/org/minima/system/network/p2p/testingutils/QuickState.cpp).

All seven factories now create real native P2PState objects and populate their existing maps and sets through QuickInetLists. Incoming peers use 60.0.0.*, outgoing peers use 70.0.0.*, non-P2P peers use 10.0.1.*, and peers refusing connections use 50.0.1.*. A factory changes only its specified collections; the all-links map starts empty.

The known-peer-only factory inserts the same address object in the known set and incoming map. An equal copy would miss the source identity relationship. The probe checks that identity, then clears the set and checks that its copied list retains the address. It also changes global defaults before construction to check the local port, connection limits and loop delay. No-connections without an address keeps the wildcard at port 9001 and the host flag false. The new helper corpus executes each factory, positive/nonpositive counts and port failures.

Both normal and sanitizer builds pass the original test suite (13 active methods, 15 assertions per runtime) and the helper corpus (1,302 comparisons, 16,100 independent checks per build). These are batch totals, not assertions attributed individually to this file. The [P2P lab](../../../../../../../../new-tests/original-p2p.md) records inputs, results and limits. Full production parity remains not_verified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `15c13f8c44bb798e4ea88d3cf22b61e3e564f4f14221a5f49eba7e37116984ee`.
