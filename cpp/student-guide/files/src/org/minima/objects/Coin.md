# Coin — identifying, storing and copying a spendable output

**Teaching status:** Authored lesson, based on the complete Java and native bodies and the existing object comparison fixtures.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/Coin.java](../../../../../../../upstream-study/src/org/minima/objects/Coin.java) | [src/org/minima/objects/Coin.cpp](../../../../../../src/org/minima/objects/Coin.cpp) |

## Node-state command comparison

The MinimaDB SQL bridge now reuses Coin’s existing private null handle, with friendship limited to MinimaDB. A failed SQL coin decode can therefore remain null in a returned list until a consumer reads it. Previously the bridge dereferenced every row while copying the list, so a tracking operation failed before Java’s second lookup. This is the same handle used by deepCopy on an I/O failure, not a new empty coin value. See [the lab](../../../../../new-tests/node-state-commands.md) for the failing examples and final comparison evidence.

## The problem this class solves

A transaction output needs more than an amount. It must say which script controls spending it, which token the amount belongs to, and which particular output it represents. `Coin` keeps these facts together with state variables and bookkeeping used by the MMR, Minima's authenticated collection of coin records.

The address stores a script hash; it does not contain or execute the script. The coin ID identifies an output. The source reserves `0x00` for an output whose final ID has not yet been assigned and `0x01` for a floating input. The transaction code calculates final output IDs; `Coin` only stores or replaces them. A constructor assigning fields is therefore not proof that an output is valid or spendable. The [transaction lesson](Transaction.md) and [checker lesson](../system/brains/TxPoWChecker.md) explain those later checks.

## Follow the state, then the methods

The main constructor assigns the ID, address, amount, token ID and `storestate` flag. Other fields begin with an empty state list, zero MMR entry number, unspent flag, zero creation height and no token descriptor. The shorter constructors delegate to this constructor, using the output ID or the default `storestate=true` as appropriate.

| Field group | Meaning | Important distinction |
|---|---|---|
| Coin ID and address | Output identity and controlling script hash | Changing an ID does not execute a script. |
| Amount and token ID | Stored quantity and token identity | A token descriptor supplies scaling; the raw amount remains stored. |
| State list and store flag | Script-visible data and an output's storage choice | Setting the flag false does not clear the list. |
| MMR entry, spent, created | Record position, spend status and creation block | Setters record facts; they do not verify a proof. |
| Optional token descriptor | Token name, scale and script metadata | `setToken` does not automatically replace `tokenid`. |

`getTokenAmount` returns the raw amount when there is no descriptor. Otherwise it asks the descriptor to scale the amount. This decision depends on descriptor presence, not a comparison of the token ID with Minima's ID. In the fixture, attaching a descriptor and resetting the token ID are deliberately two separate calls.

`checkForStateVariable` scans the list in order and returns at the first match. It compares each state's **data text**, not its port. With the wildcard flag false, it uses exact equality. With the flag true, it uses substring containment; there is no glob or regular-expression language. An empty list always gives false. An empty search string with wildcard matching succeeds when at least one ordinary state value is present.

## Two JSON views of the same state

`toJSON(false)` emits a state array, retaining each variable's port, type and data. `toJSON(true)` emits an object whose keys are decimal port numbers and whose values are data strings. Repeated ports remain separate entries in the array; in the object, a later value replaces an earlier value at the same key. `getStateAsJSON` and `convertStateListToJSON` use the latter rule.

Both views include IDs, raw amount, address, human-readable address, storage flag, spent flag, MMR entry and creation block. With no descriptor, JSON contains `"token": null` and omits `tokenamount`. With a descriptor, it includes both the token object and the scaled amount. Omitting a key is observably different from including a null key, so the comparison harness checks the actual output.

## Bytes are the copying contract

