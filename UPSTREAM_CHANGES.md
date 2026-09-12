# minima-core — what we changed in our fork, and why

**For:** Spartacus Rex / Minima Core maintainers
**From:** eurobuddha — fork at https://github.com/eurobuddha/minima-core
**Date:** 2026-08-02

Three unrelated pieces of work, all driven by running mainnet nodes on Android phones:

1. **A security fix** — `Wallet.signData` is an unsynchronized read-modify-write of the key-use
   counter, which produces genuine Winternitz one-time-signature reuse. Confirmed in the wild.
2. **MegaMMR import on constrained heaps** — a 343 MB mainnet `mega.mmr` needed ~1.3 GB decoded and
   killed the process on a 512 MB phone heap, with no usable error.
3. **A correctness fix** — `Wallet.removeScript` deletes the DB row but never removes the address
   from the in-memory `mAllTrackedAddress` / `mAllSimpleAddress` caches, so re-registering a shared
   script with `newscript trackall:false` keeps adopting every stranger's coin until node restart.
   Confirmed in the wild (shared casino covenant). See Part 3.

Everything is self-contained; no consensus rules, transaction validation or network code is touched.
Take any or all of it — happy to raise PRs in whatever shape suits you.

Two sibling documents cover our Android-app fork (`minima-core-android`), which is separate:
`UPSTREAM_IPC_CHANGES.md` (broadcast-IPC additions) and `UPSTREAM_UI_CHANGES.md` (UI redesign + H2 fix).

---

## Scope: seven files

```
src/org/minima/database/wallet/Wallet.java              security + removeScript cache fix (Part 3)
src/org/minima/system/commands/send/sendsign.java       security
src/org/minima/objects/mmr/MMR.java                     heap
src/org/minima/objects/mmr/MegaMMR.java                 heap
src/org/minima/system/commands/backup/mmrsync/megammr.java   heap
src/org/minima/utils/MiniFile.java                      heap
src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java     txpow table indexes (Part 4)
```

A raw `git diff origin/main` also shows `docker/`, `rescue.java`, `Main.java`, `CommandRunner.java`,
`GlobalParams.java` and `build.gradle`. **Those are not ours** — they are upstream commits we have not
merged yet (we are behind 5), so they appear as differences from our side. None of our commits touch
them.

---

# Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write

*Commit `c400cf9`. This is the one that matters.*

## The defect

`src/org/minima/database/wallet/Wallet.java`

```java
793  public Signature signData(String zPublicKey, MiniData zData) {   // <-- was not synchronized
...
823      int uses = key.getUses();                    // READ
826      tk.setUses(uses);
829      Signature signature = tk.sign(zData);        // SIGN AT LEAF `uses`
832      uses = tk.getUses();
835      updateUses(uses, key.getPublicKey());        // WRITE
```

Minima signatures are stateful: each key is a 64-ary, 3-level hash-based tree and every signature must
consume the **next** leaf. This method is the read-modify-write that enforces that, and it was the
**only** non-`synchronized` mutator on the class:

```
303  public synchronized SeedRow getBaseSeed()
311  public synchronized void wipeBaseSeedRow()
497  public synchronized KeyRow createNewKey()
553  public synchronized ScriptRow addScript(...)
613  public synchronized void removeScript(...)
634  public synchronized ScriptRow getScriptFromAddress(...)
662  public synchronized boolean isKeyRelevant(...)
683  public synchronized ArrayList<KeyRow> getAllKeys()
711  public synchronized KeyRow getKeyFromPublic(...)
793  public              Signature signData(...)          <-- the exception
846  private             void updateUses(...)             <-- and its write half
```

Two callers both read `uses = N`, both sign leaf `N` over **different** data, both write `N + 1`. One
leaf, two messages — the private key for that leaf is recoverable. An increment is silently lost too.

The window is not tight: `tk.sign()` between the read and the write is thousands of SHA3 hashes, so it
is milliseconds wide.

`TreeKey.verify()` only walks the Merkle path to the root public key — it has no notion of index
monotonicity, so a reused leaf verifies and the network accepts it. Nothing detects this anywhere.

## Why it is reachable in ordinary use

- `HTTPServer.java:66` starts a **thread per RPC request**; `CommandRunner.runMultiCommand` takes no lock.
- MDS starts a **handler per MiniDapp socket** (`MDSFileHandler` via `MDSManager.getSocketHandler`).
- `SendPollManager` is an **independent thread** firing queued sends every 30 s.
- On Android the broadcast IPC receiver runs each command on its own thread, and every installed dapp
  shares one node.

Collisions also concentrate rather than spread, because `getDefaultAddress()` (`Wallet.java:465-477`)
draws **uniformly at random from only 64 keys** with no rotation and no least-used preference — `uses`
is never consulted:

