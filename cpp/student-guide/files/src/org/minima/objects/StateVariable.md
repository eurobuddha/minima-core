# StateVariable — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/StateVariable.java](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java) | [src/org/minima/objects/StateVariable.cpp](../../../../../../src/org/minima/objects/StateVariable.cpp) |

## What this file does

A typed state slot carried by a coin or transaction. Its port identifies the slot, while the type determines both the canonical text and the binary payload.

## Follow the logic

1. Reject ports outside 0 through 255. Reject a value whose trimmed text is empty, but do not trim the value before classifying it. Leading whitespace can therefore change which branch is taken.
2. Classify in order: a Minima address beginning Mx, hex beginning 0x, TRUE/FALSE ignoring case, bracketed script text, otherwise a number. Address and hex inputs become canonical hex; booleans become uppercase; numbers use MiniNumber canonical text.
3. Use type tags 1 for hex, 2 for number, 4 for string and 8 for boolean. Brackets remain part of a string value; they are not stripped here.
4. Serialize the one-byte port and type, followed by the corresponding MiniData, MiniNumber, MiniString or boolean byte. The reader chooses its payload decoder from the type byte and rejects unknown tags.
5. JSON reports port, type and canonical data text. Two spellings such as 1.00 and 1 can therefore describe the same normalized numeric state.

## How the translation is approached

The native object stores MiniByte and MiniString values and dispatches serialization with a switch. Its constructor uses std::invalid_argument for validation; the differential harness normalizes failures, so matching rejection alone does not prove the Java exception type was retained.

## What the tests tell us

The state fixture crosses ports -1, 0, 127, 128, 255 and 256 with typed and invalid values, comparing canonical JSON and binary bytes. This probes unsigned-byte boundaries as well as type classification.

The [Java object oracle](../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

The [coin and proof reader lab](../../../../../new-tests/coin-streams.md) exercises invalid type bytes nested inside coins. The native reader now includes the actual type in its I/O diagnostic, matching Java. That earlier lab did not establish StateVariable in-place mutation or its invalid-type writer behavior; the interface follow-up below addresses them.

The raw-command review found that Java accepts `falſe` (long s) as FALSE, while the native ASCII comparison rejected it. The constructor now reuses [JavaStringIgnoreCase](../../../../../../src/support/JavaStringIgnoreCase.hpp). The [raw lab](../../../../../new-tests/raw-transaction-commands.md) checks both child txnstate text and real exported Boolean state bytes. The expanded raw suite passes 656 programs, and the existing object suite passes 9,464 cases, in both builds. Full nullable/reference and locale APIs remain unverified.

## Dependencies and reading map

Direct Minima imports: [MiniByte](../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [StateVariable](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L42), [StateVariable](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L44), [getPort](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L87), [getType](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L91), [getData](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L95), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L99), [toString](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L108), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L113), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L139), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java#L167).

## Check your understanding

Predict the type and stored text for TRUE, [TRUE], 1.00 and 0x01. Then explain why a leading space before 0x01 is not equivalent to trimming it first.

## Limits and traceability

 The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `7f542a4fe357b1790cb0771ba3572461e8f8619f42f2871d0f7d8de0193458dd`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Transaction-object Streamable integration

The in-place reader now assigns port, then type, then the decoded text. A failed payload read can leave new tags with old text. Unknown tags throw IOException during reading, but their writer emits only port and type, matching Java. Boolean payloads treat every nonzero byte as true and write a canonical 1 or 0.

The [transaction-object interface lab](../../../../../new-tests/streamable-transaction-objects.md) explains the source-order tests and existing generic callers. Both complete builds pass. The expanded interface suite passes 2,771 comparisons and 20,914 assertions per build across 20 types, with matching transcripts and empty native stderr. All seven affected regressions also pass in both builds. Full nullable/reference/API and node parity remain unverified.
