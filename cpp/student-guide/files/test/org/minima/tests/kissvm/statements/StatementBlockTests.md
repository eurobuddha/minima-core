# StatementBlockTests — Running a sequence of script statements

Java source: [test/org/minima/tests/kissvm/statements/StatementBlockTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementBlockTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Construct empty and nonempty statement lists. Run an empty list, RETURN TRUE, RETURN FALSE, and LET followed by a numeric RETURN. A result value and a result-set flag differ: an empty program has not made a decision.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementBlockTests.java#L27) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 3 |
| [`testExecution`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementBlockTests.java#L69) | Execute prepared objects and inspect outcomes, state changes, instruction counts, or expected exceptions present in this body. | 18 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 80](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementBlockTests.java#L80):

```java
assertEquals(false, ctr.isSuccessSet());
```

[Source line 81](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementBlockTests.java#L81):

```java
assertEquals(false, ctr.isSuccess());
```

[Source line 130](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementBlockTests.java#L130):

```java
assertThrows(ExecutionException.class, () -> { sb.run(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.expressions.OperatorExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java)
- [org.minima.kissvm.expressions.VariableExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/VariableExpression.java)
- [org.minima.kissvm.statements.Statement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java)
- [org.minima.kissvm.statements.StatementBlock](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java)
- [org.minima.kissvm.statements.commands.LETstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java)
- [org.minima.kissvm.statements.commands.RETURNstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/RETURNstatement.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.objects.Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The mapped native test is [test/org/minima/tests/kissvm/statements/StatementBlockTests.cpp](../../../../../../../../test/org/minima/tests/kissvm/statements/StatementBlockTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native cases check empty and boolean-return blocks and reject numeric RETURN values while preserving unset/false success flags. They adapt selected Java cases rather than copying every repeated block.

[CMake registration](../../../../../../../../CMakeLists.txt) registers `StatementBlockTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^StatementBlockTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `a9f30506a8192bc9f293ad468bb3b0a3c65914efc4a0b2914fbe13395f5430fc`.
