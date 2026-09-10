# FunctionExpressionTests — Calling built-in functions through expression nodes

Java source: [test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Build CONCAT, LEN, ABS, and INC with constant parameters. Wrap each function in FunctionExpression and compare its evaluated result. Check function/parameter display separately from the value it returns.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.java#L26) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testToString`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.java#L63) | Compare the displayed representation with explicit expected strings; display equality is distinct from execution correctness. | 4 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 55](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.java#L55):

```java
assertEquals("should be equal ", "0x0123456789", fe1.getValue(ctr).toString());
```

[Source line 56](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.java#L56):

```java
assertEquals("should be equal ", "HELLO WORLD", Value.getValue("[HELLO WORLD]").toString());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.expressions.FunctionExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/FunctionExpression.java)
- [org.minima.kissvm.functions.hex.CONCAT](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/CONCAT.java)
- [org.minima.kissvm.functions.hex.LEN](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/LEN.java)
- [org.minima.kissvm.functions.number.ABS](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/ABS.java)
- [org.minima.kissvm.functions.number.INC](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/INC.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The mapped native test is [test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.cpp](../../../../../../../../test/org/minima/tests/kissvm/expressions/FunctionExpressionTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native cases retain hex concatenation, byte length, absolute value, and increment examples. It also checks stack depth returns to zero. Its display coverage is narrower than the Java file's four displayed functions.

[CMake registration](../../../../../../../../CMakeLists.txt) registers `FunctionExpressionTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^FunctionExpressionTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `9f8927809ac6beda8659f4ce302dfbed961ae6b02a8588aa7b4ba0a0fd47fbd4`.
