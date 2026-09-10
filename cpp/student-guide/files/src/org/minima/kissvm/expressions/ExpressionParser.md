# ExpressionParser — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/expressions/ExpressionParser.java](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java) | [src/org/minima/kissvm/expressions/ExpressionParser.cpp](../../../../../../../src/org/minima/kissvm/expressions/ExpressionParser.cpp) |

## What this file does

Build an expression tree using recursive-descent precedence levels.

## Follow the logic

1. Reject an empty top-level token list. Parse through a LexicalTokenizer and then require that all tokens were consumed.
2. From weakest to strongest, parse Boolean combinations, comparisons, HEX bitwise operators, the shared +/−/%/shift level, multiplication/division, then unary/base expressions. Operators at the same level are accumulated left-to-right.
3. Each level consumes recognized operators and pushes back the first unrecognized token so its caller can handle it.
4. Base expressions include literals, variables, globals, Boolean constants, functions and parentheses. A leading minus consumes a following numeric token directly; NEG is the general unary expression operator.
5. Function parsing requires parentheses, builds each argument expression, enforces parameter/depth limits, then checks the function’s declared arity. Parentheses recurse and require a matching close token.

## How the translation is approached

C++ translates these levels directly with shared expression nodes. It preserves the source’s unusual precedence: modulo shares the addition/shift level, and all Boolean combinations share one level. The LexicalTokenizer constructor ignores its supplied initial depth in both versions; that quirk affects where depth checks start.

## What the tests tell us

Explicit expressions include TRUE OR FALSE AND FALSE, 1+7%3*2, NOT TRUE EQ FALSE, mixed bitwise operators, parenthesized arithmetic, -a, -(1), NEG NEG 5, extra tokens and missing parentheses. Depth cases use 1/63/64/65/70 nested groups; the harness compares tree display and execution.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [MinimaParseException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java), [MinimaFunction](../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [LexicalTokenizer](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java), [ScriptToken](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java), [BooleanValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [NumberValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [getExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java#L30), [getExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java#L56), [getRelation](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java#L90), [getLogic](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java#L118), [getAddSub](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java#L140), [getMulDiv](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java#L165), [getPrimary](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java#L184), [getBaseUnit](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ExpressionParser.java#L208).

## Check your understanding

Draw the parse tree of TRUE OR FALSE AND FALSE using this source. Why must a port not assume Java or C++ operator precedence?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `3f5b210d435973f89b903fe022dd39d9e3b79871d6a54f716ee70e539c6c4829`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
