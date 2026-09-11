# Transaction inputs — mutation before failure

This lab follows `txninput`, `txnbasics`, and the shared `txnutils` methods. Read the [Java input command](../../../upstream-study/src/org/minima/system/commands/txn/txninput.java), [C++ input command](../../src/org/minima/system/commands/txn/txninput.cpp), [Java utility](../../../upstream-study/src/org/minima/system/commands/txn/txnutils.java), and [C++ utility](../../src/org/minima/system/commands/txn/txnutils.cpp) together.

## What are we constructing?

A transaction describes coins consumed and coins created. Its witness carries evidence: coin membership proofs, scripts, and signatures. These are separate objects. Adding an input does not automatically make its witness complete, and a command failure does not automatically undo earlier changes.

`txninput` first finds the named custom transaction. It selects an input by coin ID, serialized coin bytes, or address/amount/token details, in that order. If both coin ID and coin bytes are supplied, coin ID wins. With address/amount details it creates an ELTOO floating input even when the `floating` parameter is absent or false. That follows the source's actual branches, which are more precise than a summary of its help text.

A floating input has a special coin ID. Automatic witness construction searches for the most recent unspent coin matching its amount, address and token. A concrete input is searched by ID so the proof uses its current creation block and MMR entry. The utility collects these resolved coins separately; it does not replace the transaction's input list.

## Choosing the proof block

Let `H` be the tip height, `C` the largest creation height among resolved inputs, starting at zero, and `P` the configured proof history. The utility computes `D = H - C`, caps `D` at `P`, then requests the exact ancestor at `H - D`. This uses MiniNumber arithmetic, including fractional values; it is not an integer approximation. A missing ancestor is an error.

After choosing that block, each coin gets a proof to an MMR peak. The witness receives the coin proof **before** the wallet script is looked up. If that lookup fails, the proof already added remains. The transaction overload can skip absent coins/scripts when `exitOnFail` is false. The single-coin overload always looks up the script, even if one is already in the witness, and reports a different exception category for a missing script.

`txnbasics` calls the transaction overload with `exitOnFail=false`, then recalculates output coin IDs and the transaction ID. It can succeed with an incomplete witness. That success is not evidence that the transaction is valid for broadcast.

## Burn transactions

`createBurnTransaction` scans relevant, simple, unspent Minima coins in search order. It excludes caller-listed coins, coins currently being mined, and coins used in the mempool. It stops once the amount is covered, attaches proofs and scripts, validates the amount, and creates a change output if needed. That output does not retain state. It sets the link to the main transaction, computes IDs, and optionally signs once per distinct requested public key.

Order matters: amount validity is checked after proof and script work. A zero amount may still select a coin when one is available. The native code reuses the existing searcher, MMR, wallet signing, MiniNumber arithmetic and ID generator. No separate cryptographic algorithm or SQL dialect is added.

## The experiment

The [corpus](../../tools/transaction_input_corpus.py) runs through the existing [Java harness](../../support/WalletCommandsOracle.java), [native harness](../../support/wallet_commands.cpp), and [comparison driver](../../tools/wallet_commands_differential.py). Java creates the initial coin and MMR bytes; both runtimes load them into their actual native/source objects. Each operation compares the custom transaction database's complete serialized bytes, monotonic-check flags, replies, error categories, logged bodies and SQL call sequence.

The corpus includes absent and spent coins, repeated inputs, competing input parameters, floating inputs, truncated bytes, wrong JSON types, bounded and fractional proof histories, repeated witness calls, empty chains, mining exclusions, and wallet failures injected at specific SQL calls. A successful reply alone would miss the most instructive cases: an input or proof can remain after a later error. Full database bytes expose that state.

The JDBC/SQL receiver records calls and returns supplied rows; it is not a durable database engine. Java Main and miner fixtures bypass worker startup. The native miner fixture constructs the real miner without starting its worker and populates mining membership through its existing queueing method. The miner lookup still needs binding by full native Main startup. Random change-address selection is only compared with a single candidate. These experiments do not prove full node integration, persistence, concurrent calls, or complete object aliasing.

The first run stopped at zero-based case 341: source `amount:bad` reported the invalid character, while native MiniNumber emitted a generic error. The [shared syntax helper](../../src/support/JavaBigDecimalSyntax.hpp) now follows the installed OpenJDK 26 decimal parser's diagnostic order. It distinguishes its compact (at most 18 UTF-16 units after the sign) and longer parsing paths, repeated decimal points, missing digits and invalid exponents. Existing native arithmetic and rounding are reused. This does not establish all extreme exponent or precision behavior.

After that repair, all 609 original programs passed. The expanded normal run passes **671 programs, 2,734 operations and 41,653 recorded SQL calls**. It includes token scaling, signed and unsigned burns, actual mempool membership in both witnesses, cascade flags, and explicit assertions about successful burn input/output/proof/signature counts. The sanitizer run also passes the same 671 programs and totals. Both full builds pass. **All 14 selected checks pass in each build**: the new input suite plus 13 regressions covering primitives, objects, transaction commands, search, wallet scripts/addresses/witnesses, command parsing/dispatch/errors, miner, MinimaDB and source mapping. The input suite took 38.68 seconds normally and 207.73 seconds under sanitizers while overlapping other checks; regressions took 101.73 and 276.80 seconds. This is a selected suite, not all 143 registered tests.

The reports are [normal](../../validation/automatic-transaction-work-in-progress/normal/transaction-input-commands-results.json) and [sanitized](../../validation/automatic-transaction-work-in-progress/sanitized/transaction-input-commands-results.json). The retained [first mismatch](../../validation/student-linked-evidence/reports/transaction-input-commands-syntax-baseline-results.json) is fail-fast evidence: it evaluated 342 programs before stopping, not all 609 initially planned.

## Exercises

1. Supply both `coinid` and invalid `coindata`. Which one is examined? The coin-ID branch wins; coin bytes are not read.
2. Add an input with `scriptmmr:true`, then make the wallet script query fail. What should you inspect? The transaction and witness bytes, because input insertion and proof insertion occur before the failing query.
3. Why does `txnbasics` succeeding not prove broadcast validity? Its permissive utility skips unresolved inputs or scripts; later transaction validation must reject missing evidence.
4. Does an existing witness script prevent a wallet query? In the transaction overload it can; the single-coin overload still queries the wallet.
