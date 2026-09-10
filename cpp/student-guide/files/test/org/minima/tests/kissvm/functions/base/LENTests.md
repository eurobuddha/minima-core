# LENTests — Measuring value length

Java source: [test/org/minima/tests/kissvm/functions/base/LENTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/LENTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Compare the length of a four-byte hex value with the source text example. Length is measured in the unit defined by the function, so counting printed hex characters would double the byte result. The string example is ASCII, so these original cases alone do not distinguish UTF-8 byte length from Java UTF-16 length. Preserve type and parameter-count rejection cases.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/LENTests.java#L29) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/LENTests.java#L46) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 6 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/LENTests.java#L77) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 4 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 57](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/LENTests.java#L57):

```java
assertEquals(4, ((NumberValue) res).getNumber().getAsInt());
```

[Source line 69](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/LENTests.java#L69):

```java
assertEquals(5, ((NumberValue) res).getNumber().getAsInt());
```

[Source line 85](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/LENTests.java#L85):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. The original three methods now pass in both comparison builds.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.hex.LEN](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/LEN.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The [mapped native test](../../../../../../../../../test/org/minima/tests/kissvm/functions/base/LENTests.cpp)
now reuses the BITGET assertion/factory adapter and the actual LEN function.
The [original-test runner](../../../../../../../../../tools/script_value_original_tests.py)
compiles all pinned production sources plus the unchanged Java test and invokes
its three methods. Both runtimes execute twelve successful-path assertions.
The fail() calls in unexpected-exception handlers count as source sites but are
not executed by a passing run.

See the [reverse and length lab](../../../../../../../../new-tests/reverse-and-length.md)
for the input, expected observation and limits of each group. Both normal and ASAN/UBSAN builds pass; the production function is unchanged.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `3f04cfd2b0048aaa1f8b55234ddaf70b94712c24893aa2bf16abf749b758736e`.
