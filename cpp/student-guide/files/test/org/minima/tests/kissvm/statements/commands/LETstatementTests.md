# LETstatementTests — Evaluating an expression and storing its result

Java source: [test/org/minima/tests/kissvm/statements/commands/LETstatementTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/LETstatementTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Build LET statements from boolean and arithmetic expressions, inspect their text, then execute them and inspect the named variable. An assignment stores the evaluated value, not the unevaluated syntax tree.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/LETstatementTests.java#L32) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 20 |
| [`testExecution`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/LETstatementTests.java#L177) | Execute prepared objects and inspect outcomes, state changes, instruction counts, or expected exceptions present in this body. | 19 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 39](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/LETstatementTests.java#L39):

```java
assertEquals("LET a = ( TRUE AND TRUE )", ls.toString());
```

[Source line 47](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/LETstatementTests.java#L47):

```java
assertEquals("LET a = ( FALSE EQ FALSE )", ls.toString());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.BooleanExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.expressions.Expression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java)
- [org.minima.kissvm.expressions.FunctionExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/FunctionExpression.java)
- [org.minima.kissvm.expressions.GlobalExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/GlobalExpression.java)
- [org.minima.kissvm.expressions.OperatorExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java)
- [org.minima.kissvm.expressions.VariableExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/VariableExpression.java)
- [org.minima.kissvm.functions.general.GET](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/GET.java)
- [org.minima.kissvm.functions.number.DEC](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/DEC.java)
- [org.minima.kissvm.functions.sigs.CHECKSIG](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/CHECKSIG.java)
- [org.minima.kissvm.statements.commands.LETstatement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/tests/kissvm/statements/commands/LETstatementTests.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `7b6ccfd1c1a541c78630dca6e35e0b6600c6f9256f6c72d670e1311302d1530b`.
