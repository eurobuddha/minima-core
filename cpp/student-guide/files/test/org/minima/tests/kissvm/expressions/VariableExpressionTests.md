# VariableExpressionTests — Looking up locally assigned script variables

Java source: [test/org/minima/tests/kissvm/expressions/VariableExpressionTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/VariableExpressionTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Read four missing names, require execution errors, assign values of four types, and read those same names again. Local variables belong to the contract execution state; they are separate from environment globals.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/VariableExpressionTests.java#L24) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 8 |
| [`testToString`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/VariableExpressionTests.java#L62) | Compare the displayed representation with explicit expected strings; display equality is distinct from execution correctness. | 4 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 55](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/VariableExpressionTests.java#L55):

```java
assertEquals("should be equal ", bv, ve1.getValue(ctr));
```

[Source line 56](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/VariableExpressionTests.java#L56):

```java
assertEquals("should be equal ", hv, ve2.getValue(ctr));
```

[Source line 32](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/VariableExpressionTests.java#L32):

```java
assertThrows(ExecutionException.class, () -> { ve1.getValue(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.VariableExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/VariableExpression.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.objects.StateVariable](../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java)
- [org.minima.objects.Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)

## C++ translation and validation

The mapped native test is [test/org/minima/tests/kissvm/expressions/VariableExpressionTests.cpp](../../../../../../../../test/org/minima/tests/kissvm/expressions/VariableExpressionTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native adaptation uses a loop over four names/types, preserves missing-variable exceptions, and compares returned value identity and variable:name text.

[CMake registration](../../../../../../../../CMakeLists.txt) registers `VariableExpressionTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^VariableExpressionTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `7754f3c1aba50115c6d9d3fef2c11e639c1cdbd84c04c0763d82f964b098cec1`.
