# Coin and proof readers — what survives a failed read?

Read this after the [coin ownership lab](coin-ownership.md). Its central question is: **when input ends halfway through a record, which changes have already happened?** A parser is a sequence of observable operations, not just a function from valid bytes to values.

## Source and reused building blocks

The experiment calls the actual pinned Java `Coin`, `CoinProof`, `MMRProof`, `StateVariable`, `TxPoWSearcher` and `MinimaLogger`. The native version reuses their existing codecs, hash calculation, JSON rendering, shared handles and logger. See the [Java experiment](../../support/CoinStreamsOracle.java), [native experiment](../../support/coin_streams.cpp) and [comparison driver](../../tools/coin_streams_differential.py). Both reuse the existing object fixtures rather than independently inventing supposedly equivalent records.

The sibling port under `/Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/` was inspected. Its state type codes, coin ownership and proof layout differ from this source pin, so those implementations could not supply the required reader behavior.

## Three different mutation boundaries

| Reader | Operations before a later failure | What another owner observes |
|---|---|---|
| Coin | Assign each completed field; replace the state list before reading its count; append completed states; clear the token before reading its presence flag. | Earlier fields remain changed. A separately retained old state list remains alive but is no longer this coin's list. |
| CoinProof | Decode a whole coin, assign it, then decode and assign a whole proof. | Failure inside the coin leaves the old coin reference. Failure inside the proof leaves the newly assigned coin and the old proof. |
| MMRProof | Read block time, replace the chain, read its count, then append complete chunks. | The outer proof can contain a new time and only some of the requested chunks. |

For example, suppose a CoinProof contains coin A and proof P. Input contains a complete coin B followed by a truncated proof Q. After failure, the wrapper contains **B and P**. A local variable that retained A still refers to A. A second reference to the wrapper observes B. This distinguishes replacing a field from mutating the object that used to occupy it.

Native Coin, CoinProof and outer MMRProof handles retain shared field storage. Keep handles by value to preserve Java-like ownership across vector growth. A borrowed reference to a vector element still follows C++ invalidation rules. Individual proof chunks and nested value objects do not yet have complete Java identity semantics.

## Design the experiment before fixing the port

Start with one coin shared by a proof wrapper and witness. Keep another reference to the wrapper, its old coin, its old proof, and the coin's old state list. Give a second coin that same list through the owned native `getSharedState`/`setSharedState` bridge.

Java generates four coin fixtures, four wrapped proof fixtures and three MMR proofs. For each fixture, send **every prefix**, from zero bytes to the complete record, to both the in-place reader and convenience converter. Add trailing bytes, seeded single-bit changes and explicitly malformed hash/number lengths. Single-bit mutation keeps this corpus reproducible without accidentally asking the reference runtime to allocate enormous random arrays.

Compare the result or exact exception category/message, logger bodies, consumed input count, and eight retained objects plus the old state list. Then mutate retained owners and take another snapshot. Comparing only the reader's return value would miss almost all of the ownership observations.

The driver also tests copy, renamed copy and real floating-coin search with oversized IDs, null state lists, and a logger that throws either a runtime exception or a Java-style Error. The complete expanded corpus has **3,045 programs**. Counts describe programs; each program compares multiple observations.

## Null and exceptions are part of the contract

Coin and CoinProof convenience converters catch only I/O failures, call the actual logger and return null (an empty optional at the converter boundary). Other exception categories escape. MMRProof's converter propagates its I/O failure.

Coin's `deepCopy()` also logs an I/O failure and returns null. Its native return type remains a Coin handle, with `isNull()` distinguishing that result. Accessing a null handle throws the native Java-null-pointer exception. `getSameCoinWithCoinID()` then fails when trying to reset the absent copy. A search helper must translate an absent copied coin to an empty optional: an optional containing a null handle would falsely report success.

A logger is executable code. If logging itself throws, the caller never reaches its planned null return. The test injects failure into the first actual println callback and checks which message had already been emitted. It excludes timestamps, memory prefixes, stack frames and Java's detailed null-expression text; it does not erase the exception category or the ordinary error message.

## A test harness can be wrong too

An intermediate run found twelve differences. Four involved the missing invalid state type in the reader diagnostic: Java reports, for example, `Invalid StateVariable type : 5`. Eight involved objects with both an oversized ID and a null state list. Java's existing snapshot helper renders JSON before bytes, while the new native helper initially serialized first. Each operation failed for a different valid reason. Sequencing JSON first in the harness corrected the experiment without changing production behavior to suit a flawed test.

## Evidence checkpoint

The pre-reader baseline contains **2,716 mismatches among 3,029 programs**; its 313 passing programs did not establish reader fidelity. The retained report is `cpp/build/coin-streams-baseline-results.json`. The intermediate twelve-difference report is `cpp/build/coin-streams-reader-baseline-results.json`. Later floating-search programs expand the corpus, so baseline and final totals must not be treated as the same test set.

Both full builds pass. The expanded corpus passes all **3,045 programs** with zero differences in each build: normal CTest took **23.31 seconds**, sanitizer CTest **66.03 seconds**. The first normal affected-subsystem run finished 20/21 in 242.66 seconds. The search harness failed its one-JSON-reply-per-line check because actual converter logging now reached stdout; its Java oracle already redirects diagnostics to stderr. The native harness was adapted at its logger callback to match that transport convention. The dedicated stream experiment still compares actual logger bodies and failures. The normal search rerun passes all 189 programs and 6,468 queries in 9.58 seconds. The sanitizer broad run passes all 20 selected checks in 711.13 seconds, including search. Its block-ownership check runs separately and passes **1,558 programs and 6,219 operations in 146.76 seconds**, within the unchanged 240-second limit. Both final full builds pass without compiler warning/error lines. Across this checkpoint, all **29 selected checks** pass in each build after the one normal search-harness correction; this is a subset of 141 registrations. All 36 broad comparison reports were inspected with zero differences. Final comparison reports use `coin-streams-results.json` in each build directory.

```sh
ctest --test-dir cpp/build -R '^coin_streams_differential$' --output-on-failure
ctest --test-dir cpp/build-sanitized -R '^coin_streams_differential$' --output-on-failure
```

## Limits and exercises

The legacy `setState(vector)` API still copies its argument. The explicit owned-list bridge establishes the sharing in this experiment; transaction-to-output state sharing through all existing callers is not thereby fixed. Retained token, primitive value object, proof-chunk and MMRData identity remain open. StateVariable's own in-place reader and invalid-type writer behavior remain incomplete. The original CoinTests counterpart, full native startup and H2/MVStore backend remain absent.

1. Why not restore the receiver after an exception? **Answer:** the pinned source does not; rollback would change observable behavior.
2. Why retain both a wrapper and its previous coin? **Answer:** one follows replacement of the wrapper's field, while the other still owns the old coin.
3. Why inspect consumed bytes after failure? **Answer:** a caller may continue using the stream; failure at the same conceptual field does not guarantee the same stream position.
4. Can matching malformed-input results certify consensus safety? **Answer:** no. They establish these reader observations; trusted-root validation and full node integration require other evidence.
