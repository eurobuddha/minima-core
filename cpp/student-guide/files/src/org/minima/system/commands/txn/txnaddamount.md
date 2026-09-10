# txnaddamount — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnaddamount.java) · [native implementation](../../../../../../../../src/org/minima/system/commands/txn/txnaddamount.cpp)

## Purpose

txnaddamount selects inputs for an amount, adds destination/change outputs and optionally reserves input IDs in its shared offline lock list.

## Follow the command

1. Read amount/token/burn before row lookup; resolve custom-token metadata and convert display amount to raw Minima units. Positive burns are rejected for custom tokens. Require a row; a missing tip returns an empty response array without filling it.
2. Check token round-trip precision or Minima value validity, parse onlychange/fromaddress and search either relevant simple coins or unrestricted coins at the selected address with global MegaMMR support. Remove IDs in the static lock list when enabled.
3. Call send.selectCoins for raw amount plus burn. That helper groups descending-size coins by address, seeks one sufficient coin while skipping pending/mining coins, but returns its whole ordered list if no single coin succeeds. The fallback includes those skipped coins, and grouping can invalidate its early-break assumption about remaining sizes.
4. Sum selected amounts and reject negative change. Append every selected input and reserve its ID before parsing storestate, address or split. Later failures therefore leave inputs and locks changed.
5. Unless onlychange, divide raw amount by split, emit that many equal destination outputs and an extra positive remainder output to preserve total. There is no explicit positive split guard; zero division fails and negative split skips the loop before remainder handling.
6. Add positive change with storeState false, using fromaddress when supplied or a default wallet address otherwise. Even with fromaddress, the default wallet lookup happens first. Precompute coin IDs, recalculate transaction ID and return the row.
7. The lock helpers add unique IDs only when enabled, return false when disabled and clear all IDs on disable. They have no persistence or synchronization and do not automatically release reservations when later construction fails.

## Translation and dependencies

The native translation reuses Command parameter/reply helpers, send.selectCoins, Token scaling, shared Transaction/Coin handles, TxPoWSearcher, Wallet default-address lookup and TxPoWGenerator ID computation. It preserves the source sequence of effects and failures.

## Understanding the tests

The new [coin-selection and funding lab](../../../../../../../new-tests/coin-selection-and-funding.md) fixtures distinguish a sufficient single coin from an insufficient-single fallback with multiple coins, pending/mining entries, address grouping and locked IDs. Assert scaling, burn, onlychange, split rounding/zero/negative, existing inputs and failure after reservation. Repeated invocation should reveal any duplicate inputs rather than assuming row-aware filtering.

- [Selection and grouping helpers](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java)
- [Lock toggle](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncoinlock.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both builds pass the shared 736-program corpus with 3,418 operations and 39,016 recording SQL calls. These are suite-wide totals, not assertions for this file alone.

## Limits

This command itself has no coinage parameter, proof/script setup or rollback. Locking is a cooperative in-memory exclusion mechanism, not network reservation.

## Check your understanding

Why must a failed address/split test inspect both the stored transaction and LOCKED_COINS?

Pinned Java SHA-256: `8b5af169ae0fdfc0ce6ca7fb2009af0c67276886691f6fa182ae4f3a974f7317`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
