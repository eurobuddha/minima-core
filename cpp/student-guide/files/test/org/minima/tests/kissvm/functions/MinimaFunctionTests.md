# MinimaFunctionTests — Function lookup, fresh instances, and parameter lists

Java source: [test/org/minima/tests/kissvm/functions/MinimaFunctionTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/MinimaFunctionTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Resolve a known function and reject an unknown name. Create the dummy function, add expression parameters, retrieve them by index, and test an invalid index. Parameter metadata is checked without requiring a real transaction.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/MinimaFunctionTests.java#L18) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testGettersAndSetters`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/MinimaFunctionTests.java#L40) | Read/set fixture state and compare values or enforce the source type checks. | 9 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 51](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/MinimaFunctionTests.java#L51):

```java
assertEquals(ce1, mf.getParameter(0));
```

[Source line 59](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/MinimaFunctionTests.java#L59):

```java
assertEquals(ce2, mf.getParameter(1));
```

[Source line 28](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/MinimaFunctionTests.java#L28):

```java
assertThrows(MinimaParseException.class, () -> { MinimaFunction.getFunction("DUMMYFUNCTION"); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. The native counterpart preserves these source expectations; its typed exception checks do not accept an arbitrary failure.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)

## C++ translation and validation

The [mapped C++ test](../../../../../../../../test/org/minima/tests/kissvm/functions/MinimaFunctionTests.cpp) preserves **11 assertions executed on the success path**: four in the constructor method and seven in the getter/setter method. The navigation table also counts `fail()` calls inside catch blocks; those must not run in a passing test. Thus its 14 written assertion sites do not mean 14 successful assertions.

First, registry lookup finds CONCAT. Looking up DUMMYFUNCTION must throw MinimaParseException because the test helper is not registered. Constructing DUMMYFUNCTION directly and calling getNewFunction produces an instance with the expected name and no parameters. Registry membership and availability as a class are separate concepts.

Next, the test constructs two ConstantExpression objects, adds them, and checks parameter count and indexed retrieval. Java's assertEquals here ultimately uses object identity because the expression does not supply value-based equality. C++ compares the shared pointers to the exact objects inserted. Comparing only their displayed values would weaken this expectation. Reading index 2 after adding two parameters must throw ExecutionException, and the count remains 2 afterward.

The native test reuses production MinimaFunction, ConstantExpression and Value classes. The [original Java runner](../../../../../../../../support/MinimaFunctionOriginalTests.java) executes both unchanged JUnit methods with the original DUMMYFUNCTION source. The shared runner compiles that extra helper only for this test. Both full builds passed. Both unchanged Java methods and the 11 native assertions pass in normal and sanitizer builds. All nine selected suites pass in 10.55 seconds and 10.83 seconds respectively.

These tests do not cover negative indexes, null parameters, mutations through getAllParameters, locale-dependent names or every registry member. Nor do they execute the helper's constant-TRUE body. They establish the listed observations only.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `3d3a8e0b66e7de4c8fe4399f51e5a23064f715dc28216cc9a60944d1d9852ff5`.
