# AddressTests — Representing script addresses in binary and human-readable form

Java source: [test/org/minima/objects/AddressTests.java](../../../../../../../upstream-study/test/org/minima/objects/AddressTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../source-map.json).

## Purpose and logic

Construct addresses, compare equal and unequal values, and write/read the binary representation. The address conversion cases check conversion to and from the Minima address format. Equality and encoding are different properties: a readable string must still identify the same underlying bytes.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testAddress`](../../../../../../../upstream-study/test/org/minima/objects/AddressTests.java#L22) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 3 |
| [`testWriteAndReadDataStream`](../../../../../../../upstream-study/test/org/minima/objects/AddressTests.java#L48) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 3 |
| [`testMakeMinimaAddress`](../../../../../../../upstream-study/test/org/minima/objects/AddressTests.java#L73) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 8 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 29](../../../../../../../upstream-study/test/org/minima/objects/AddressTests.java#L29):

```java
assertNotNull("should not be null", a);
```

[Source line 42](../../../../../../../upstream-study/test/org/minima/objects/AddressTests.java#L42):

```java
assertFalse("should not be equal ", a.isEqual(adrThree));
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.base.MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)

## C++ translation and validation

Native counterpart: [AddressTests.cpp](../../../../../../test/org/minima/objects/AddressTests.cpp).

The native file now preserves all three annotated methods and ten successful-path assertions. Construction checks non-nullness and equality of script-derived addresses; the stream test reads back into the same object and checks its hex string; five original address samples round-trip through the Mx encoder and decoder. Odd-length hex inputs are retained exactly as written in the source.

The supplementary observation serializes the source stream sample `0xFFF0F0` and compares exact bytes plus decoded JSON with Java. This is separate from the original assertions. It catches a wire or content mismatch that a non-null check would miss. No change to Address production logic was needed.

Both normal and sanitizer builds pass the batch: eight original methods, twenty source assertions per runtime and six supplementary wire/decoded-content comparisons per build. The [object test lab](../../../../../new-tests/original-object-tests.md) explains the direct Java runner, retained source assertions and validation limits. Full parity remains not_verified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `f2bcf1900952c91d076e44235e0bc35a189ef444215f303f0febe7bf94fbfa58`.
