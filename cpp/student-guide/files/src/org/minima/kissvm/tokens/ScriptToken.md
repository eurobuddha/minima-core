# ScriptToken — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/tokens/ScriptToken.java](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java) | [src/org/minima/kissvm/tokens/ScriptToken.cpp](../../../../../../../src/org/minima/kissvm/tokens/ScriptToken.cpp) |

## What this file does

Pair a token kind with its source text and render a stable diagnostic label.

## Follow the logic

1. Construct the token from an integer type code and text.
2. Map known codes to names such as COMMAND, FUNCTION or VALUE; unknown codes return the string null.
3. Render TYPE:text and expose the raw code/text.
4. The static tokenize helper creates a ScriptTokenizer and delegates to it.

## How the translation is approached

C++ stores the same integer constants and strings, including the source spelling TOKEN_FUNCTIION. Its explicit toString replaces Java Object dispatch.

## What the tests tell us

For each tokenized script, the values/tokens harness records numeric type, raw token text and toString. This checks the three views together. Unknown manually constructed type codes are not claimed as directly tested by this driver.

The actual [KissOracle.java](../../../../../../../support/KissOracle.java) and [kiss.cpp](../../../../../../../support/kiss.cpp) execute the operations built by [kiss_differential.py](../../../../../../../tools/kiss_differential.py). Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing [normal report](../../../../../../../build/kiss-differential-results.json) records 1,451 cases passed and zero failures; no new run was performed for this lesson.

## Dependencies and reading map

Direct Minima imports: [MinimaParseException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java).

Method navigation (declarations with bodies found in this file): [ScriptToken](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java#L31), [getTokenType](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java#L36), [getTokenTypeString](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java#L40), [getToken](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java#L72), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java#L77), [tokenize](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java#L84).

## Check your understanding

Why retain a misspelled public constant while printing the correctly spelled label FUNCTION?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `d12d21165ec5f1057126382c061f8bdf58a0603cb43e99f45151789018cdc967`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
