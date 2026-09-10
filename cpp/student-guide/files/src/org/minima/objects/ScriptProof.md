# ScriptProof — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/ScriptProof.java](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java) | [src/org/minima/objects/ScriptProof.cpp](../../../../../../src/org/minima/objects/ScriptProof.cpp) |

## What this file does

Commits to script text and a Merkle path, deriving the address at which that script can be used.

## Follow the logic

1. For a script alone, Java builds a one-leaf MMR with sum zero and obtains its proof. A single leaf has no sibling chunks.
2. For a supplied proof, keep the script and proof together. Hash the MiniString script as a zero-sum leaf, then fold the proof to obtain a root hash.
3. Construct an Address from that root hash. This means the address includes the proof context, not merely a direct hash of script characters.
4. Write script then proof. On reading, recompute the address rather than reading a separately serialized cached address. JSON includes script, address and proof.

## How the translation is approached

The native single-script constructor delegates to an empty default MMRProof, reusing the single-leaf equivalence. It stores script and proof by value and recalculates the address in both normal construction and deserialization.

## What the tests tell us

The scriptproof branch compares four script strings through the constructors and derived addresses. The broader script/address and leaf branches include Unicode text, and byte round trips exercise address recomputation. Direct supplied-proof constructor equivalence still needs an explicitly linked fixture.

The [Java object oracle](../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [MMR](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java), [MMRData](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [MMREntry](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java), [MMRProof](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [ScriptProof](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L34), [ScriptProof](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L36), [ScriptProof](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L59), [calculateAddress](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L74), [getScript](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L86), [getProof](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L90), [getAddress](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L94), [getAddressData](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L98), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L102), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L111), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L117), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java#L124).

## Check your understanding

Why can an empty proof be valid for a single-leaf tree? How would adding one sibling alter the resulting script address?

## Limits and traceability

 The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `57541a26df77e41613560a6f7aa02c2fc8b3a0be3f5f2d49336cb94288337b3f`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
