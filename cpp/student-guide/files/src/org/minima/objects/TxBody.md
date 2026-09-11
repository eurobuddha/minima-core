# TxBody — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/TxBody.java](../../../../../../../upstream-study/src/org/minima/objects/TxBody.java) | [src/org/minima/objects/TxBody.cpp](../../../../../../src/org/minima/objects/TxBody.cpp) |

## What this file does

The transaction-bearing part of a TxPoW, containing normal and burn transactions, their witnesses, randomness, difficulty and referenced TxPoW IDs.

## Follow the logic

1. Initialize a fresh 32-byte random PRNG field and maximal transaction difficulty. Create independent empty normal/burn Transaction and Witness objects and an empty referenced-ID list.
2. resetRandomPRNG replaces only the random bytes. It does not update a containing TxPoW’s cached hashes automatically.
3. JSON emits prng, txndiff, txn, witness, burntxn, burnwitness and txnlist. List order is retained.
4. Serialize PRNG with hash-length limits, transaction difficulty with ordinary MiniData encoding, the two transaction/witness pairs, then a MiniNumber list count and hash-encoded referenced IDs. The two MiniData encoding choices have distinct size validation.
5. Java reading replaces PRNG and difficulty but calls readDataStream on the already existing transaction and witness objects. It replaces the referenced-ID list only after those nested reads complete, so failure ordering and retained references are observable.

## How the translation is approached

TxBody now implements Streamable and reuses Transaction/Witness's shared record pattern. All seven fields are nullable owned handles, including a replaceable list of nullable MiniData references. Reads replace the two scalar fields after complete decoding, invoke existing child readers while retaining each receiver, then replace the ID list before its count. Writers append nested children directly. Native public-field accessors use parentheses and support assignment to the shared record. TxPoW exposes owned body/list getters, so retained bodies survive clearBody. See the [ownership lab](../../../../../../student-guide/new-tests/streamable-txbody.md) for the translation, independent model and reentrant-reader experiment.

## What the tests tell us

The body fixture fixes PRNG bytes so random initialization does not make comparisons nondeterministic, then compares full JSON and bytes. Body-containing TxPoW and TxBlock fixtures exercise nested composition, and generated read cases test incomplete/trailing payloads. Deterministic fixtures do not establish random-source quality or retained nested-reference parity. See [Java object oracle](../../../../../../support/ObjectOracle.java), [native object harness](../../../../../../support/objects.cpp) and [object driver](../../../../../../tools/object_differential.py). The inspected existing full-object report has 9,244 successful comparisons; this is a corpus total, not a count of this file’s tests. Generic ERROR normalization leaves exact exception types unproven.

The [transaction-check lab](../../../../../../student-guide/new-tests/transaction-check.md) adds in-place body reads with retained field owners, aliases between main/burn fields, null fields, truncated data and repeated reads. It compares identity, consumed bytes, partial state and exception category. These tests are more specific than a fresh-object round trip.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Crypto](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [TxBody](../../../../../../../upstream-study/src/org/minima/objects/TxBody.java#L54), [resetRandomPRNG](../../../../../../../upstream-study/src/org/minima/objects/TxBody.java#L59), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/TxBody.java#L63), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/TxBody.java#L88), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/TxBody.java#L107).

## Check your understanding

Why must a test replace PRNG before comparing independently constructed Java and C++ bodies? Which hash in a containing TxPoW becomes stale if PRNG changes afterward?

## Limits and traceability

The file is a data container, not a proof-of-work or transaction verifier. The ownership lab tests nullable fields/entries, retained lists and bodies, partial readers, nested writer prefixes and selected reentrant/JSON behavior. Complete collection, subclass, custom-stream, allocation and concurrency behavior remains unproved. Source-map status is partial and parity not_verified.

Pinned Java SHA-256: `30288d222f98694cd3cad1b6ff1a99a1e8b5ae4fe358948307ae22567c7f012e`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
