# P2PDBTest — Saving a peer list and protocol version

Java source: [test/org/minima/system/network/p2p/P2PDBTest.java](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/P2PDBTest.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Generate 1,000 peers from four private address ranges. Check a new database is empty, set the peer list, save/load it, and check its count. Separately round-trip the P2P version.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`before`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/P2PDBTest.java#L22) | Create fresh fixture objects used by later tests; setup is preparation, not an assertion. | 0 |
| [`testGetEmptyPeersList`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/P2PDBTest.java#L36) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testSetPeersList`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/P2PDBTest.java#L49) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testSavingAndLoadingPeersList`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/P2PDBTest.java#L59) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 2 |
| [`testSavingAndLoadingVersion`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/P2PDBTest.java#L82) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 43](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/P2PDBTest.java#L43):

```java
assertTrue(emptyPeers.isEmpty());
```

[Source line 54](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/P2PDBTest.java#L54):

```java
assertFalse(db.getPeersList().isEmpty());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.system.network.p2p.params.P2PParams](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java)

## C++ translation and validation

The mapped native test is [test/org/minima/system/network/p2p/P2PDBTest.cpp](../../../../../../../../test/org/minima/system/network/p2p/P2PDBTest.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native test keeps the original four cases but uses a unique temporary directory and explicit cleanup, avoiding the Java fixture's fixed tmp.db filename. It does not test native H2 or MVStore.

[CMake registration](../../../../../../../../CMakeLists.txt) registers `P2PDBTest` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^P2PDBTest$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `4f10bbccd70e23f78e96429ed2660c8d20cb5bdb5016fa34235708239657e1fb`.
