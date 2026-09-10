# REVTests — Reversing byte order

Java source: [test/org/minima/tests/kissvm/functions/base/REVTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/REVTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Reverse 0x01234567 and require 0x67452301. This reverses bytes, not individual hexadecimal digits. The invalid cases vary argument count and operand types. The case labelled invalid domain supplies two empty values, so it proves wrong-arity rejection, not rejection of one empty value.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/REVTests.java#L28) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/REVTests.java#L45) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 3 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/REVTests.java#L64) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 6 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 56](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/REVTests.java#L56):

```java
assertEquals("0x67452301", ((HexValue) res).toString());
```

[Source line 72](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/REVTests.java#L72):

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
- [org.minima.kissvm.functions.hex.REV](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/REV.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The [mapped native test](../../../../../../../../../test/org/minima/tests/kissvm/functions/base/REVTests.cpp)
now reuses the BITGET assertion/factory adapter and the actual REV function.
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

Pinned Java SHA-256: `35385d7788cf61d0396d0d014c3ac2dfd2e76539cc91bcdd00209eb236e859d0`.
