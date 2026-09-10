# SPHINCSUtils — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCSUtils.java) · `src/org/minima/utils/sphincs/SPHINCSUtils.cpp` (absent)

## Purpose

SPHINCSUtils connects the signature construction to transaction state and a KISS VM script. Its calculateTransactionID method returns message bytes to be signed; it does not itself hash them into the usual transaction ID.

## Follow the algorithm

1. getKISSVMScript substitutes the supplied public-key hex into a fixed script. The script reconstructs input/output text, hashes it, checks the TreeKey signature from states 100/101, then checks 16 outer and inner MMR proofs.
2. calculateTransactionID concatenates input coin IDs in order, then output address, token amount, token ID and uppercase keep-state text with SPHINCS separators. Counts and a COINJOIN separator distinguish the input/output portions. It wraps MiniString bytes as MiniData; empty inputs fail at get(0).
3. setupTransaction stores the serialized TreeKey signature at state 100 and FORS root hash at 101. For each i, it writes HORST root, forest proof, preimage and public-key proof at ports 5*i through 5*i+3, leaving each fifth slot unused.
4. The script binds forest proof sums to 120 and inner public-key sums to 2147450880, the sums of indices 0..15 and 0..65535. Both the proof hash and numeric aggregation matter.
5. After state insertion, precompute output coin IDs through TxPoWGenerator and calculate the transaction’s actual ID. These final calls happen after mutation and can fail after states are present.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. Reuse Transaction/StateVariable/MiniString, signature serialization, and KISS VM semantics. The similarly named TransactionTest uses different output separators and a reduced verification loop, so it is not an interchangeable implementation.

## Understanding the tests

No native counterpart or dedicated assertion for this helper is claimed. Proposed tests should compare the exact reconstructed signing-message bytes between Java helper and VM variables, all 66 inserted state ports, token amount formatting, input/output order, and a full 16-iteration script. Mutating each signature state or output field should identify the expected failing proof/signature check.

- [Related demonstration with different script/separators](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/TransactionTest.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work. Empty outputs also expose a difference: the helper can concatenate no output strings, while the script starts by computing output zero.

## Check your understanding

Why is the method named calculateTransactionID not a substitute for Transaction.calculateTransactionID? Count the state entries inserted for 16 trees.

Pinned Java SHA-256: `9a6d8b400151f2c74404b56559e295462115e5d7bb30066fbd957b9f3d2fbffe`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
