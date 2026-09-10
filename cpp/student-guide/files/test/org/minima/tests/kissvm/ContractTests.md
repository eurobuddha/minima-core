# ContractTests — The execution context of a KISS script

Java source: [test/org/minima/tests/kissvm/ContractTests.java](../../../../../../../../upstream-study/test/org/minima/tests/kissvm/ContractTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../source-map.json).

## Purpose and logic

Inspect a newly constructed contract, parse valid and invalid scripts, set variables/globals, and run a script that exceeds the instruction budget. Separate parse status, exception status, and a RETURN result: they answer different questions.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../upstream-study/test/org/minima/tests/kissvm/ContractTests.java#L31) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 24 |
| [`testGettersAndSetters`](../../../../../../../../upstream-study/test/org/minima/tests/kissvm/ContractTests.java#L166) | Read/set fixture state and compare values or enforce the source type checks. | 10 |
| [`testExecution`](../../../../../../../../upstream-study/test/org/minima/tests/kissvm/ContractTests.java#L220) | Execute prepared objects and inspect outcomes, state changes, instruction counts, or expected exceptions present in this body. | 12 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 34](../../../../../../../../upstream-study/test/org/minima/tests/kissvm/ContractTests.java#L34):

```java
assertEquals(0, ctr.getGlobalVariables().size());
```

[Source line 35](../../../../../../../../upstream-study/test/org/minima/tests/kissvm/ContractTests.java#L35):

```java
assertTrue(ctr.isParseOK());
```

[Source line 213](../../../../../../../../upstream-study/test/org/minima/tests/kissvm/ContractTests.java#L213):

```java
assertThrows(ExecutionException.class, () -> { ctr.getBoolParam(0, mf); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.txn.input.GETINADDR](../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/input/GETINADDR.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.StateVariable](../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java)
- [org.minima.objects.Transaction](../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.objects.base.MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.utils.MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java)

## C++ translation and validation

The mapped native test is [test/org/minima/tests/kissvm/ContractTests.cpp](../../../../../../../test/org/minima/tests/kissvm/ContractTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native test retains selected constructor/state/error cases, exact parse-error text, a shared global-variable map, and the 1025-instruction rejection. It does not reproduce every Java signature, previous-state, and trace fixture.

[CMake registration](../../../../../../../CMakeLists.txt) registers `ContractTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^ContractTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `bfca27a2a8a79d7e8be25d3de1f8dac0ced42e372f77da651c7b2c02600d66e7`.
