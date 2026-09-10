# TxHeader — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/TxHeader.java](../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java) | [src/org/minima/objects/TxHeader.cpp](../../../../../../src/org/minima/objects/TxHeader.cpp) |

## What this file does

The durable metadata of a TxPoW: chain identity, time, block number, proof-of-work fields, ancestor links, MMR commitment and body hash.

## Follow the logic

1. Initialize the nonce/block number to zero, chain ID to MAIN_NET, time to the current clock, block difficulty to the maximal hash, default Magic and empty superparent hashes for every configured cascade level.
2. Compress adjacent equal superparent IDs with run-length encoding. Write a one-byte count and one hash per run; the configured cascade-level count determines the total number to reconstruct, so no separate run count is written.
3. The binary sequence is nonce, chain ID, time, block number, block difficulty, compressed superparents, MMR root/total, Magic, custom hash and body hash. Chain ID and difficulty use ordinary MiniData encoding; the commitment hashes use the hash-specific size limit.
4. The reader expands each run into the existing superparent array until all levels are filled. A zero run consumes count/hash bytes without advancing the filled count. An oversized run reaches array bounds after partially writing Java entries.
5. JSON presents the same runs with their final difficulty index and count. It also renders date from time.getAsLong using the environment’s local date/time formatting.
6. The convenience MiniData converter catches IOException, logs and returns null. A header does not recalculate or authenticate its stored body hash; TxPoW and checking code supply that behavior.

## How the translation is approached

Native code uses a vector sized at construction from GlobalParams and matching run grouping. It checks run overflow before expansion, which changes partial-mutation timing compared with Java. Date rendering now reuses the existing Java-compatible date helper. A four-zone command experiment found that the old independent localtime_r/strftime formatter printed the wrong timezone label for January 1970 in London; the shared helper already matched Java. Native conversion catches all std::exception, wider than Java IOException.

## What the tests tell us

The header fixture crosses timestamps 0, -1, 99999999, 1788883200000 and 2147483648000 with superparent run parameters 0, 1, 2, 7 and 32. It fixes nonce/block number/body hash and compares JSON plus wire bytes. Both processes run with TZ=UTC, so passing date strings are evidence for that timezone and selected timestamps, not every locale or calendar date. The [Java object oracle](../../../../../../support/ObjectOracle.java), [native harness](../../../../../../support/objects.cpp) and [object driver](../../../../../../tools/object_differential.py) compare explicit observations and Java-generated wire round trips, trailing bytes and truncations. The inspected existing full-object result has 9,464 matches; that is not a per-file assertion count. Generic ERROR comparisons leave exact exception parity open.

The [chain command lab](../../../../../new-tests/chain-commands.md) follows this formatter discrepancy, the retained failing observation and header/tree rendering at fractional and extreme timestamps in four zones. It does not certify every calendar/time-zone combination.

## Dependencies and reading map

Direct Minima imports: [MiniByte](../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [GlobalParams](../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [Crypto](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [TxHeader](../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java#L82), [getBodyHash](../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java#L92), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java#L96), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java#L158), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java#L212), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java#L244), [convertMiniDataVersion](../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java#L253).

## Check your understanding

Encode the conceptual ancestor list A,A,B,B,B as count/hash pairs. Why must changing GlobalParams.MINIMA_CASCADE_LEVELS after constructing an object be reviewed separately?

## Limits and traceability

Public Java reference fields and native value fields have different alias semantics. Malformed run counts, mutable globals, default-array references, date locale and converter logging still require full API review. The source map remains partial/not_verified.

Pinned Java SHA-256: `f8c85b692975c5f5fbdc26ec7a4f79e448ee757a39ed09177a550bd5a61da6a5`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
