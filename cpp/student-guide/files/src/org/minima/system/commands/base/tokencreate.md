# tokencreate — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/tokencreate.java) · `src/org/minima/system/commands/base/tokencreate.cpp` (absent)

## Purpose

tokencreate builds and signs a transaction that colors a quantity of Minima into a new token, optionally signs its creator metadata and mines the result.

## Follow the command

1. Require name and amount. Accept a name JSONObject only if it contains name, otherwise wrap a string. Read state, mine and uselimits; floor token supply and require at least one. Optional limits reject supply above one trillion or decimals above sixteen; there is no explicit nonnegative-decimal check.
2. Compute colored Minima as MINI_UNIT × 10^decimals × floored supply, scale as MAX_DECIMAL_PLACES minus decimals, and required inputs as colored amount plus burn. Get a default address and mature relevant simple Minima coins.
3. Skip coins already being mined or in the mempool; accumulate in search order until enough. No eligible coins throws, while insufficient selected amount returns status:false and a message. Add inputs and choose an MMR proof block based on the newest input, clamped by proof history.
4. For every input add a CoinProof and its wallet ScriptProof, rejecting a missing script. Deduplicate required signing public keys. Create the first output with TOKENID_CREATE, colored amount and storeState true.
5. Add optional webvalidate metadata. For signtoken, derive the first output coin ID from the first input, sign that ID and insert signature fields into the same name object. Construct token metadata, add the token output and optional Minima change with storeState false.
6. Parse state ports/values, precompute coin IDs and calculate transaction ID. Sign once per required public key, generate TxPoW and calculate its ID. Mine synchronously with a 120-second limit or queue asynchronous mining, then return TxPoW JSON.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse Token, CoinProof, ScriptProof, transaction ID construction, Wallet signing and TxPoWGenerator/miner. Exact decimal arithmetic, state serialization and signing order determine the resulting IDs; they cannot be approximated.

## Understanding the tests

The runner fixture checks tokencreate’s permission classification only. Proposed direct cases use deterministic coins/keys to assert colored amount, flooring, limits disabled, confirmation boundary, pending exclusions, change, proof-history clamp, missing scripts and deduplicated signatures. Check mutable name metadata and signing-use state after late state/mining failures; a failure can occur after signatures were consumed.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

Async return means mining was queued, not confirmed on-chain. This body has no transaction-wide rollback and uses the caller’s name JSONObject directly. No token was minted while writing the lesson.

## Check your understanding

Why must the optional token-creator signature be over the future first output coin ID rather than the transaction ID?

Pinned Java SHA-256: `67582fbebbcc7afd76ea170deedc56ce910e34f171022274169df43c3e75ff43`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
