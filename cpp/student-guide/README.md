# Learning the Minima Java-to-C++ port

This teaching directory follows all **457 Java source files** in the pinned Minima repository. Each file has its own page under `files/`, mirroring the original package path. `src/` contains the 369 production files; `test/` contains the 88 original test files. Identical short names in different packages have separate pages.

Start with [how to study this port](how-to-study-this-port.md), then use the [complete file index](INDEX.md). The [checker rejection lab](new-tests/checker-rejections.md) and [command runner error lab](new-tests/command-runner-errors.md) show how concrete discrepancies become regression tests. The [wallet-command lab](new-tests/wallet-commands.md) follows script replacement, signing and SQL failures. The [address-command lab](new-tests/address-commands.md) explains key creation, notification membership and saved preferences. The [witness-command lab](new-tests/witness-commands.md) follows partial script additions, positional MMR proofs and exception categories. The [coin ownership lab](new-tests/coin-ownership.md) distinguishes shared objects from equal copies. The [coin and proof reader lab](new-tests/coin-streams.md) explains partial mutation, retained lists and logging failures. The [chain command lab](new-tests/chain-commands.md) teaches database lookup, display contracts and runtime time zones. The [KISS test lesson](evidence/kiss-builtins.md) explains how the existing function comparisons work.

## Read the labels carefully

**Port status** describes code. The current totals are recorded in the [source map](../source-map.json) and [coverage audit](coverage-audit.json). Every counterpart is still partial and every file's full behavioral parity remains `not_verified`. A native entry point and H2/MVStore storage engine remain unfinished.

**Teaching status** describes this documentation. All 457 pages now contain file-specific explanations: 369 production lessons and 88 original-test lessons, with no remaining reading drafts. Each explains source logic, translation status, concrete test evidence or explicitly proposed validation, and remaining limits. This completes the requested first teaching pass; it does not certify exhaustive API review or complete translations. The machine-readable [coverage audit](coverage-audit.json) keeps documentation and code counts separate.

## A suggested reading order

1. **Values and bytes:** `objects/base/`, followed by `utils/json/`. Learn why an equal printed value may still have a different wire representation.
2. **Coins and proofs:** `objects/`, `objects/mmr/` and `objects/keys/`. Connect a coin's fields to hashing, membership proofs and signatures.
3. **KISS programs:** `kissvm/values/`, tokens, expressions, statements and functions. Read a small built-in lesson before studying Contract.
4. **Chain rules:** `database/txpowtree/`, cascade and `system/brains/`. Separate consensus decisions from caches and external effects.
5. **Runtime and storage:** `utils/messages/`, databases, networking and startup. Study shared ownership, failures and lifecycle alongside ordinary successful calls.
6. **Commands and original tests:** connect each user operation to its implementation and inspect what the original test actually asserts. Empty methods and commented examples do not provide executable coverage.

## Keeping the course current

For each next implementation/test change, update the corresponding file lesson with the observed discrepancy, reused native building block, minimal adaptation, test inputs/observations, result and remaining limits. The new-test lab can preserve the detailed experiment; file pages should link to it and explain the lasting rule.

The initial guide generator was a navigation aid; those inventories have now been replaced or supplemented by authored explanations. An import or compilation is never treated as proof of behavioral coverage. Preserve hand-edited lessons and the ownership exclusions in the audit tool when updating the course. The source pin, status and SHA-256 values come from [the authoritative mapping](../source-map.json).

