# QuickInetLists — Factories for address lists and UID-to-address maps

Java source: [test/org/minima/system/network/p2p/testingutils/QuickInetLists.java](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickInetLists.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Generate 1-based address suffixes from a prefix and attach the requested port. A second helper composes a hostname/prefix string. Map creation assigns a random port in 7000..9999 when the supplied port is nonpositive. These details determine the input data other tests receive.

This is a test fixture helper rather than a standalone test suite.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`generateInetSockAddrList`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickInetLists.java#L22) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`generateInetSockAddrWithHostnameList`](../../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickInetLists.java#L40) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

There are no active assertions in this helper. Its outputs become the inputs to other tests; correctness must be assessed through callers or a dedicated helper test.

## C++ translation and validation

Native counterpart: [QuickInetLists.cpp](../../../../../../../../../test/org/minima/system/network/p2p/testingutils/QuickInetLists.cpp).

All three generators now call native collection and address types. The list loop appends one-based suffixes. A nonpositive count returns an empty list without dereferencing its prefix. The hostname overload concatenates before entering that loop, so a null hostname or prefix still fails with zero elements. The map captures `port <= 0` once, then draws a new port for every entry; changing the port inside the loop must not turn later entries into fixed-port entries.

The native factory reuses `InetSocketAddress::resolve` once and throws UnknownHostException before checking the port. Thus `bad/1` with port 65536 fails as an unknown host, while `192.168.0.1` with that port fails as an illegal argument. The differential corpus covers these priorities, IPv4/IPv6, nullable prefixes, empty results, real generated addresses and map keys. Random ports are checked against 7000–9999 before their values are normalized for comparison. This does not compare unseeded random sequences.

Both normal and sanitizer builds pass the original test suite (13 active methods, 15 assertions per runtime) and the helper corpus (1,302 comparisons, 16,100 independent checks per build). These are batch totals, not assertions attributed individually to this file. The [P2P lab](../../../../../../../../new-tests/original-p2p.md) records inputs, results and limits. Full production parity remains not_verified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `c7b4fefb82e91186450959552ce7cebe773f9530ba97e691f3dfd0f2bfed5701`.
