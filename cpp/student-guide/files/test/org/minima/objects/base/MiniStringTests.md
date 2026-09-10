# MiniStringTests — Text construction and stream transport

Java source: [test/org/minima/objects/base/MiniStringTests.java](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniStringTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../source-map.json).

## Purpose and logic

This file is a small smoke test for constructing and transporting text. Two methods have `@Test`; JUnit would discover those methods. The third method, `testWriteDataStreamString`, lacks that annotation. Our comparison runner invokes the two unchanged annotated methods after checking their annotation membership, then runs the third separately as a supplemental helper. It does not claim to use JUnitCore discovery.

There are **five successful-path assertions** in the annotated methods: four in construction and one after reading. The supplemental helper adds one. The exception-handler assertions are alternative branches, not additional assertions executed on the passing fixtures.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testValueString`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniStringTests.java#L18) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 4 |
| [`testReadFromStream`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniStringTests.java#L34) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |
| [`testWriteDataStreamString`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniStringTests.java#L55) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 23](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniStringTests.java#L23):

```java
assertNotNull("should not be null", i);
```

[Source line 25](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniStringTests.java#L25):

```java
assertNotNull("should not be null", j);
```

`testValueString` constructs `i` from `"MinimaTest"`, constructs `j` from `i`, and checks that both objects, the returned string and the returned byte array are non-null. It never asserts that their contents match `"MinimaTest"`.

`testReadFromStream` serializes `"test"`, calls the static reader through the variable `i`, **discards the returned object**, and checks that the original `i` is still non-null. Calling a static method through an instance does not turn it into an in-place read. A decoder returning the wrong text could pass this source test.

The unannotated helper serializes `"t"` and checks that its original object is non-null. Both stream methods accept an I/O exception only when its message contains `"input too large"`. Those branches do not execute for these tiny valid fixtures.

## C++ translation and validation

The [mapped C++ test](../../../../../../../test/org/minima/objects/base/MiniStringTests.cpp) now retains all three method bodies and separates the helper using `--unannotated-helper`. It reuses the existing production MiniString encoder/decoder unchanged. Native string and vector return values cannot be null; their source non-null checks are represented by materialized owned results. They deliberately remain smoke checks, rather than being relabeled as content-equality assertions.

The [Java runner](../../../../../../../support/MiniStringOriginalTests.java) calls unchanged pinned methods using the project's existing direct-call convention. It checks the two annotation memberships and total before running. The [shared original-test driver](../../../../../../../tools/script_value_original_tests.py) compiles the actual source and records ordinary and supplemental runs separately.

Both builds and all four selected checks pass: MiniStringTests, its original-Java comparison, the shared runner’s FLOOR branch, and the primitive differential suite. Normal CTest took **8.63 seconds**, sanitizer CTest **10.72 seconds**. Each runtime completes the five ordinary and one supplemental MiniString assertions, thirty FLOOR assertions, and **3,802 primitive comparisons** with zero differences. The final reports are `cpp/build/ministring-original-results.json` and `cpp/build-sanitized/ministring-original-results.json`.

```sh
ctest --test-dir cpp/build -R '^(MiniStringTests|ministring_original_tests)$' --output-on-failure
ctest --test-dir cpp/build-sanitized -R '^(MiniStringTests|ministring_original_tests)$' --output-on-failure
```

Existing [primitive differential tests](../../../../../../../tools/differential.py) provide stronger content observations, including malformed UTF-8 bytes. They are separate evidence and must not be counted as assertions in this source file. The [coin and proof reader lab](../../../../../../new-tests/coin-streams.md) illustrates why distinguishing static decoding from in-place mutation matters.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `5cfd9495661c5009af0364d0d68f121c4ad9f8c18c3d8364b4b460d9a8c0e3f2`.
