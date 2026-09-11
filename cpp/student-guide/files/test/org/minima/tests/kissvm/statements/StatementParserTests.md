# StatementParserTests — Recognizing statement syntax and rejecting malformed programs

Java source: [test/org/minima/tests/kissvm/statements/StatementParserTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Feed tokens to the statement parser for LET, arrays, EXEC, MAST, IF, WHILE, ASSERT, and RETURN. Malformed variants assert parsing exceptions. A parser must reject malformed structure before the VM tries to execute it.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L20) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 2 |
| [`testParsingLETArrays`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L35) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 9 |
| [`testParsingLET`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L99) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 13 |
| [`testParsingEXEC`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L200) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 5 |
| [`testParsingMAST`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L235) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 5 |
| [`testParsingIF`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L270) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 11 |
| [`testParsingWHILE`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L342) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 4 |
| [`testParsingASSERT`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L374) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 8 |
| [`testParsingRETURN`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L429) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 5 |
| [`testParsingException`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L464) | Parse the prepared syntax and inspect its structure or the explicitly expected parse failure. | 1 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 49](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/StatementParserTests.java#L49):

```java
assertThrows(MinimaParseException.class, () -> { String Script = "LET (a = TRUE"; List<ScriptToken> tokens = ScriptToken.tokenize(Contract.cleanScript(Script)); StatementBlock sb = StatementParser.parseTokens(tokens,0); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this checkpoint records successful execution of unchanged Java tests and native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.statements.StatementBlock](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java)
- [org.minima.kissvm.statements.StatementParser](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java)
- [org.minima.kissvm.tokens.ScriptToken](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java)
- [org.minima.utils.MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java)

## C++ translation and validation

[Native counterpart](../../../../../../../../test/org/minima/tests/kissvm/statements/StatementParserTests.cpp) preserves all 15 original assertions.
All ten methods retain 93 parser calls: 78 accepted constructions and fifteen required parse exceptions. Successful source cases do not inspect or execute the resulting blocks; this coverage limit is explicit. Unchanged Java tests and native counterparts pass in normal and sanitizer
builds. The production parsers are reused unchanged.

[Parser lab](../../../../../../../new-tests/original-parser-tests.md) explains
construction versus execution, handcrafted tokens and precedence. Eight supplemental
Java/native full-record comparisons and sixteen independent outcome checks pass.
[Checkpoint evidence](../../../../../../../../validation/original-parser-tests-work-in-progress/progress.json).
Full parser/API/resource-limit and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `fc5e8539cc2fba2ace5c780cb02f889350ad7bb086f8dd05de242816cad92df7`.
