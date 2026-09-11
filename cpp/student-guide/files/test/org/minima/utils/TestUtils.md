# TestUtils — Constructing small synthetic TxPoW values for tests

Java source: [test/org/minima/utils/TestUtils.java](../../../../../../../upstream-study/test/org/minima/utils/TestUtils.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../source-map.json).

## Purpose and logic

The one-argument helper delegates to a three-argument helper using block 1 and weight 0. The three-argument helper constructs TxPoW directly from the requested ID, block, and weight. This is test data construction, not proof-of-work mining.

This is a test fixture helper rather than a standalone test suite.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`getTxPoW`](../../../../../../../upstream-study/test/org/minima/utils/TestUtils.java#L7) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`getTxPoW`](../../../../../../../upstream-study/test/org/minima/utils/TestUtils.java#L11) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

There are no active assertions in this helper. Its outputs become the inputs to other tests; correctness must be assessed through callers or a dedicated helper test.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.TxPoW](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java)

## C++ translation and validation

The [mapped counterpart](../../../../../../test/org/minima/utils/TestUtils.cpp) now implements the source helper's active
methods using the existing production constructors. The [helper lab](../../../../../new-tests/original-test-helpers.md)
explains the exact delegation, generated data, retained references, test inputs
and remaining limitations. Both builds pass 584 comparisons and 5,472 independent
checks for this two-helper batch. Full dependent API parity remains not_verified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `445aadcb501544e3dc16624f5ebe8d373e09370625f00c610b009c567b507245`.