```java
int rand = new Random().nextInt(numkeys);
return allkeys.get(rand);
```

By the birthday bound you expect a repeat within ~10 calls.

## Evidence

Confirmed on a live node: **7 of 64 default keys re-used**, witness-exact (one leaf, two distinct
on-chain transactions).

The obvious alternative — a resync with `keyuses` set too low — is **ruled out by the data**. The
busiest key had **58** on-chain signatures, while every bulk counter rewrite in the node sets far more:

| call | default |
|---|---|
| `vault action:resetkeys` (`vault.java:231`) | 100 |
| `restore` (`restore.java:177`) | +256 |
| `archive` / `megammrsync` / `mysql` resync | 1000 |

The 262,144 wraparound is four orders of magnitude away, and no restore or resync had been run.

Reproduction (`disclosure/repro/RaceProof.java`, standalone, no node required) — 8 concurrent signers
on one key, modelling lines 823-835 exactly:

```
UNsynchronized  threads=8  distinct leaves=1  final uses=1  REUSED LEAVES=1
    leaf 0 signed 8 DIFFERENT txns: [txn2, txn0, txn3, txn7, txn4, txn6, txn1, txn5]
synchronized    threads=8  distinct leaves=8  final uses=8  REUSED LEAVES=0
```

## The change

```java
public synchronized Signature signData(String zPublicKey, MiniData zData) {
...
private synchronized void updateUses(int zUses, String zPublicKey) throws SQLException {
```

Monitors are reentrant, so the `synchronized` helpers `signData` calls (`getKeyFromPublic`,
`isBaseSeedAvailable`) are unaffected. This also fixes the shared `PreparedStatement`s `SQL_GET_KEY` and
`SQL_UPDATE_KEY_USES` (`Wallet.java:152-154`), which are not thread-safe and had their bound parameters
clobbered by the same race.

## Also in this commit — `sendsign` burns a leaf per input coin

`src/org/minima/system/commands/send/sendsign.java`

`reqsigs.add(pubkey)` was never called, so the `if(!reqsigs.contains(pubkey))` guards at `:114` and
`:142` never fired. An N-input transaction from one address consumed **N leaves instead of 1** and
appended N duplicate signatures to the witness. `send.java:464-465` has the correct pattern.

One subtlety when fixing it: the burn transaction needs its **own** dedupe list. It has a different
transaction ID, so a key that signed the main transaction must still sign the burn — sharing one list
produces an unsigned burn.

## Related, not changed — your call

- **`TreeKey.sign()` wraps instead of refusing.** `TreeKey.java:100-106` logs
  `SERIOUS ERROR : MAX TREEKEYS USED`, sets `mUses = 0` and signs anyway — guaranteeing reuse of every
  leaf from 0 upward, which `Wallet.signData` then persists to disk. We think this should throw, but
  refusing to sign is a behavioural decision that is yours.
- **`getaddress` ignores `uses`.** A key at 262,143 uses is as likely to be handed out as one at 0.
  Least-used selection would spread load, defer exhaustion, and make the race above meaningfully rarer.
- **`keys action:list` publishes a misleading aggregate.** Its top-level `maxuses` is the max `uses`
  across **all** keys (`SQL_GET_ALL_KEYS = "SELECT * FROM keys"`, no filter), including non-default
  `newaddress` keys — and the `keys` table has no default flag to separate them. Since
  `updateAllKeyUses` is `UPDATE keys SET uses=?` with no `WHERE`, feeding that aggregate into a resync
  sets **all** 64 default keys to a value inflated by an unrelated key.
- **No way to set one key's `uses`.** `keys` offers `list / genkey / checkkeys / new / createallkeys`
  only. A per-key setter (never-lower: `SET uses = MAX(uses, N)`) would let tooling apply a computed
  safe value instead of the all-or-nothing bulk rewrite. We worked around it by burning leaves via
  repeated `sign`, which works but is slow.

---

# Part 2 — MegaMMR import on constrained heaps

*Commits `1770d8a`, `a9ef09d`, `49feed4`, `33c438d`.*

Importing a mainnet `mega.mmr` on Android crashed the process "before it unpacks", with no error. Four
distinct causes, found in order by fixing each and re-measuring on a real device.

## 2a. An `OutOfMemoryError` is an `Error`, not an `Exception` — `1770d8a`

The `catch(Exception)` at the import site let an OOM kill the whole node. Also: a truncated `mega.mmr`
imported a **silent partial object graph**.

- Pre-flight heap check with the actual numbers, before minutes of loading
- `catch(Throwable)` around the load and the import pipeline, so an OOM reports instead of killing
- `MiniFile.loadObjectSlowStrict` propagates `IOException` — a truncated file now fails loudly
- `MegaMMR.pruneUnspendable` removes in place instead of copying the entire unspent-coins `Hashtable`
  (which doubled the largest structure at the worst possible moment); `readDataStream` pre-sizes the
  table and logs progress every 250k coins

