# Wallet — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java) · [Native body](../../../../../../../src/org/minima/database/wallet/Wallet.cpp)

## Purpose

Wallet owns seed, key and script SQL records, caches membership for fast searches and manages stateful TreeKey signing. Its SQL coordination and cryptography are separate from the storage engine.

## Follow the logic

1. createSQL creates keys, scripts and seed tables, prepares exact statements, rebuilds public-key/tracked/simple-address caches, loads or creates the base seed and ensures the base multisig script exists. TEST_PARAMS changes the static default-key target to eight; a later normal constructor does not reset it.
2. initBaseSeed returns a fresh SeedRow from an existing row. Otherwise it generates words, selects an optional configured phrase (cleaned unless any-seed), clears the configured phrase before insertion, derives the seed and inserts it. updateSeedRow replaces the in-memory SeedRow only after SQL update succeeds; callers retaining an older row keep that old object.
3. wipeBaseSeedRow wipes all private-key fields, writes an empty phrase/0x00 seed, replaces the cached seed, compacts/closes the database and reopens it. resetBaseSeedPrivKeys first updates the base seed, then derives each private key as hash(serialized seed, serialized modifier) and updates that row. It catches SQL exceptions and returns false without rolling back earlier updates.
4. createNewKey requires an available seed, uses cached unique-key count as its BigInteger modifier, derives a default TreeKey and inserts its parameters. Only after SQL execution does it add the public key to the cache. The returned KeyRow has modifier 0x00 even when the SQL insert stored a nonzero modifier; the two observations deliberately differ.
5. createNewSimpleAddress builds RETURN SIGNEDBY(publickey) from a new key, then adds a tracked simple script. addScript hashes the script into an address and inserts flags, adding tracked/simple cache membership only after execution. removeScript deletes the SQL row but never removes cache entries, so membership can remain true until caches are rebuilt.
6. initDefaultKeys returns true immediately when shutting down or new keys are stopped. Otherwise it creates at most the requested number toward the shared target and re-queries default addresses to determine completion. getDefaultAddress randomly picks from the returned list and fails for an empty list.
7. signData rejects a locked seed, loads the key, reconstructs TreeKey with stored size/depth/private seed and use count, signs, then updates the SQL use count before returning the signature. A SQL failure updating uses is caught and returns null after the signature computation; runtime failures escape. No explicit signature/public-key match check is added here.
8. Queries build fresh row/list objects and generally catch SQL exceptions, returning null or partial lists. Default-address listing catches all exceptions. Relevance checks use caches rather than querying SQL; all-key validation can therefore appear successful on an empty list returned after a query failure. Bulk use updates set or increment counts directly.

## Translation and dependencies

The native Wallet mirrors SQL text and statement order through SqlDBServices and abstract connection/prepared/result interfaces. shared_ptr preserves seed-row aliases; optional strings retain nullable fields, recursive locking matches synchronized re-entry, atomic flags avoid data races in exposed stop/shutdown state, and explicit 32-bit wrapping preserves arithmetic. It reuses the native BIP39, Address, Crypto and TreeKey implementations. Its membership sets are ordered internally, but the fixture compares membership after sorting. There is no native H2 engine behind the default boundary yet.

## Understanding the tests

The complete WalletOracle and native fixture call actual Wallet bodies while substituting only a recording SQL driver. Each operation returns value/error, base and held seed identities, cache contents, target, remaining phrase parameter, SQL events and logs. The driver sweeps checked/runtime failure positions for methods and schema load; null connection/statement/prepared/results; script flag combinations; locked/null/empty seeds; stopped/default-key gates; held-row mutation; closed/reopened connections; and signing uses 0, 3 and 4 with failures. It compares actual derived keys and serialized signatures, including new-key/simple-address paths. The recording driver returns supplied rows rather than executing SQL, so later queries are fixture rows, not proof of persisted inserts. Random selection is limited to zero/one candidate; generated phrase paths use a supplied fixture phrase.

- [Actual Java Wallet harness](../../../../../../../support/WalletOracle.java)
- [Native Wallet harness](../../../../../../../support/wallet.cpp)
- [Wallet case corpus and comparison](../../../../../../../tools/wallet_differential.py)
- [Recording JDBC boundary](../../../../../../../support/SqlDBOracle.java)
- [Native recording boundary](../../../../../../../support/SqlRecordingFixture.hpp)
- [Diagnostic normalization](../../../../../../../tools/sql_db_differential.py)
- [Native ownership and service API](../../../../../../../src/org/minima/database/wallet/Wallet.hpp)

No build or test was run while writing this lesson. The inspected driver defines the evidence scope; it does not certify full parity.

## Limits

No live wallet or credentials were accessed. Native persistence, crash-safe signing-use storage, concurrent lifecycle and multi-candidate random selection remain unverified. Java-specific enhanced exception expressions and logger prefixes/stacks are normalized. A matching SQL event log proves what was requested and in what order, not that a database durably committed it.

## Check your understanding

Why can removeScript make a SQL row disappear while isAddressRelevant still returns true, and which lifecycle step rebuilds the cache?

Pinned Java SHA-256: `dc4deb0584360af5330bd0fd3ef63ff24f8badb006a824bb35cd24327dda06b5`. [Mapping](../../../../../../../source-map.json) · [Project limitations](../../../../../../../REVIEW.md).
