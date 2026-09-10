# TxPoW — logic, translation and tests

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/TxPoW.java](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java) | [src/org/minima/objects/TxPoW.cpp](../../../../../../src/org/minima/objects/TxPoW.cpp) |

## Teacher's worked explanation

TxPoW packages the information used to represent transactions and blocks. Its
header contains information needed for hashing and chain structure. Its body
contains transactions, witnesses, and the list of transaction IDs included in a
block. The body can be discarded while the header remains. That makes the
difference between “a TxPoW object exists” and “its body exists” essential.

### Trace a normal object

The ordinary constructor creates both header and body. Setters fill their fields.
`setHeaderBodyHash()` hashes the body into the header, and `calculateTXPOWID()`
then hashes the header. The latter method computes several cached properties:

1. The TxPoW ID, in bytes and hexadecimal text.
2. Whether that ID lies below the block difficulty target.
3. Block weight: zero for a non-block, otherwise the maximum hash value divided
   by the difficulty, using Java's DECIMAL32 precision.
4. Main and burn transaction IDs when a body exists, plus the transaction-PoW flag.
5. A superblock level from the bit length of `difficulty / TxPoWID`, capped by
   the configured number of cascade levels.
6. The serialized byte size.

The C++ implementation reuses Crypto, MiniData, header/body serialization and
MMREntryNumber's decimal division helper. It must not replace the decimal weight
calculation with an integer division simply because the surrounding values are
hash-sized integers.

An easy naming trap: `isTransaction()` checks for nonempty main or burn
transactions when a body exists. The cached transaction-PoW flag is a different
property and appears in JSON. Read the method body rather than inferring behavior
from the name.

### Header-only objects and cached state

`clearBody()` removes the body. `hasBody()` becomes false, `isTransaction()` becomes
false, and methods listing block transactions return an empty list. However,
methods such as `getWitness()` still dereference the absent body in Java. Returning
an empty witness would silently invent a different behavior.

The native body uses `std::optional<TxBody>`. Selected getters now explicitly
raise a typed native null-pointer exception instead of exposing C++'s
`bad_optional_access`. The failure point matters: earlier validation stages may
reject before a body field is needed.

The serialized layout always writes the header, then a one-byte body-presence
flag, followed by the body only when present. `deepCopy()` goes through bytes and
reconstructs derived fields; it is not merely a shallow copy of cached members.
The source also has a special test constructor with supplied ID, height, weight,
parent and flags. This constructor supports synthetic tree tests and is distinct
from a real serialized/mined transaction.

### How the tests support the translation

`object_differential.py` varies nonce, difficulty, body mode and header fields.
For TxPoW cases it compares wire bytes, JSON where supported, IDs, block/transaction
flags, superlevel, byte size and decimal weight representation. It also creates
read-back and truncated-input cases. The complete object suite currently passes
9,244 normal-build comparisons; this number includes other object classes.

`consensus_differential.py` places source-generated TxPoW objects into checker and
generator scenarios. Its 2,890 passing normal-build comparisons include other
consensus functions, so they are integration evidence rather than a count of
TxPoW methods tested individually.

The newer `checker_logs_differential.py` includes a header-only fixture. It found
that Java's exception category differed from the old native optional-access
exception when simple or MMR checking reached body getters. Those paths now match
in the expanded 1,343-case checker suite.

### What remains unfinished

The current native API offers static deserialization but does not yet reproduce
Java's complete in-place `readDataStream` behavior on an existing TxPoW object.
That distinction matters for retained references and failed reads. Several body
setters still expose native optional-access behavior, and deep-copy
error logging is not fully equivalent. The default conversion helper now logs
caught IOException through MinimaLogger/NativeThrowable before returning null;
the mining-command corpus compares this path with Java. Cached values should not be assumed to
recompute after arbitrary mutations unless a source method explicitly does so.

**Exercise:** clear the body after calculating an ID. Predict the results of
`hasBody()`, `isTransaction()`, `getBlockTransactions()`, and `getWitness()`
separately. Which methods return values, and which operation fails?


## Open the test evidence

- [object_differential.py](../../../../../../tools/object_differential.py)
- [ObjectOracle.java](../../../../../../support/ObjectOracle.java)
- [objects.cpp](../../../../../../support/objects.cpp)
- [consensus_differential.py](../../../../../../tools/consensus_differential.py)
- [checker_logs_differential.py](../../../../../../tools/checker_logs_differential.py)

## Dependencies and reading map

Direct Minima imports: [MiniByte](../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [GlobalParams](../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [Crypto](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [TxPoW](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L77), [TxPoW](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L81), [addTestTransaction](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L94), [getTransactions](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L98), [TxPoW](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L120), [getTxHeader](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L125), [getTxHeaderBodyHash](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L129), [setHeaderBodyHash](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L133), [isMonotonic](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L137), [getTxBody](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L148), [hasBody](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L152), [clearBody](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L156), [setNonce](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L160), [getNonce](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L164), [getChainID](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L168), [getMagic](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L172), [setMagic](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L176), [setTxDifficulty](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L180), [getTxnDifficulty](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L184), [getTransaction](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L188), [getBurnTransaction](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L192), [setTransaction](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L196), [setWitness](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L200), [setBurnTransaction](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L204), [setBurnWitness](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L208), [getWitness](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L212), [getBurnWitness](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L216), [addBlockTxPOW](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L220), [getBlockTransactions](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L224), [getBlockDifficulty](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L233), [setBlockDifficulty](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L237), [getParentID](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L241), [setSuperParent](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L249), [getSuperParent](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L253), [getBurn](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L257), [setTimeMilli](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L261), [setTimeMilli](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L265), [getTimeMilli](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L269), [setBlockNumber](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L273), [getBlockNumber](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L277), [getMMRRoot](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L285), [setMMRRoot](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L289), [getMMRTotal](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L293), [setMMRTotal](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L297), [getCustomHash](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L301), [setCustomHash](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L305), [getCheckRejectNumber](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L309), [incrementCheckRejectNumber](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L313), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L317), [toString](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L350), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L355), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L369), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L382), [deepCopy](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L392), [convertMiniDataVersion](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L424), [calculateTransactionID](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L447), [getTxPoWID](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L455), [getTxPoWIDData](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L459), [getSuperLevel](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L463), [isBlock](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L467), [getWeight](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L475), [isTransaction](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L479), [getSizeinBytes](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L491), [getSizeinBytesWithoutBlockTxns](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L495), [calculateTXPOWID](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L505), [getSuperLevel](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java#L568).


Pinned Java SHA-256: `46e3867c753cd9dde3814bb01505426e2978c2b97bafbbf3fad7c0a99558c456`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Mining-command integration

The mining/posting suite compares full serialized TxPoW objects, retained row owners and message side effects after fixing only the generated PRNG bytes at miner entry. Truncated and bodyless payloads exercise conversion and failure ordering. This does not supply a complete in-place TxPoW reader or full Main runtime.
