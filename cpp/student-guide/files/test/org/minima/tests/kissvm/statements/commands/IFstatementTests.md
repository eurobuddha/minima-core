# IFstatementTests — Conditional branches and their execution costs

Java source: [test/org/minima/tests/kissvm/statements/commands/IFstatementTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/IFstatementTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Construct lists of conditions and statement blocks. Run them with different truth values, checking the decision and instruction count. Only the selected branch should determine the result.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/IFstatementTests.java#L23) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 1 |
| [`testExecution`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/IFstatementTests.java#L48) | Execute prepared objects and inspect outcomes, state changes, instruction counts, or expected exceptions present in this body. | 28 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 43](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/IFstatementTests.java#L43):

```java
assertEquals("IF FALSE, ELSEIF FALSE, ELSEIF FALSE", ifs.toString());
```

[Source line 77](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/IFstatementTests.java#L77):

```java
assertEquals(false, ctr.isSuccessSet());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this checkpoint records successful execution of unchanged Java tests and their native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.statements.Statement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java)
- [org.minima.kissvm.statements.StatementBlock](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java)
- [org.minima.kissvm.statements.commands.IFstatement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/IFstatement.java)
- [org.minima.kissvm.statements.commands.RETURNstatement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/RETURNstatement.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

[Native counterpart](../../../../../../../../../test/org/minima/tests/kissvm/statements/commands/IFstatementTests.cpp) retains both annotated methods and 22
original assertions. Unchanged Java tests and native counterparts pass in normal
and sanitizer builds. Source constructor naming/list mistakes and the inactive empty-IF rejection comment are preserved; execution instruction counts are asserted.

[Statement command lab](../../../../../../../../new-tests/original-statement-commands.md)
explains reference translation, direct execution, control flow, instruction counts
and source coverage limits. Eight supplemental full-record Java/C++ comparisons
and sixteen independent expected-outcome checks pass across the batch.
[Evidence](../../../../../../../../../validation/original-statement-commands-work-in-progress/progress.json).
No production correction was needed. Full command API and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `b7e9b8eb79fcbf406a7321bc8233e53fc3eac30367fa54f3b6dd52071e2443c5`.
