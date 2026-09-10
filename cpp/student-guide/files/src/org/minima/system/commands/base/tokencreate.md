# tokencreate — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/tokencreate.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/tokencreate.cpp)

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

The native command uses existing Command helpers, JavaInteger parsing, exact
MiniNumber arithmetic, Token/Coin/Transaction serialization, Wallet signing,
MMR proofs, TxPoWGenerator and TxPoWMiner. The flow retains source order: default
address lookup comes before chain checks, mining/mempool exclusions occur in
wallet search order, and proof construction precedes token metadata signatures.

Calling send or createtokenfrom would change behavior: send reorders selection
and has different validation; createtokenfrom composes temporary-row commands.
This source builds the wallet transaction directly. The name object remains
shared with its caller, so later failures do not undo inserted metadata.

## Understanding the tests

The [token-creation lab](../../../../../../../new-tests/token-creation.md) was
written before testing. Its Java-generated coin/MMR fixtures and actual keys
feed both implementations. Compare replies, full TxPoW bytes, database calls,
mining queues and mutations of a caller-supplied JSON name, including after errors.

Independent assertions calculate colored Minima, floored supply, token decimals,
change, input-output difference and output state flags. Separate cryptographic
checks verify transaction signatures and the metadata signature over the minted
coin ID. The initialized mining fixture fixes only the generated PRNG at entry;
its queue and synchronous mining bodies still run. Other cases inject explicit
mining failures, SQL faults, exhausted keys and missing script/history data.

An initial reservation fixture created a Java miner without a message queue,
which could test selection but failed at submission. The new cases reserve
coins by submitting through the already initialized mining fixture in both
runtimes. Keep this distinction visible when interpreting failures.

- [Comparison driver](../../../../../../../../tools/token_creation_differential.py)
- [Validation evidence](../../../../../../../../validation/token-creation-work-in-progress)
- [Shared parameter parsing](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both builds pass 335 programs / 1,349 operations. Consult the lab and saved
reports for the exact scope, independent assertions and regression evidence.

## Limits

Async return means mining was queued, not confirmed on-chain. This body has no transaction-wide rollback and uses the caller’s name JSONObject directly. Fixture transactions are constructed and signed locally; network acceptance and durable SQL remain unverified.

## Check your understanding

Why must the optional token-creator signature be over the future first output coin ID rather than the transaction ID?

Pinned Java SHA-256: `67582fbebbcc7afd76ea170deedc56ce910e34f171022274169df43c3e75ff43`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
