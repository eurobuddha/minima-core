# StatementParser — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/StatementParser.java](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java) | [src/org/minima/kissvm/statements/StatementParser.cpp](../../../../../../../src/org/minima/kissvm/statements/StatementParser.cpp) |

## What this file does

Group command tokens into executable statements and nested blocks.

## Follow the logic

1. Increase the supplied parse depth and reject a depth above the contract limit. At each new statement require a COMMAND token.
2. LET distinguishes a named variable from a parenthesized list of index expressions, finds the equals sign, and parses the right-hand expression up to the next command. EXEC/MAST/ASSERT/RETURN similarly consume an expression to the next command.
3. IF collects a condition through THEN, finds the matching same-depth ELSE/ELSEIF/ENDIF boundary and recursively parses each action block. ELSE becomes a constant TRUE condition.
4. WHILE collects its condition through DO, scans through the matching ENDWHILE and recursively parses the body. Boundary scanners retain nested delimiters so inner blocks remain intact.
5. Return a StatementBlock containing the constructed statements. Some missing-terminator paths fail through list access or discard the last available token; do not silently replace them with a different grammar.

## How the translation is approached

C++ uses the existing ExpressionParser and statement classes. It replaces the source’s recursive delimiter scanners with an iterative nesting counter, preserving the token slices passed to recursive block parsing. Source depth addition uses explicit32-bit wraparound. Generic malformed-list failures are normalized by current tests.

## What the tests tell us

The driver contains ordinary/nested IF, ELSEIF, WHILE and LET array programs, missing right-hand expressions and delimiters, stray ELSE, empty branches and depths around64. Selected malformed WHILE cases expose the source’s lenient boundary behavior. The actual Contract parse flags and subsequent execution are compared.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [MinimaParseException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java), [ConstantExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java), [Expression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [ExpressionParser](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java), [ASSERTstatement](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/ASSERTstatement.java), [EXECstatement](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/EXECstatement.java), [IFstatement](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/IFstatement.java), [LETstatement](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java), [MASTstatement](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/MASTstatement.java), [RETURNstatement](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/RETURNstatement.java), [WHILEstatement](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/WHILEstatement.java), [LexicalTokenizer](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java), [ScriptToken](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java), [BooleanValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java).

Method navigation (declarations with bodies found in this file): [parseTokens](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java#L38), [getElseOrElseIfOrEndIF](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java#L302), [getEndWHILE](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java#L356), [getTokensToNextCommand](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java#L402), [getTokensToRequiredCommand](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java#L422), [getTokensToNextEquals](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java#L457).

## Check your understanding

When finding an outer ELSE, why must the scanner ignore an ELSE belonging to a nested IF? Draw the depth counter transitions.

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `4f1b1ece8e3dd7746653c9b10c1a487ea7bc6e7fb41dd81d9b46ae0405ec3ab7`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