## 2b. Stream the IBD in batches — `a9ef09d`

A 343 MB mainnet `mega.mmr` needed ~1.3 GB decoded as a whole graph. The import now reads sequentially
(version → MegaMMR → cascade → per-`TxBlock`) and feeds `TxPoWProcessor` batched `postProcessIBD`
calls, so peak heap is MegaMMR + cascade + **one 256-block batch** rather than the entire `TxBlock`
list.

Requirements honoured (documented inline): cascade only in batch #1, `restore=true` on every call,
per-batch `isIBDProcessFinished` poll, and `resetFirstIBDTimer()` to defuse the 5-minute / 3-hour
"chain tip up to date" gate that otherwise silently drops later batches.

## 2c. Heap exhaustion is process-global — `49feed4`

Field data from a Samsung device (512 MB growth limit): the import filled the heap and an **unrelated
timer thread** OOM'd first on a 24-byte allocation, killing the process — so the import's own
`catch(Throwable)` never saw it.

- **Heap watermark** in the coin-read loop (every 64k coins) and per IBD batch: below
  `max(32MB, 5% of heap)` free, abort cleanly while *other* threads still have runway
- **`-megaprune` at read time**: `isPrunable` is a pure per-coin test, so prunable coins are dropped
  before entering the table. Upstream loaded **all** coins and pruned afterwards, allocating the full
  unpruned set at the worst moment. MMR entries are still marked unspendable exactly as
  `scanUnspendable` would.

`sqlcoindb` was investigated and ruled out: it swaps the `TxPoWTree` per-node coin lists only. The
MegaMMR table has no disk-backed option, and a running `-megammr` node holds the same full table at
steady state.

## 2d. The MMR tree is the real ceiling — `33c438d`

Measured against a real mainnet `megammr.mmr` (343 MB file): the **MMR tree** is 1,513,769 entries
(~1.0 GB in RAM), loaded *before* the coin table (429k coins, ~130 MB) — alone, twice a 512 MB phone
heap. The coin-phase watermark never got a chance to fire.

Same watermark applied every 64k entries in `MMR.java`. Verified at `-Xmx512m` against the mainnet
file: aborts at 1.18M/1.51M entries with 10.9 MB free, **process alive**.

**This settles an architecture question:** mainnet MegaMMR needs ~1.2 GB heap minimum. No coin-store
change (SQL or otherwise) fits it in 512 MB — the MMR tree itself would need disk-backing. On a 512 MB
phone the practical mainnet path is `-megammr` **plus** `-megaprune`.

---

# Part 3 — `Wallet.removeScript` leaves the relevance caches stale

## The defect

`Wallet` keeps two fast-check caches beside the SQL scripts table: `mAllTrackedAddress` (feeds
`isAddressRelevant`, i.e. `TxPoWTreeNode.checkRelevant`'s address test) and `mAllSimpleAddress`.
They are populated on load and in `addScript(...)` — but `removeScript(zAddress)` only executes the
SQL delete and never touches either cache.

`newscript` re-registration is `removeScript` + `addScript(..., track)`, and `addScript` only ever
ADDS to the caches (when `zTrack`/`zSimple` are true). So demoting a script from `trackall:true` to
`trackall:false` updates the DB but leaves the address in `mAllTrackedAddress` until the node
restarts — `checkRelevant` keeps returning true for EVERY coin at that address, and the wallet keeps
adopting other people's coins into its relevant set (balance `locked`, `coins relevant:true`, and
`history`) block after block. The same staleness applies to `removescript` proper.

Where this bites in practice: any app built on a SHARED covenant address (one script, many users —
our casino, an order book, a liquidity pool) that ever registered it `trackall:true` and then
corrects to `trackall:false`. The correction silently does not take effect for the running node; a
polluted wallet keeps growing. `cointrack enable:false` per coin cannot keep up because relevance is
re-checked only when a block first processes a coin.

## The change

`removeScript` additionally removes the address from both caches, so they always mirror the DB:

```java
mAllTrackedAddress.remove(zAddress);
mAllSimpleAddress.remove(zAddress);
```

A subsequent `addScript` re-adds whatever the new flags say, so `newscript trackall:true` behavior
is unchanged. Two lines; no consensus, transaction or network code touched.

## Verification

**Security fix** — the standalone race harness above; plus the fix has been running in our Android node
build (`minimaCore 1.6.7-ui-h2`) with no ill effects.

**removeScript cache fix** — `newscript trackall:true` a shared script, then re-register it
`trackall:false`, then process a block carrying a foreign coin at that address: without the fix the
coin is adopted (relevant) until restart; with it, it is not. Existing behavior for `trackall:true`
registrations is unchanged.

