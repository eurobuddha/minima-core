# WHILEstatementTests — Repeated execution under a loop condition

Java source: [test/org/minima/tests/kissvm/statements/commands/WHILEstatementTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/WHILEstatementTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Build a condition over variable a and a statement body that changes execution state. The source includes display and execution examples. Read the actual assertions carefully: executing a loop without asserting its final state is weaker than checking its result.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/WHILEstatementTests.java#L28) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 1 |
| [`testExecution`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/WHILEstatementTests.java#L53) | Execute prepared objects and inspect outcomes, state changes, instruction counts, or expected exceptions present in this body. | 2 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 48](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/WHILEstatementTests.java#L48):

```java
assertEquals("WHILE ( variable:a LT 5 )", ws.toString());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this checkpoint records successful execution of unchanged Java tests and their native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.BooleanExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.expressions.OperatorExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java)
- [org.minima.kissvm.expressions.VariableExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/VariableExpression.java)
- [org.minima.kissvm.statements.Statement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java)
- [org.minima.kissvm.statements.StatementBlock](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java)
- [org.minima.kissvm.statements.commands.LETstatement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java)
- [org.minima.kissvm.statements.commands.RETURNstatement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/RETURNstatement.java)
- [org.minima.kissvm.statements.commands.WHILEstatement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/WHILEstatement.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

[Native counterpart](../../../../../../../../../test/org/minima/tests/kissvm/statements/commands/WHILEstatementTests.cpp) retains both annotated methods and 1
original assertions. Unchanged Java tests and native counterparts pass in normal
and sanitizer builds. The WHILE source has only one constructor assertion; both execution fixtures are preserved, and supplemental tests check final loop state and early returns.

[Statement command lab](../../../../../../../../new-tests/original-statement-commands.md)
explains reference translation, direct execution, control flow, instruction counts
and source coverage limits. Eight supplemental full-record Java/C++ comparisons
and sixteen independent expected-outcome checks pass across the batch.
[Evidence](../../../../../../../../../validation/original-statement-commands-work-in-progress/progress.json).
No production correction was needed. Full command API and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `7d187df43a056501d61b5276056e4d5292165663366916fc0ef9eb9bd78ef33e`.
