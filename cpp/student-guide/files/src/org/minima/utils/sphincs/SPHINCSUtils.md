# SPHINCSUtils — teaching guide

**Teaching status:** Authored explanation of the Java source, native transaction integration, and validation in progress.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCSUtils.java) · [Native implementation](../../../../../../../src/org/minima/utils/sphincs/SPHINCSUtils.cpp)

## Purpose

SPHINCSUtils connects the signature construction to transaction state and a KISS VM script. Its calculateTransactionID method returns message bytes to be signed; it does not itself hash them into the usual transaction ID.

## Follow the algorithm

1. getKISSVMScript substitutes the supplied public-key hex into a fixed script. The script reconstructs input/output text, hashes it, checks the TreeKey signature from states 100/101, then checks 16 outer and inner MMR proofs.
2. calculateTransactionID concatenates input coin IDs in order, then output address, token amount, token ID and uppercase keep-state text with SPHINCS separators. Counts and a COINJOIN separator distinguish the input/output portions. It wraps MiniString bytes as MiniData; empty inputs fail at get(0).
3. setupTransaction stores the serialized TreeKey signature at state 100 and FORS root hash at 101. For each i, it writes HORST root, forest proof, preimage and public-key proof at ports 5*i through 5*i+3, leaving each fifth slot unused.
4. The script binds forest proof sums to 120 and inner public-key sums to 2147450880, the sums of indices 0..15 and 0..65535. Both the proof hash and numeric aggregation matter.
5. After state insertion, precompute output coin IDs through TxPoWGenerator and calculate the transaction’s actual ID. These final calls happen after mutation and can fail after states are present.

## Native translation and reuse

The C++ implementation reuses Transaction, StateVariable, Coin token scaling,
MiniString, signature writers and TxPoWGenerator's output-ID calculation. The script
literal is copied exactly from the pinned source. The serialization-to-MiniData
helper was extracted unchanged from FORSSignature into a shared support header;
it logs only I/O exceptions and returns an empty optional. The caller's subsequent
null dereference is represented as a JavaNullPointerException.

State writes are deliberately sequential. A missing inner proof can leave that
iteration's root, outer proof and secret in the transaction. The cached transaction
ID is unchanged until every iteration and output-ID update completes. Each gap
port 5*i+4 and unrelated existing state remains present.

## Understanding the tests

The [transaction lab](../../../../../../../student-guide/new-tests/sphincs-transactions.md)
was written before execution. Both builds have matched 250 initial Java/native
cases: script replacement, independent message bytes, input/output ordering,
scaled tokens, ELTOO output IDs, all 66 state writes, and failures at each of 16
positions in each nested list. Null components, preserved unrelated state and
oversized-proof I/O logging are included. Source and native fixtures reuse the
existing ObjectOracle/CoinFixture coin builders.

Both full builds pass. Object, envelope and KISS VM regressions pass in both builds.
The normal full-size signature was accepted by both VMs in 937 operations. A
malformed-signature error-reporting difference was fixed; both builds pass all
268 replay cases and 1,677 independent assertions. The original sanitizer signing
run completed with the same signatures and the two already-resolved error-text findings; see [saved status](../../../../../../../validation/sphincs-utils-work-in-progress/progress.json).

## Limits and a useful next test

Full node parity, raw Java byte-array aliases, arbitrary subclasses/concurrency,
and unrepresented null Coin fields/list entries remain unverified. Empty outputs
expose a source distinction: the helper accepts them but the script asks for output
zero. Ordinary transaction serialization failures outside the covered fixture
shapes need a separate audit; no full Transaction API equivalence is claimed.

## Check your understanding

Why is the method named calculateTransactionID not a substitute for Transaction.calculateTransactionID? Count the state entries inserted for 16 trees.

Pinned Java SHA-256: `9a6d8b400151f2c74404b56559e295462115e5d7bb30066fbd957b9f3d2fbffe`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).

## Shared demonstration helpers

The tested formatter and 66-state writer now live in [SPHINCSHelpers.hpp](../../../../../../../src/support/SPHINCSHelpers.hpp). TransactionTest supplies its AMOUNT/TOKENID separators and installs state without the final ID updates; SPHINCSUtils retains its original SPHINCS separators and final updates. Both 268-case transaction replays pass after extraction.
