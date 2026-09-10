# txncheck — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncheck.java) · [Native command](../../../../../../../../src/org/minima/system/commands/txn/txncheck.cpp)

## Purpose

txncheck reports amounts, witness counts and several independent validity checks for a stored transaction.

## Follow the command

1. Collect distinct token IDs from inputs then outputs, treating TOKENID_CREATE as Minima. For each compute input/output totals and difference input minus output. Also report raw input-minus-output total as burn, output JSON, counts and Transaction.checkValid.
2. Generate a temporary TxPoW with the existing transaction/witness, clear the transaction’s monotonic cache and run checkTxPoWBasic. This wrapper does not explicitly precompute or recalculate transaction ID before reading it for signatures.
3. For each signature set a TreeKey public root and verify against the stored transaction ID. Record per-signature validity and allsignaturesvalid, initialized true so an empty signature list leaves it true.
4. Check MMR only when proof count equals input count, using short-circuit AND. Independently execute script checks against the current tip even if basic, signature or count checks failed.
5. Return validity details. The final validtransaction expression is basic AND mmr AND scripts; it does not explicitly include the separately computed allsignaturesvalid boolean.

## Translation and dependencies

The native command reuses Command parameter/reply helpers, the actual MinimaDB custom transaction store, TxPoWGenerator, TxPoWChecker, TreeKey and Transaction totals. Its factory is registered in CommandRunner. The generator now accepts shared transaction/witness owners, so clearing the stored transaction’s validation flag also affects the temporary TxPoW. Independent report fields and the source’s exact final boolean composition are preserved.

## Understanding the tests

The [transaction-check lab](../../../../../../../../student-guide/new-tests/transaction-check.md) explains the source-derived corpus and its validation results. The test seeds actual chain/MMR and transaction objects from ConsensusOracle, then runs the actual command directly and through the registry. It compares replies, failure categories, logs, stored bytes and cached validation flags. Explicit assertions distinguish valid scripts, invalid extra signatures, and retained ownership after mutation or row deletion.

- [Comparison driver](../../../../../../../../tools/wallet_commands_differential.py)
- [Command and ownership cases](../../../../../../../../tools/transaction_check_corpus.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

## Limits

A diagnostic result is conditional on the current tip and helper checks, not future acceptance. Amount validity, signature validity and script satisfaction are distinct questions. The tests use a recording SQL receiver and controlled chain fixtures; they do not start a full node or post a transaction.

## Check your understanding

Which fields explain why zero signatures can still produce allsignaturesvalid:true without proving spendability?

Pinned Java SHA-256: `89bfaaa98453f0ae97aa8af709fcbff4a12d62860281a2b93eca238f02b368de`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
