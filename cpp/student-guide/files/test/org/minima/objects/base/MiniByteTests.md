# MiniByteTests — A byte wrapper with boolean and stream behavior

Java source: [test/org/minima/objects/base/MiniByteTests.java](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniByteTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../source-map.json).

## Purpose and logic

Construct zero, one, true, and false values and compare their boolean meaning. Exercise writing and reading a single byte. A byte has 256 bit patterns even when a host language presents signed numeric values.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testValueBytes`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniByteTests.java#L18) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 7 |
| [`testReadFromByte`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniByteTests.java#L48) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |
| [`testWriteDataStreamByte`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniByteTests.java#L69) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 31](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniByteTests.java#L31):

```java
assertNotNull("should not be null", i);
```

[Source line 33](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniByteTests.java#L33):

```java
assertNotNull("should not be null", k.getByteValue());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

## C++ translation and validation

The mapped native test is [test/org/minima/objects/base/MiniByteTests.cpp](../../../../../../../test/org/minima/objects/base/MiniByteTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native test loops from −512 through 512 and checks low-eight-bit conversion, string formatting, boolean interpretation, round trips, and rejection when reading beyond the buffer. Java testWriteDataStreamByte has no `@Test` annotation.

[CMake registration](../../../../../../../CMakeLists.txt) registers `MiniByteTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^MiniByteTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `d38efb1fd1b5b81804b69a749b14483d7b785d3c0a2302ce7798fbc77fc067b5`.
