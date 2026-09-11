# ScriptTokenizer — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/tokens/ScriptTokenizer.java](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java) | [src/org/minima/kissvm/tokens/ScriptTokenizer.cpp](../../../../../../../src/org/minima/kissvm/tokens/ScriptTokenizer.cpp) |

## What this file does

Split script text into typed tokens before syntactic parsing.

## Follow the logic

1. Walk Java UTF-16 positions from the start, tracking whether the previous consumed character was whitespace. Recognize whitespace, single-character operators, doubled shift characters and parentheses.
2. A square bracket starts a nested-bracket string scan. The scanner emits the accumulated text even when the closing bracket is missing; later stages may reject it. Dollar-prefixed words become function-parameter tokens.
3. For an ordinary word, optionally compute case-insensitive forms and classify in source order: command, registered function, Boolean operator, numeric/HEX literal, Boolean constant, global or variable.
4. Commands require preceding whitespace; variables are lowercase letters with a32-character limit. Numeric and HEX recognition use restricted ASCII forms.
5. Throw for an unrecognized word or malformed doubled shift token. A terminal < or > fails while accessing its next character, before the explicit mismatch error.

## How the translation is approached

C++ uses UTF-16 positions and explicit ASCII classifiers in place of Java regexes. Its case helper includes inspected Unicode-to-ASCII mappings relevant to the default/root-locale fixture. Native function names are a pinned separate list; Java reads the mutable MinimaFunction registry. Arbitrary registry mutation and other default locales remain gaps.

## What the tests tell us

The driver runs each script in both case modes, includes all built-in and command names, nested/unclosed strings, missing command whitespace, malformed shifts, nonbreaking spaces and Unicode case examples such as ligatures, long-s and Kelvin sign. It adds300 deterministic random strings and compares full token triples or normalized errors.

The actual [KissOracle.java](../../../../../../../support/KissOracle.java) and [kiss.cpp](../../../../../../../support/kiss.cpp) execute the operations built by [kiss_differential.py](../../../../../../../tools/kiss_differential.py). Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing [normal report](../../../../../../../validation/student-linked-evidence/build/kiss-differential-results.json) records 1,451 cases passed and zero failures; no new run was performed for this lesson.

## Dependencies and reading map

Direct Minima imports: [MinimaParseException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java), [MinimaFunction](../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java).

Method navigation (declarations with bodies found in this file): [ScriptTokenizer](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L67), [ScriptTokenizer](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L71), [getNextWord](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L79), [isNumeric](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L100), [isHex](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L104), [isVariable](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L108), [isGlobal](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L112), [isWhiteSpace](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L116), [tokenize](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java#L120).

## Check your understanding

Why can a token scanner accept an unclosed bracket string without the complete program being valid?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `d2e407791640b5fc0e37fd50f36ec6479200a7f24ddd20139e962686704584c1`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
