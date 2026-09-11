# Token — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/Token.java](../../../../../../../upstream-study/src/org/minima/objects/Token.java) | [src/org/minima/objects/Token.cpp](../../../../../../src/org/minima/objects/Token.cpp) |

## What this file does

Describes a custom token’s originating coin, denomination scale, total backing amount, name, script and creation block; its identifier commits to those serialized details.

## Follow the logic

1. Store the constructor fields, copying the token script through MiniString, then calculate the token ID. The special IDs 0x00 and 0xFF identify Minima and token creation respectively.
2. Scale an underlying Minima amount into token units by multiplying by ten in a loop scale.getAsInt times. Reverse conversion divides by ten in the same loop. A negative loop count performs no operations; fractional scales are narrowed to int before iteration.
3. Total tokens uses the scaling operation on the stored backing amount. Reported decimal places are 44 minus the narrowed scale; this getter does not independently validate a sensible token denomination.
4. Serialize coin ID with the hash limit, then script, scale, backing amount, name and created block. To derive the ID, wrap those bytes in MiniData and hash that object, including its length prefix. Hashing only the raw token bytes would give a different result.
5. For JSON, a trimmed name beginning with { is inserted as a MiniString object rather than parsed as JSON. Other names are ordinary quoted strings. Malformed brace-prefixed text can therefore produce malformed JSON output.
6. Reading restores all six serialized fields then recalculates the token ID. Java ID calculation catches any Exception and logs it; the convenience converter separately catches IOException and returns null.

## How the translation is approached

Native members are values and token ID is optional. Native JSON explicitly emits brace-prefixed name text as raw JSON to preserve the Java object serialization behavior. It uses bit-level wraparound for decimal-place subtraction. Its ID calculation now catches mapped exceptions and logs them without clearing the optional; Java Error and native allocation failures escape. The converter catches and logs I/O failures. Broader nullable/subclass and logger-failure behavior remains open.

## What the tests tell us

The token branch crosses scales -1, 0, 1, 6, 18 and 44 with ordinary names, JSON-looking names, malformed JSON-looking names and Unicode. It compares wire bytes, total/decimals JSON and the recomputed token ID. Coin fixtures also attach token metadata. No dedicated assertion here proves all possible scale narrowing or caught-error logging behavior.

Evidence: [Java oracle](../../../../../../support/ObjectOracle.java), [native harness](../../../../../../support/objects.cpp) and [object differential driver](../../../../../../tools/object_differential.py). The inspected object report records 9,244 matching observations across the complete corpus. That total is not this file’s test count; generic ERROR normalization does not prove exact exception classes.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Crypto](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Token](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L67), [Token](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L76), [Token](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L80), [getScaledTokenAmount](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L92), [getScaledMinimaAmount](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L101), [getScale](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L110), [getAmount](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L114), [getTotalTokens](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L118), [getDecimalPlaces](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L122), [getName](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L126), [getTokenScript](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L130), [getCoinID](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L134), [getCreated](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L138), [getTokenID](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L142), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L146), [calculateTokenID](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L176), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L204), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L214), [convertMiniDataVersion](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L228), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/Token.java#L248).

## Check your understanding

A backing amount of 0.001 at scale 3 represents how many token units? Why is replacing the repeated division loop with one floating-point multiplication unsafe for a parity port?

## Limits and traceability

The stored scale is a MiniNumber, so its textual declaration alone does not guarantee it is a valid small nonnegative integer. Validation at token-creation/consensus call sites must be studied separately. Counterpart status remains partial and full parity not_verified.

Pinned Java SHA-256: `01fad2200cb13f9f187d62ac69dd347dd1dbdf9ddc102d693e17e03a80563842`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Transaction-object Streamable integration

The reader assigns all six serialized fields in source order, then computes the token ID. A failure before that calculation leaves the prior ID intact. ID calculation now logs caught exceptions without clearing an existing ID; Java Error and native allocation failures escape. Conversion logs IOException and returns no token; other runtime exceptions escape. The test independently hashes the length-framed serialized token, preserving the extra MiniData layer.

The [transaction-object interface lab](../../../../../new-tests/streamable-transaction-objects.md) explains the source-order tests and existing generic callers. Both complete builds pass. The expanded interface suite passes 2,771 comparisons and 20,914 assertions per build across 20 types, with matching transcripts and empty native stderr. All seven affected regressions also pass in both builds. Full nullable/reference/API and node parity remain unverified.


## Separate hash-writer update

The [hash-writer lab](../../../../../new-tests/hash-writers.md) explains distinct ordinary/hash dispatch, mutable
length limits, partial output and receiver lifetime during field replacement.
Both builds pass 1,848 comparisons and 12,752 independent assertions for the
represented paths. This update does not establish full field identity or class parity.
