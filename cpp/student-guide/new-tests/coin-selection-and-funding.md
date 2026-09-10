# Lab: selecting coins, funding transactions, and sending

This lab accompanies the translations of `send.java`, `txnaddamount.java`, and
`txncoinlock.java`. It is written before their first native comparison run.
Results below must be updated from actual reports; a planned test is not evidence.

## The problem

A wallet balance is a collection of unspent coins. To pay 8 units, a wallet must
choose whole input coins and create recipient and change outputs. We reuse the
ported `MiniNumber`, `Coin`, `Transaction`, `Witness`, `Wallet`, `TxPoWSearcher`,
`TxPoWGenerator`, `txnutils`, `vault`, and `keys` implementations. Proofs and
signatures come from those real implementations.

## Trace the selector before improving it

The Java code first sorts the supplied list by decreasing amount, then returns a
second list grouped by address. For A:10, B:9, A:1 the sorted input remains
A:10, B:9, A:1, while the returned list is A:10, A:1, B:9. For a payment of 8,
the selector remembers A:10 and stops when it sees A:1. It never checks B:9.
The C++ translation must preserve this source behavior. Sorting equal amounts
must also retain their original order.

Mining and mempool reservations are skipped while looking for one sufficient
coin. If none is found, the source returns the entire grouped list. `send`
filters reservations again; `txnaddamount` does not. Tests must observe both
callers instead of assuming the selector always returns spendable coins.

## Mutation is part of the result

`txnaddamount` adds inputs and records locks before parsing the output address,
store-state flag, and split count. A later exception can therefore leave a changed
transaction and locked coins. Unlocking clears the list. Clearing the list alone
does not disable locking. We compare state after every operation, including
failures, using the existing wallet command harness and recording SQL adapter.

## Sending has more stages

`send` filters coin age, attaches MMR and script proofs, scales token amounts,
constructs outputs and state, signs, and submits work to the miner. A dry run
skips main-transaction signing and posting, but a custom-token burn still calls
the source burn helper with signing enabled. Password failure paths are checked
in their source order; there is no invented universal rollback or relock.

The existing Java fixture provides serialized coins and a real short chain.
The native side reads the same bytes. We compare replies, transaction bytes,
SQL calls, logs, locks, miner queues, and signature verification. Synthetic rows
and a dormant Main/miner receiver do not demonstrate a complete native node or
durable H2 replacement.

## Planned experiments

1. Empty lists, equal amounts, address grouping, list mutation, and aliasing.
2. Single-coin success, fallback, and mining/mempool reservations.
3. Funding with exact value, change, burns, splits, custom tokens, and failures.
4. Repeated lock/fund/unlock sequences and changes retained after exceptions.
5. Send metadata, parameter validation, proof construction, dry runs, signatures,
   and submission, comparing Java and native observations.

## Evidence

Both final builds pass 736 programs, 3,418 operations and 39,016 recording SQL calls. The normal run took 47.76 seconds and ASAN/UBSAN took 229.24 seconds. Both reports were inspected. These are suite-wide counts. Retained failures:

- The test used `bad` as an invalid address. Both languages accepted these valid
  hexadecimal digits; the failure experiment now uses `nothex`.
- The native generator omitted the source's mempool work/size rejection logs.
  The existing MinimaLogger now records them in the source branches. Logging the
  rejected ID also exposed independent fixture PRNG/time values; the synthetic
  mempool TxPoW now receives fixed values before either runtime hashes it.
- A non-array `multi` parameter produced `bad_variant_access` instead of Java's
  ClassCastException. The shared Command helper now follows its existing typed
  JSONObject-cast pattern for JSONArray as well.

The expanded corpus includes custom-token chains, non-simple token scripts,
actual token burn signatures during dry runs, password locks, SQL failures and
an oversized mempool transaction. These cases pass in both builds. Seven shared-component checks also pass in both builds after regenerating the map for the two new original-test files. The first sanitizer map check was stale; its corrected rerun passed.
