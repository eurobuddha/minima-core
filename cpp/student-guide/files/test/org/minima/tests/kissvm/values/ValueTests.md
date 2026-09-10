# ValueTests — Parsing values and enforcing VM type boundaries

Java source: [test/org/minima/tests/kissvm/values/ValueTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ValueTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Parse TRUE/FALSE, hex, a number, and bracketed script text. Reject malformed literals, inspect type names, and compare same-type versus cross-type checks. Type checking is explicit because VM values are not interchangeable host-language values.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testGettersAndSetters`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ValueTests.java#L17) | Read/set fixture state and compare values or enforce the source type checks. | 23 |
| [`testTypeCheckers`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ValueTests.java#L62) | Read/set fixture state and compare values or enforce the source type checks. | 28 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 18](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ValueTests.java#L18):

```java
assertEquals("should be equal ", new StringValue(" RETURN TRUE ").toString(), Value.getValue("[ RETURN TRUE ]").toString());
```

[Source line 19](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ValueTests.java#L19):

```java
assertEquals("should be equal ", new HexValue("0xFFFF").toString(), Value.getValue("0xFFFF").toString());
```

[Source line 23](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ValueTests.java#L23):

```java
assertThrows(IllegalArgumentException.class, () -> { Value.getValue("!]"); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)

## C++ translation and validation

The mapped native test is [test/org/minima/tests/kissvm/values/ValueTests.cpp](../../../../../../../../test/org/minima/tests/kissvm/values/ValueTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native adaptation uses a four-type matrix for verifyType/checkSameType, checks boolean singleton identity, and retains the unknown-type-name case. Its malformed-literal list is selected rather than every spelling in Java.

[CMake registration](../../../../../../../../CMakeLists.txt) registers `ValueTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^ValueTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `c6572ae27e3eb513c7294f823d1c263b6d01d01a1264612aa448ea5c48a5eeae`.
