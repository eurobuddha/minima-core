# Mining and posting a stored transaction

The three commands have different endpoints. txnpost constructs a TxPoW and either queues it for mining or mines synchronously with posting enabled. txnmine mines synchronously with posting disabled and returns exported bytes. txnminepost decodes supplied bytes and queues a MAIN_TXPOWMINED message; it does not mine or validate that payload itself.

Read [txnpost](../../../upstream-study/src/org/minima/system/commands/txn/txnpost.java), [txnmine](../../../upstream-study/src/org/minima/system/commands/txn/txnmine.java), [txnminepost](../../../upstream-study/src/org/minima/system/commands/txn/txnminepost.java), and their [native package](../../src/org/minima/system/commands/txn).

## Follow state changes, not just the final reply

Posting rejects a negative burn before row lookup. It then clears the cached transaction-check flag, optionally attaches proofs/scripts, precomputes output IDs and recalculates the transaction ID. A burn transaction excludes coins already present in witness proofs and links to that ID. Generation preserves the row's transaction/witness owners. The size limit is checked against the current chain tip before the miner is called.

The wrapper parses txndelete only after posting/mining succeeds. A malformed deletion flag can therefore fail after a message was already queued. If deletion is requested, the generated object keeps the transaction/witness alive after its row disappears. A failed miner result becomes the source's 120-second failure message.

Mining by id takes precedence over supplied data. Mining by data decodes a separate TxnRow and does not add it to the custom database. It clears the cached flag and recomputes IDs before generation. Unlike txnpost, this command has no separate size-limit check.

A decoded null payload is still posted by txnminepost when Main exists; the later toJSON access then fails. That surprising ordering belongs in the test: an error response is not proof that no side effect occurred.

## Reuse and test boundaries

The commands reuse the actual database, txnutils, generator and miner. The existing getMiner adapter and a Main-message receiver expose the missing native Main integration explicitly. Missing receivers throw at lookup; they do not fake successful posting.

The wallet command fixture reuses the existing dormant-miner setup. Its subclass fixes only the generated PRNG's 32 bytes on entry to the miner, then calls the actual asynchronous queue or synchronous mining body. Header fields, difficulty, transaction data and proofs remain unchanged. This makes random-dependent hashes and mining results comparable. The source's clock clamp is exercised with old fixture block times.

The fixture also injects a false mining result and thrown/posting failures to isolate caller behavior without waiting 120 seconds for every failure case. Separate existing miner tests cover the actual timed failure path. Main messages are recorded locally; no external network or full Main constructor runs. Java methods are overridable, so the native miner entry methods now permit the same dispatch.

## Run and interpret the evidence

The [corpus](../../tools/transaction_mining_corpus.py) supplies 207 programs with 699 operations. They pass against freshly compiled pinned Java in both normal and AddressSanitizer/UndefinedBehaviorSanitizer builds, including 1,893 recording-SQL calls per build. Runtime: 54.49 seconds normal and 62.54 seconds sanitizer. All sixteen selected affected regressions also have passing final results in each build; the older transaction protocol test required the harness correction described below.

The programs compare metadata, typed parameters, ID/data precedence, malformed exported rows and TxPoWs, missing receivers, size-limit failures, synchronous and asynchronous paths, signed burns, input proofs, row deletion and retained reservations. Success assertions require actual successful mining responses and the expected queue/posting behavior; signed-burn cases require a signature, inputs and the correct link to the main transaction ID.

The first failure revealed that native convenience decoders returned null but logged through a different destination. TxnRow and TxPoW now reuse Coin's MinimaLogger/NativeThrowable pattern for caught IOException. The earlier offline transaction harness was then changed to route diagnostics to stderr, like its Java counterpart, so logs cannot corrupt its JSON protocol. This fixes observation plumbing; it does not suppress the production log.

Run `ctest --test-dir cpp/build --output-on-failure -R '^transaction_mining_commands_differential$'`, and repeat with `cpp/build-sanitized`. The driver compares actual replies, stored wire, three cache flags, logs, SQL calls and mining snapshots after each operation. Exact log prefixes, stack frames and Java-specific null/cast diagnostic wording are outside this comparison.

This lab does not claim a functioning full node or durable native SQL storage.