[Java's writer](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L352) and [native `serialise`](../../../../../../src/org/minima/objects/Coin.cpp) write this exact sequence:

1. Coin ID and address, each using the hash encoding.
2. Amount using `MiniNumber`, then token ID using the hash encoding.
3. One byte for the state-storage choice.
4. MMR entry number, spent byte and creation block number.
5. State count encoded as a `MiniNumber`, followed by every state variable in list order.
6. A token-presence byte and, only when present, the descriptor's bytes.

Notice that the writer includes the state list even when `storestate` is false. The flag expresses a choice used by callers; it is not an instruction for this writer to skip bytes. The reader consumes the same order, converts the state count to an integer and reads that many entries. It does not independently validate spending rules.

Java `deepCopy` writes the entire coin and reads a new coin from those bytes. The native method follows that same encode/decode route. This is stronger than copying a pointer: the successful result has reconstructed state and token data. `getSameCoinWithCoinID` first performs that deep copy and then replaces its ID, leaving the original ID unchanged. It is not a method for returning the original object with a mutation.

`convertMiniDataVersion` attempts one coin read from the supplied bytes. Neither body requires that every supplied byte be consumed. Java catches `IOException`, logs it, and returns null. The native converter catches `wire::IOException`, calls its error callback and returns an empty optional. Other exception categories escape. The default callback now calls the actual MinimaLogger. The deep-copy path also catches I/O errors, logs and returns a null Coin handle, detectable with `isNull()`. Logger failures escape.

## How the existing tests teach the translation

The Java fixture is [ObjectOracle.coin](../../../../../../support/ObjectOracle.java#L47). The native counterpart reuses [CoinFixture.hpp](../../../../../../support/CoinFixture.hpp#L5), which is also used by other harnesses. The [driver](../../../../../../tools/object_differential.py) constructs 32 coin cases, indexed 0 through 31:

- Each case has amount `0.125`, address `0x001122` and a distinct initial coin ID.
- Even indexes store state; odd indexes attach a token with scale 3 and then reset the token ID.
- Every third index is spent. Creation height is `index * 123`, and the MMR entry is `2^index`.
- State-list length is `index % 5`. Ports use `i % 3`, creating a duplicate port in four-element lists. Values alternate bracketed text containing a panda and a number initially written as `1.00`.

For each coin, the harness compares its serialized bytes, ordinary JSON, simple-state JSON, and the deep copy's bytes and ordinary JSON. The driver then takes the **Java-produced** bytes and asks both readers to decode the complete input, the same input with trailing `FFFF`, and five truncated prefixes: lengths 0, 1, 2, halfway, and one byte short. This checks shared wire rules against the source, rather than merely asking the native writer to agree with its own reader.

The latest completed normal and sanitizer runs each passed all **9,464 object-suite cases** with zero differences. That is the total across many classes, not 9,464 Coin-only cases. The 32 direct coin fixtures and their derived reads are part of that total. The harness normalizes failures to `ERROR`; it does not establish exact exception type/message parity for every malformed coin. The original direct deep-copy fixtures compare content. The 220 added ownership programs separately mutate the copy and original to test their independence.

To rerun this existing suite after a successful build:

```sh
ctest --test-dir cpp/build -R '^object_differential$' --output-on-failure
ctest --test-dir cpp/build-sanitized -R '^object_differential$' --output-on-failure
```

## Ownership experiment: share the coin, copy only when requested

An ordinary native `Coin` copy now retains a shared allocation containing its fields. A transaction input, output, and witness proof can therefore refer to the same coin, as in Java. Retaining a `Coin` handle by value also survives vector growth and removal; retaining a C++ reference to the vector element does not offer that guarantee.

The [coin ownership lab](../../../../../new-tests/coin-ownership.md) adds 220 mutation programs. It varies the owner through which a change occurs, grows and clears containers, rebinds a local handle, and checks that `deepCopy()`, `getSameCoinWithCoinID()` and serialized copies remain independent. The baseline failed all 220 while the previous 9,244 object cases passed. Both corrected object runs now pass all 9,464 cases. The lab records the 21 selected regressions, including the sanitizer timeout and successful isolated rerun. This changes ownership; the existing wire format, hashing, JSON and encode/decode copying algorithms are retained.

## What remains different or unproved

Java `setState` retains the caller's list reference. Native `getSharedState`/`setSharedState` now provide an owned bridge for that behavior, including a null list. The legacy vector setter still copies; not every transaction/output caller uses the bridge. Token and other object-reference sharing still need their own ownership tests.

The native in-place reader now assigns completed fields in source order, replaces the state list before its count and clears the token before its flag. The [coin and proof reader lab](../../../../../new-tests/coin-streams.md) explains every-prefix tests, retained owners, null copy results and actual logging failures. Null primitive/token field graphs, nested object identity and all Unicode substring edge cases remain unproved.

These are reasons the map still says `implemented_partial` and parity `not_verified`, despite passing finite suites. The complete native node and H2/MVStore backend also remain unfinished.

## Dependencies and reading map

Direct Minima imports: [MiniByte](../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MMREntryNumber](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Coin](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L94), [Coin](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L98), [Coin](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L102), [Coin](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L106), [Coin](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L114), [getSameCoinWithCoinID](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L119), [resetCoinID](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L128), [resetTokenID](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L132), [getToken](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L136), [setToken](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L140), [setMMREntryNumber](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L144), [getMMREntryNumber](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L148), [setSpent](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L152), [getSpent](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L156), [setBlockCreated](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L160), [getBlockCreated](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L164), [setStoreState](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L168), [storeState](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L172), [getCoinID](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L176), [getAddress](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L180), [getAmount](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L184), [getTokenAmount](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L188), [getTokenID](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L195), [getState](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L199), [setState](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L203), [checkForStateVariable](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L207), [checkForStateVariable](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L211), [toString](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L227), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L231), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L235), [getStateAsJSON](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L280), [convertStateListToJSON](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L288), [convertMiniDataVersion](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L299), [deepCopy](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L322), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L352), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L378), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/Coin.java#L403).

## Check your understanding

1. A coin has two state entries at port 7, first `[old]` and then `[new]`. Predict both JSON views. **Answer:** the array retains both entries; simple state contains only the later value for key `"7"`.
2. You set `storestate=false` on a coin that already has two state variables. How many does its writer emit? **Answer:** two; this writer does not filter them by the flag.
3. Why test bytes produced by Java as well as a native round trip? **Answer:** a native writer and reader could share the same incorrect field order and still round-trip successfully.
4. Does comparing a deep copy's JSON prove that changing its state cannot change the original? **Answer:** no. Content equality and mutation independence are separate observations; add a deliberate mutation experiment to test the latter.

## Traceability

Pinned Java SHA-256: `79a341889c63d2088faec31518c15c5a0c3273b99aed97ec72d75d3c5543567c`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Transaction-object Streamable integration

Coin now implements Streamable using its existing in-place reader. Its append writer emits each field directly, checks the state list only at its source position and delegates to StateVariable and Token writers. Oversized later hashes and null state lists leave precisely the preceding fields in the supplied output. Retained owner behavior continues to use the existing shared storage.

The [transaction-object interface lab](../../../../../new-tests/streamable-transaction-objects.md) explains the source-order tests and existing generic callers. Both complete builds pass. The expanded interface suite passes 2,771 comparisons and 20,914 assertions per build across 20 types, with matching transcripts and empty native stderr. All seven affected regressions also pass in both builds. Full nullable/reference/API and node parity remain unverified.