- [Wallet-key lab](new-tests/wallet-keys.md): filter precedence, derivation checks and background work requests.
- [Bit-get/set lab](new-tests/bit-get-set.md): independent bit patterns, little-endian indexing and empty output arrays.
- [Vault/signing lab](new-tests/vault-and-signing.md): encrypted seed transitions, real signatures, exact failures and posting order.
- [Bit-counting lab](new-tests/bit-counting.md): mutable table entries, unsigned byte indices and Java integer wraparound.
- [Powers/significant digits lab](new-tests/power-significant-digits.md): argument order, exact rejection types and what delegation tests can establish.
- [Mining/posting lab](new-tests/transaction-mining.md): real mining, shared message owners, imported rows and side effects before failures.
- [Increment/decrement lab](new-tests/increment-decrement.md): exact decimal differences and the distinction between source assertion sites and executed assertions.
- [Transaction-check lab](new-tests/transaction-check.md): independent validity reports, shared transaction/witness owners, and partial in-place body reads.
- [Transaction-input lab](new-tests/transaction-inputs.md): selection order, floating inputs, partial witness changes, burn construction, and a shared numeric-parser correction.

- [Coin-selection and funding lab](new-tests/coin-selection-and-funding.md): grouping, reservations, mutations after failures and real send construction.

- [Reverse/length lab](new-tests/reverse-and-length.md): byte order, UTF-16 length, and what an arity rejection actually proves.

- [Offline transaction-file lab](new-tests/offline-send-files.md): snapshot ordering, repeated signing versus duplicate witnesses, exact filesystem errors and the demonstration entry point.

- [Unsigned-file lab](new-tests/unsigned-transaction-files.md): confirmed ancestors, fractional splitting, unsigned burns and the complete offline command sequence.

- [Send-polling lab](new-tests/send-polling-command.md): command rewriting, stable queue identities, snapshots, removal and real worker retries.

- [Wallet orchestration lab](new-tests/wallet-command-orchestration.md): child-command order, cleanup, nullable errors, exported bytes and decoded signature verification.

- [Raw-transaction lab](new-tests/raw-transaction-commands.md): contrasting input/script order, JSON casts, state conversion and real exported bytes.

- [Custom-address sending lab](new-tests/custom-address-send-commands.md): funding/posting error order, token output replacement, preserved change and decimal exponent limits.

- [Custom-address consolidation lab](new-tests/consolidate-custom-address.md): mature coin selection, stable order, one-versus-ten outputs, burn limits and helper cleanup.

- [Wallet consolidation lab](new-tests/wallet-consolidation.md): address grouping, forwarded send parameters, password redaction and failed-reply precedence.

- [Wallet balance lab](new-tests/wallet-balance.md): confirmation depth, sendable classification, token scaling, raw names and exact overflow errors.

- [Coin-search command lab](new-tests/coin-search-command.md): relevance by parameter presence, age/depth, reservations, ordering and reused selection.

- [Transaction-history lab](new-tests/transaction-history.md): SQL action selection, tracked-address totals, token scaling, collection order and bounded default timestamps.

- [TxPoW-search lab](new-tests/txpow-search.md): lookup precedence, RAM/SQL/archive fallbacks, stable deduplication and chain confirmations.

The [node-state lab](new-tests/node-state-commands.md) follows mempool counters, coin tracking and desired capacity preferences. It explains registration, typed JSON reads and null-coin evaluation order through failing Java/C++ comparisons and their fixes.

The [seed derivation lab](new-tests/seed-random-command.md) teaches deterministic framed hashing and wallet lock checks. The [original casting tests](new-tests/cast-original-tests.md) explain Boolean, hex, number and text conversions, including why SCRIPTTests tests STRING.

- [Node-status lab](new-tests/node-status-command.md): complete report assembly, decimal device estimates, median speeds, and explicit JVM/native memory boundaries.

- [RPC lab](new-tests/rpc-command.md): ordered configuration updates, shared user arrays, typed failures, and remaining live-server validation.

- [Historical burn-metrics lab](new-tests/burn-metrics.md): independent window
  totals, retained command state, lower-middle medians and null-sort mutations.

- [Wallet token-creation lab](new-tests/token-creation.md): supply, coloring,
  shared-name mutation, creator signatures and real mining queue effects.

- [Launcher boundary lab](new-tests/minima-entry-point.md): wrapper shape, nullable callers, starter failures, actual process hooks and the remaining storage/startup gap.
