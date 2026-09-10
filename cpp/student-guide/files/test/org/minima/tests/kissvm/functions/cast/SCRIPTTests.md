# SCRIPTTests — Converting values to string/script text

Java source: [test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

The historical filename tests the imported STRING function. Compare the displayed text of values of different VM types. Comments about old behavior are observations in the source, not evidence of a current passing test.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.java#L31) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.java#L48) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 27 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.java#L159) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 1 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 59](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.java#L59):

```java
assertEquals("TRUE", ((StringValue) res).toString());
```

[Source line 70](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.java#L70):

```java
assertEquals("FALSE", ((StringValue) res).toString());
```

[Source line 167](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.java#L167):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These source expectations were executed successfully in the comparison described below.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.cast.STRING](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/STRING.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The [mapped C++ test](../../../../../../../../../test/org/minima/tests/kissvm/functions/cast/SCRIPTTests.cpp) preserves all 23 active assertions across the three original methods. It reuses the LENTests contract/constant-expression harness and current cast implementations. Fresh functions prevent parameters leaking between cases.

The source filename is SCRIPTTests but its function and registry name are STRING. Nine valid inputs become displayed text. Hex stays 0x-prefixed text, not decoded characters. Uppercase and mixed-case text remain unchanged. The commented lowercase expectations stay inactive.

The unchanged pinned Java methods and native counterpart pass in both normal and ASAN/UBSAN builds. All eight selected casting checks pass (7.70s/7.68s), including four native executables and four original-JUnit comparisons. See [the lab written before these tests](../../../../../../../../new-tests/cast-original-tests.md). Assertion call-site counts above include fail() branches; the executed totals here count only assertions actually reached on these successful paths. This is finite test coverage, not full production parity.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `f265a9f312a052eaa3a288cd0253b6111b6b585ab101e2f0a3ad2f503970ba82`.
