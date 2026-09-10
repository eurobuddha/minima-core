# MegaMMR — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/mmr/MegaMMR.java](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java) | [src/org/minima/objects/mmr/MegaMMR.cpp](../../../../../../../src/org/minima/objects/mmr/MegaMMR.cpp) |

## What this file does

Maintains an MMR plus an identifier-indexed set of unspent coins, with optional aggressive pruning.

## Follow the logic

1. addBlock applies previous peaks, spent-input proofs and new output leaves to the current MMR. Spent coin IDs are removed from the unspent table; output deep copies get fresh entry numbers and creation heights before insertion.
2. Prunability is a configured policy: when mega-pruning is enabled, a non-32-byte address qualifies, and optional flags also qualify coins with state or non-Minima token IDs. This is a policy decision, not execution of the coin’s script.
3. After applying a block, compare the computed root with the TxPoW root. A mismatch logs a message; it does not reject the block, roll back changes or return false.
4. Scanning updates leaf unspendable flags from current coin policy. Tree pruning records removed entry numbers, and a replacement coin table retains only coins not in that pruned set.
5. Writing can mutate the object: when enabled, it prunes before writing version 1, MMR and coins in Java Hashtable enumeration order. Thus serialization is not a pure observation.
6. Reading consumes but does not validate version, replaces the MMR/table, clears MMR finalization, loads coins by ID and rescans/prunes using current settings. Duplicate IDs overwrite earlier table entries.
7. clear replaces both structures. isEmpty checks stored MMR entries rather than the unspent-coin count, so an empty coin table alone is insufficient.

## How the translation is approached

C++ reuses applyBlockMMR for the same state transition as tree nodes and JavaHashtable for observable coin iteration. MMR and coins have shared ownership; the table container itself is assigned in place, which can differ from Java retained-table aliases after replacement. Log output currently uses std::clog rather than the full Java logger.

## What the tests tell us

The [object oracle](../../../../../../../support/ObjectOracle.java) and [native harness](../../../../../../../support/objects.cpp) build MegaMMRs over one or three rounds, spend every third eligible entry in later rounds and optionally prune after each block. The [driver](../../../../../../../tools/object_differential.py) combines eight output sizes, sixteen flag settings and two round counts, plus empty state: 257 initial MegaMMR fixtures, then derived read/trailing/truncation cases. Wire bytes, MMR JSON, coin JSON/order and emptiness are compared. The existing object report has 9,244 passing total comparisons; this is a whole-suite count. Java fixture logs are suppressed, so root-mismatch logging is not verified there.

## Dependencies and reading map

Direct Minima imports: [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [CoinProof](../../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java), [Token](../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [TxBlock](../../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [GeneralParams](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MiniFile](../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MiniFormat](../../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [MegaMMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L31), [getMMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L40), [isEmpty](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L48), [addBlock](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L55), [isPrunable](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L128), [scanUnspendable](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L157), [pruneUnspendable](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L179), [clear](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L226), [loadMMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L231), [saveMMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L236), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L244), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L269), [main](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L297), [getCoinData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L410), [getCoinData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MegaMMR.java#L414).

## Check your understanding

Why could calling serialise reduce getAllCoins().size()? Explain which configuration causes it and why a test must snapshot state after writing as well as compare bytes.

## Limits and traceability

Policy changes between creation/read/write, retained table aliases, logger/error fidelity and arbitrary file failures remain open. A matching root fixture does not prove full consensus validation: this method only logs mismatches. Partial counterpart, parity not_verified.

Pinned Java SHA-256: `071b2e965d2ae1b99b5b99827f210f94b8203ac57cedb6401c378693a1a88593`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
