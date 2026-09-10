# GenesisTxPoW — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/genesis/GenesisTxPoW.java](../../../../../../../../upstream-study/src/org/minima/system/genesis/GenesisTxPoW.java) | [src/org/minima/system/genesis/GenesisTxPoW.cpp](../../../../../../../src/org/minima/system/genesis/GenesisTxPoW.cpp) |

## What this file does

Constructs the special first transaction/block candidate by spending the predefined genesis coin to a supplied destination.

## Follow the logic

1. Create the genesis MMR. Initialize transaction and block difficulty to MAX_HASH, nonce to 256, current time, block number 1 and all superparents to 0x00.
2. Add GenesisCoin as the transaction input. Add a one-billion Minima output with COINID_OUTPUT and the supplied destination address bytes.
3. Attach the genesis coin proof to peak zero and the TRUE_ADDRESS script proof to the witness. These explain where the input comes from and how its script can be satisfied.
4. Set the body hash and calculate transaction identifiers, then build a TxBlock over the genesis MMR. Java constructs a TxPoWTreeNode and obtains the resulting MMR root and total.
5. Put that root and total in the header, calculate the TxPoW identifier, log it, and explicitly mark both block and transaction proof-of-work flags true.

## How the translation is approached

The native constructor uses the same object constructors and constants. It replaces Java’s TxPoWTreeNode route with the existing constructBlockMMR support routine, so that adapter is a material dependency to review. C++ logs through std::clog.

## What the tests tell us

Five genesis destinations are compared. Before comparing, both harnesses overwrite time with 1788883200000 and PRNG with 000102, then recalculate body hash and TxPoW ID. This makes the fixture deterministic; it is not evidence that independent real-time constructor calls produce identical IDs. Output includes the genesis coin and initial MMR as well as the resulting TxPoW.

The [Java object oracle](../../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [TxPoWTreeNode](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [Address](../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [CoinProof](../../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java), [ScriptProof](../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java), [Token](../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [TxBlock](../../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [Witness](../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [MMREntryNumber](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java), [MMRProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [GlobalParams](../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [Crypto](../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [GenesisTxPoW](../../../../../../../../upstream-study/src/org/minima/system/genesis/GenesisTxPoW.java#L26).

## Check your understanding

Which fields must change if the destination changes? Why does the test pin time and PRNG before comparing IDs?

## Limits and traceability

Full-node startup, persistence, networking and chain acceptance are outside this constructor fixture. The support MMR adapter is not evidence that all TxPoWTreeNode behavior has been mapped. The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `5c736bf2b832b2e9480ca291446661100bee12f255863dda33c4c1f9347232c6`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
