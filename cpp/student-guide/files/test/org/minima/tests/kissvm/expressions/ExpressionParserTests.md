# ExpressionParserTests — Turning tokens into correctly grouped expressions

Java source: [test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Tokenize scripts and invoke the expression parser for boolean, unary, and arithmetic forms. Check node classes and displayed trees. Invalid syntax should raise a parsing exception. Operator precedence is visible in the shape of the parsed tree, even before evaluation.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testBooleanExpressions`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java#L22) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 106 |
| [`testBooleanExpressionsUnary`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java#L416) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 60 |
| [`testOperatorExpressions`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java#L647) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 106 |
| [`testOperatorExpressionsUnary`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java#L1041) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 60 |
| [`testOperatorExpressionsExceptions`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java#L1272) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 2 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 31](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java#L31):

```java
assertEquals(expr.getClass(), BooleanExpression.class);
```

[Source line 32](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java#L32):

```java
assertEquals("( TRUE " + Op + " TRUE )", expr.toString());
```

[Source line 349](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/expressions/ExpressionParserTests.java#L349):

```java
assertThrows(MinimaParseException.class, () -> { String Script = "LEN 0x12345678 " + Op + " LEN 0x12345678"; List<ScriptToken> tokens = ScriptToken.tokenize(Contract.cleanScript(Script)); Expression expr = ExpressionParser.getExpression(tokens,0); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.BooleanExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java)
- [org.minima.kissvm.expressions.Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java)
- [org.minima.kissvm.expressions.ExpressionParser](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java)
- [org.minima.kissvm.expressions.OperatorExpression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java)
- [org.minima.kissvm.tokens.ScriptToken](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/tests/kissvm/expressions/ExpressionParserTests.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `52660832d9eaee9b0a9872273049a78319ebf962d391dcd9763f1a204ce5a6b8`.
