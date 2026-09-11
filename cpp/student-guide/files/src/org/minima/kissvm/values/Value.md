# Value — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/values/Value.java](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java) | [src/org/minima/kissvm/values/Value.cpp](../../../../../../../src/org/minima/kissvm/values/Value.cpp) |

## What this file does

Classify and construct the four KISS value kinds, and enforce type compatibility.

## Follow the logic

1. Test the token in this order: surrounding square brackets, lowercase 0x prefix, exact TRUE/FALSE, then a leading minus or ScriptTokenizer.isNumeric.
2. Construct the matching subclass or throw an invalid-value error. The classifier getValueType follows the same syntax categories but does not construct the value.
3. verifyType requires exact tag equality. checkSameType first compares the two tags, then optionally checks the requested tag.
4. Translate known tags to their display names and return ERROR_UNKNOWN_TYPE for an unknown tag.

## How the translation is approached

The native factory returns shared polymorphic values and stable shared references to static booleans. It retains classification order and delegates real numeric/HEX parsing, so recognition and successful construction remain different questions.

## What the tests tell us

The driver uses value/type/hexnumber/check on the same strings, including bracket mismatches, 0x/0X, uppercase/lowercase Boolean text, exponent-like numbers, a lone minus and whitespace. verify and same check several valid and invalid type tags.

The actual [KissOracle.java](../../../../../../../support/KissOracle.java) and [kiss.cpp](../../../../../../../support/kiss.cpp) execute the operations built by [kiss_differential.py](../../../../../../../tools/kiss_differential.py). Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing [normal report](../../../../../../../validation/student-linked-evidence/build/kiss-differential-results.json) records 1,451 cases passed and zero failures; no new run was performed for this lesson.

## Dependencies and reading map

Direct Minima imports: [ScriptTokenizer](../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java).

Method navigation (declarations with bodies found in this file): [verifyType](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java#L40), [getValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java#L51), [getValueType](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java#L77), [getValueTypeString](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java#L106), [checkSameType](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java#L125), [checkSameType](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java#L131).

## Check your understanding

Can getValueType("-") identify NUMBER while getValue("-") still throws? Follow both methods.

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `94f1521af1abd1de28b337caf561fef96abf69bd248426afba008635a7b95354`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