**MegaMMR work** — desktop `javac -source/-target 8` build with 405-class parity; happy-path import OK;
truncated file → clean `EOFException`; 200 MB file on `-Xmx256m` → clean "too large" pre-flight with the
node alive; multi-batch verified with a `BATCH_SIZE=1` test build (6 blocks / 6 batches, cascade attach
and tip continuation, no "NO PARENT"), imported chain intact after restart. On-device measurements are
quoted per-commit above.

# Part 4 — The `txpow` table has no index on the columns it is queried by

**Commit `f7f0e08` · branch `MinimaCoreC++` · shipped in minimaCore Android 1.6.17-ui-h2**

## The defect

`TxPoWSqlDB.createSQL()` creates one index, `fastsearch ( txpowid, parentid )`. Five hot
statements sort or filter on `timemilli` and `isrelevant`, and neither is indexed:

```sql
SELECT * FROM txpow ORDER BY timemilli DESC LIMIT ? OFFSET ?          -- getLatestTxPoW
SELECT * FROM txpow WHERE isrelevant=1 ORDER BY timemilli DESC ...    -- getAllRelevant
SELECT Count(*) AS tot FROM txpow WHERE isrelevant=1
SELECT Count(*) AS tot FROM txpow WHERE timemilli > ?
DELETE FROM txpow WHERE timemilli < ? AND isrelevant=0                -- the 12-hourly clean
```

Each is a full table scan plus sort, and `SELECT *` drags every row's `txpowdata` BLOB through
that sort. Every `TxPoWSqlDB` method is `synchronized` on the instance, so one slow call stalls
every other reader of the DB.

## Evidence

Measured live on a Galaxy S10+ running a 500 MB+ txpow store, 2026-09-11. All **1,626**
`Long monitor contention` mentions in a two-hour window named `TxPoWSqlDB`; by method name
(these jars carry no `LineNumberTable`, so the log's own method names are the only reliable
attribution) `getLatestTxPoW` held the monitor **506** times, blocking `exists` (299) and
`getTxPoW` (210) — the calls the chain processor needs every block. Longest hold **47.171 s**.

`getLatestTxPoW` has exactly one caller: the `history` command. On Android the thread holding
the lock was the app's IPC command executor, which is single-threaded, so one `history` call
from any companion app stalls the node and every other companion at once.

## The change

Two indexes, created by the existing `CREATE INDEX IF NOT EXISTS` in `createSQL()`, so existing
databases index themselves on the next open — no migration, no schema version, one slow first
start on a large store:

```sql
CREATE INDEX IF NOT EXISTS txpowtime     ON txpow ( timemilli )
CREATE INDEX IF NOT EXISTS txpowrelevant ON txpow ( isrelevant, timemilli )
```

Plus a deliberate `WHERE timemilli > 0` on `SQL_SELECT_TOPTXPOW`. **The indexes alone change
nothing**: H2 will not use an index for a bare `ORDER BY … LIMIT` with no `WHERE`, so without
that predicate the query stays a full table scan even with `txpowtime` present. The first cut of
this patch was indexes only; it verified cleanly through jar and dex and would have shipped a fix
that did nothing for the hot path.

## Verification

`EXPLAIN ANALYZE` against **H2 2.1.214** — the version the Android APK links — on a scratch
database of 60,000 rows with 2 KB BLOBs, `ANALYZE`d:

| | plan | page reads |
|---|---|---|
| index only, query unchanged | `tableScan` | 25,535 |
| index + `WHERE timemilli > 0` | index | **3,755** |

`getAllRelevant`, `getRelevantSize` and the cleanup `DELETE` all move off `tableScan` as well.
The gap widens with the table: the scan is O(rows), the index path O(log n + limit).

Wall-clock timing on a warm SSD showed 85 ms vs 88 ms — no signal, and it points the wrong way.
Measure work done, not elapsed time.

## Honest caveat

This makes a bloated table survivable; it does not un-bloat it, and it has **not** been shown to
fix a live fault. Both affected devices were fixed by `megammrsync action:resync`, one of them on
an unpatched build. The underlying cause is Part 3 plus the fact that
`DELETE FROM txpow WHERE timemilli < ? AND isrelevant=0` never removes rows marked relevant, so
a polluted relevant set is permanent. That second half is still unfixed.

---

## Branches

| branch | contents |
|---|---|
| `security/signdata-race` | the security fix alone, clean off `origin/main` — 2 files, ready to PR |
| `feat/megammr-import-robustness` | the MegaMMR work + the security fix |
| `main` | the MegaMMR work |

We are 5 commits behind `origin/main` (docker + `rescue.java` additions) and have not merged them.
