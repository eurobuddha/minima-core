# Greeting — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/Greeting.java](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java) | [src/org/minima/objects/Greeting.cpp](../../../../../../src/org/minima/objects/Greeting.cpp) |

## What this file does

The initial peer message describing node version, extra connection metadata and the sender’s chain from tip toward root.

## Follow the logic

1. Initialize version from GlobalParams, empty extra JSON, top block zero and an empty chain. createGreeting mutates and returns the same object.
2. Acquire the database read lock before entering the try block. Add welcome, optional configured host and port, then inspect the chain tip. A missing tip sets top block to -1.
3. Walk tip to parent repeatedly and append each TxPoW ID. The method does not clear existing IDs first; calling it twice accumulates chains. Likewise omitted host or peer fields can retain prior JSON values.
4. If P2P is enabled, obtain the peer list and serialize addresses into extra JSON. Catch and log exceptions from inside the body, then unlock afterward. Lock/unlock calls themselves sit outside that catch.
5. getRootBlock returns -1 for top -1 or top above one trillion. Otherwise it computes top - (chain size - 1); an empty chain therefore gives top + 1 rather than being separately rejected.
6. Write version, extra JSON as MiniString text, top block, count and ordinary MiniData IDs. Reading parses the extra JSON: ParseException becomes a logged empty object, while a successfully parsed wrong type has different cast behavior.

## How the translation is approached

Native GreetingServices injects the database lock, welcome, tip and peers operations. It reproduces the ordering and repeated append behavior using the mapped tree nodes and address serializer. Missing callbacks fail on use; this boundary must be connected by the runtime. Optional JSON can represent parsed null, and getExtraData then fails, corresponding only approximately to Java null dereference behavior.

## What the tests tell us

The object fixture varies top block, chain length and welcome text, comparing wire bytes, root height and extra JSON. More directly, [NetworkStartupOracle](../../../../../../support/NetworkStartupOracle.java) and [native startup harness](../../../../../../support/network_startup.cpp) invoke createGreeting against controlled database/user/peer services. They start with retained JSON and a preexisting chain entry, repeat calls, and record call order, lock depth, appended IDs, fields and bytes. The [startup driver](../../../../../../tools/network_startup_differential.py) injects faults at lock, unlock, welcome, tip and peers; varies missing/empty/invalid peers and host/P2P flags; and includes 120 seeded sequences of 12 calls. This is a controlled service fixture, not a complete running node. Generic exceptions are normalized. The inspected existing startup report records 532 matching cases, including 1,764 createGreeting calls, across greeting and connect-command work.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWTreeNode](../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Main](../../../../../../../upstream-study/src/org/minima/system/Main.java), [P2PManager](../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java), [InetSocketAddressIO](../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java), [GeneralParams](../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [GlobalParams](../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [JSONParser](../../../../../../../upstream-study/src/org/minima/utils/json/parser/JSONParser.java), [ParseException](../../../../../../../upstream-study/src/org/minima/utils/json/parser/ParseException.java).

Method navigation (declarations with bodies found in this file): [Greeting](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L47), [createGreeting](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L52), [getExtraData](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L102), [getExtraDataValue](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L106), [setTopBlock](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L110), [getTopBlock](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L114), [getVersion](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L118), [getRootBlock](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L122), [getChain](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L147), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L152), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L168), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/Greeting.java#L188).

## Check your understanding

After two createGreeting calls on a chain of three nodes, starting with one retained ID, how many IDs are present? Explain why a later failure at peers can still leave the new top block and appended chain visible.

## Limits and traceability

The service harness deliberately bypasses full Java Main startup. Runtime integration, exact logging and all JSON cast/null exceptions remain separate verification work. The native getRootBlock also omits Java’s extra logging around a NumberFormatException during subtraction. Source-map status is partial and parity not_verified.

Pinned Java SHA-256: `79a81a0659fe6bacedfba8cf8955c1f326418ad59315543552eab838b0feb82e`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Nullable replies and references

The existing optional extra-data field can contain no object after reading JSON null. The new native getExtraDataOptional accessor exposes that state for callers that forward the reference into a JSON reply. ping's loopback test sends a valid Greeting whose extra-data text is null and asserts extradata:null. Existing reference-taking getExtraData overloads remain for callers that dereference the object; their null exception categories and all mutable aliasing boundaries still need further review. See the [nullable-reply lab](../../../../../new-tests/nullable-command-replies.md).
