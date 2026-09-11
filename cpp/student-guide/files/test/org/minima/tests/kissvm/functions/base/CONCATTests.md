# CONCATTests — Joining byte arrays

Java source: [test/org/minima/tests/kissvm/functions/base/CONCATTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/CONCATTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Concatenate two or more hexadecimal values in order. The source checks the exact output bytes and rejects inappropriate parameter types. Its empty-hex case has a special asserted representation; follow the source rather than assuming every empty value is invalid.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/CONCATTests.java#L29) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/CONCATTests.java#L46) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 6 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/CONCATTests.java#L81) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 14 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 58](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/CONCATTests.java#L58):

```java
assertEquals("0x0123456789ABCDEF", ((HexValue) res).toString());
```

[Source line 73](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/CONCATTests.java#L73):

```java
assertEquals("0x0123456789ABCDEF0123456789ABCDEF01234567", ((HexValue) res).toString());
```

[Source line 89](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/CONCATTests.java#L89):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; current execution results are recorded below.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.hex.CONCAT](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/CONCAT.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The [mapped native test](../../../../../../../../../test/org/minima/tests/kissvm/functions/base/CONCATTests.cpp)
reuses the existing native CONCAT, factory registry, ConstantExpression and Contract.
Its fixture and exception adapters follow LENTests. Each source argument and result
is retained, including the successful empty-hex case and eight type rejections.
The Java wrapper calls all three unchanged source methods directly.

There are 21 executed assertions on a passing run: four metadata checks, six
result type/value checks, three arity rejections and eight type rejections. The
navigation table counts source call sites including fail() handlers; those
handlers are not reached on a passing run. The commented-out empty-input rejection
remains inactive. Expected ExecutionException catches reject unrelated exceptions.

Both complete builds and both registered tests pass in each build, with 21
assertions per runtime and empty native stderr. See the
[concatenation lab](../../../../../../../../../student-guide/new-tests/concatenation-original-tests.md)
for the plan, completed results and limits. The source cases do not test maximum
size or expression-evaluation side effects and cannot establish full API parity.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `72e7c053ab5da3db91169dbce8211741c5702c5249a70cae46a72c97c8c93366`.
