# LexicalTokenizer — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/tokens/LexicalTokenizer.java](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java) | [src/org/minima/kissvm/tokens/LexicalTokenizer.cpp](../../../../../../../src/org/minima/kissvm/tokens/LexicalTokenizer.cpp) |

## What this file does

Maintain a cursor and depth counter over an already-tokenized list.

## Follow the logic

1. Retain the token list and set the cursor to zero. The Java constructor receives zStackDepth but never assigns it, leaving mStackDepth initially zero.
2. getNextToken rejects exhaustion, then returns the current token and advances the cursor. goBackToken rejects position zero before decrementing.
3. Expose whether there are more tokens and whether all have been used. Increment/decrement the depth counter separately.

## How the translation is approached

C++ intentionally ignores the constructor depth parameter and stores tokens in a vector. Explicit unsigned arithmetic plus bit conversion models Java signed32-bit depth wraparound. Copying the token list rather than retaining arbitrary mutable Java list aliases remains an API difference to review.

## What the tests tell us

The lexical driver supplies initial depth 17, reads the observed depth, increments once and decrements twice, then walks positions and tokens. It checks all-used and one-token pushback/re-read. It does not claim all arbitrary list mutations or repeated invalid pushbacks are covered.

The actual [KissOracle.java](../../../../../../../support/KissOracle.java) and [kiss.cpp](../../../../../../../support/kiss.cpp) execute the operations built by [kiss_differential.py](../../../../../../../tools/kiss_differential.py). Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing [normal report](../../../../../../../validation/student-linked-evidence/reports/kiss-differential-results.json) records 1,451 cases passed and zero failures; no new run was performed for this lesson.

## Dependencies and reading map

Direct Minima imports: [MinimaParseException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java).

Method navigation (declarations with bodies found in this file): [LexicalTokenizer](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L21), [getNextToken](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L27), [getCurrentPosition](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L34), [goBackToken](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L38), [checkAllTokensUsed](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L45), [hasMoreElements](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L49), [getStackDepth](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L53), [incrementStackDepth](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L57), [decrementStackDepth](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java#L61).

## Check your understanding

What depths are observed after construction with17 and after +1,−1,−1? Explain why implementing the apparent constructor intent would fail parity.

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `041b3c00ca3a13b10880a311f833f7ce3c6433568d8e060f9a4cfa887cf53ded`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
