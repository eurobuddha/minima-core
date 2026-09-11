# MMRProof — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/mmr/MMRProof.java](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java) | [src/org/minima/objects/mmr/MMRProof.cpp](../../../../../../../src/org/minima/objects/mmr/MMRProof.cpp) |

## What this file does

An ordered list of sibling commitments that lets a verifier reconstruct an MMR ancestor from one leaf.

## Follow the logic

1. Store a block time and a chain of chunks. Each chunk holds a one-byte left flag and an MMRData sibling containing both hash and value.
2. Start calculateProof with the supplied leaf. For each chunk, place the sibling on the left when its flag is true; otherwise place it on the right. Replace the running value with the new parent.
3. An empty chain returns the input leaf unchanged. The method calculates a candidate root; it does not decide whether that root is trusted.
4. Serialize block time, the chunk count as MiniNumber, then each left flag and sibling. The reader converts the count to int and reads chunks in sequence.
5. Java replaces its chain before reading the count and appends completed chunks; malformed input can therefore leave a partially changed existing object. The convenience MiniData converter propagates IOException.

## How the translation is approached

C++ uses a vector of chunks within shared outer proof storage. Ordinary proof handles share that storage. The in-place reader updates block time, clears the chain before reading its count and appends complete chunks. The static decoder creates a fresh proof and invokes that same reader. The reader preserves the original MiniByte flag after interpreting its truth value, maintaining noncanonical flag bytes during round trips.

## What the tests tell us

Ninety seeded proof cases vary chunk counts, directions, sibling text and sums. The oracle compares serialized proof, JSON and the calculated root. Generated wire cases also test truncation and trailing data. Comparing a reconstructed root with a trusted chain root remains the caller’s job.

The [Java object oracle](../../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,464 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

The [coin and proof reader lab](../../../../../../new-tests/coin-streams.md) adds every-prefix reads into existing proofs, retained outer proof handles and exact error/offset observations. Individual chunks and their MMRData still use native value semantics.

## Dependencies and reading map

Direct Minima imports: [MiniByte](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [MMRProofChunk](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L25), [MMRProofChunk](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L27), [isLeft](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L32), [getMMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L36), [toJSON](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L40), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L48), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L54), [MMRProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L70), [MMRProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L74), [getBlockTime](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L78), [addProofChunk](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L82), [addProofChunk](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L86), [getProofChunk](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L90), [getProofLength](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L94), [calculateProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L98), [toJSON](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L117), [toString](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L134), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L139), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L154), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L169), [convertMiniDataVersion](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java#L178).

## Check your understanding

Write the parent expressions for two chunks: first left, then right. Explain why reversing their order changes the result even if all sibling hashes are unchanged.

## Common interface integration

This class now implements the native Streamable interface. Its existing byte
format is retained; reads dispatch into the same object and writers append to the
supplied output. The [interface lab](../../../../../../new-tests/streamable-interface.md) tests base-reference calls,
partial reads and existing generic file/hash/network-message helpers. Both normal and sanitizer
runs pass 909 comparisons and 3,850 assertions across all five adopted types;
this does not establish full interface adoption or complete class parity.

## Limits and traceability

 The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `d38fac343155d3a60e2bbf8447b50b8a7b513f4660d650d5cf3f778c21e46a63`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## Incremental output reused by HORST and FORS

The native `writeDataStream(vector&)` now exposes the existing proof encoding as
an append operation. It writes block time, chunk count, and then each direction
byte and MMRData. This preserves the partial prefix if a later oversized hash
throws. HORSTSignature and FORSSignature share this method; serialise wraps it
with a fresh vector. The [FORS lab](../../../../../../../student-guide/new-tests/fors-signatures.md)
checks nested failures, while the existing object and HORST comparisons regress
unchanged successful proof bytes. This does not model every failing Java output stream.
