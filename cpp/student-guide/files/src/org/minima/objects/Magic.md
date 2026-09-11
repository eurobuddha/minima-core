# Magic — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/Magic.java](../../../../../../../upstream-study/src/org/minima/objects/Magic.java) | [src/org/minima/objects/Magic.cpp](../../../../../../src/org/minima/objects/Magic.cpp) |

## What this file does

The current and desired capacity parameters carried by the chain. Changes are intentionally gradual rather than jumping directly to a miner’s requested values.

## Follow the logic

1. Initialize size, KISS operations and transactions per block to 65536, 1024 and 256. Initialize the work threshold to floor((2^256 - 1)/10000), encoded as MiniData.
2. For each capacity value, clamp the desired value to between half and twice the current value. Then apply its hard minimum. Order matters: the hard floor is applied after the relative clamp.
3. Calculate the next current value as (16383*current + clampedDesired)/16384 using MiniNumber arithmetic. This gives the desired value a weight of only 1/16384.
4. For the work threshold, perform analogous clamping and averaging with positive BigInteger arithmetic. Its hard bound is an upper bound on the numeric hash threshold, since a larger permitted hash threshold corresponds to easier work.
5. Create the result as a fresh default Magic, then assign only its current fields. Desired fields in the returned object therefore reset to defaults; they are not copied from the receiver.
6. checkSame compares only the four current fields. The three numeric capacities compare numerically; the MiniData work threshold compares exact bytes. Desired fields do not participate.
7. Serialize all current fields first and all desired fields second, preserving decimal scale and work bytes. Reading restores those values without applying calculateNewCurrent or validating them.

## How the translation is approached

The native counterpart reuses MiniNumber for decimal capacities and BigInt for the wide work threshold. A small shared weighted helper reproduces the three identical capacity branches. It retains the fresh-default result semantics and byte-sensitive work equality.

## What the tests tell us

The magic fixture starts from defaults, sets several desired triples including below-limit, doubled, very large and fractional requests, and crosses empty/zero/one/maximal work bytes. It compares both the original object and calculateNewCurrent output. This directly observes clamping, weighted results and returned defaults, but does not establish every nondefault-current state or checkSame branch. The [Java object oracle](../../../../../../support/ObjectOracle.java), [native harness](../../../../../../support/objects.cpp) and [object driver](../../../../../../tools/object_differential.py) compare explicit observations and Java-generated wire round trips, trailing bytes and truncations. The inspected existing full-object result has 9,244 matches; that is not a per-file assertion count. Generic ERROR comparisons leave exact exception parity open.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Crypto](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Magic](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L75), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L87), [checkSame](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L103), [getMaxTxPoWSize](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L115), [getMaxKISSOps](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L119), [getMaxNumTxns](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L123), [getMinTxPowWork](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L127), [setDesiredKISSVM](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L134), [setDesiredMaxTxPoWSize](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L138), [setDesiredMaxTxns](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L142), [calculateNewCurrent](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L153), [toString](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L236), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L241), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L254), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L266), [main](../../../../../../../upstream-study/src/org/minima/objects/Magic.java#L272).

## Check your understanding

If current KISS operations is 1024 and desired is 2048, what is the next current value? Which desired value will the returned Magic hold unless a caller explicitly changes it again?

## Limits and traceability

The main method is a printed demonstration, not an assertion suite. Consensus callers must decide when current values are checked and desired values refreshed; this container alone does not establish valid chain parameter updates. The source map remains partial/not_verified.

Pinned Java SHA-256: `d244cc2600842c2ca650aba572fa21fad7cccfbc9a0a525a78114559d6cf0bfe`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Streamable and ownership follow-up

The [Magic lab](../../../../../new-tests/streamable-magic.md) explains nullable
owned fields, shared default objects, retained aliases through the TxPoW setter,
source-ordered partial reads, eager comparisons and nested failure prefixes.
The native main now contains the complete source demonstration. Consult the lab's
evidence link for the actual build/test status; the earlier object-only counts
above describe historical coverage. Full class and node parity remain unverified.


## Separate hash-writer update

The [hash-writer lab](../../../../../new-tests/hash-writers.md) explains distinct ordinary/hash dispatch, mutable
length limits, partial output and receiver lifetime during field replacement.
Both builds pass 1,848 comparisons and 12,752 independent assertions for the
represented paths. This update does not establish full field identity or class parity.
