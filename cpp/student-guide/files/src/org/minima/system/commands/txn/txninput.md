# txninput — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txninput.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/txn/txninput.cpp)

## Purpose

txninput appends a found, decoded or floating coin to a stored custom transaction, then optionally adds its proof/script.

## Follow the command

1. Look up the required row and parse floating. Selection precedence is coinid, then coindata, then address/amount/token fields.
2. coinid searches for an unspent coin and rejects a missing result. The searcher returns a deep copy. If floating is true, reset that returned copy’s ID to COINID_ELTOO; the command adds no further copy. coindata instead decodes Coin bytes and optionally resets its ID.
3. Without coinid/coindata, normalize address and convert displayed amount through token scaling when needed. Construct an ELTOO coin regardless of whether floating was supplied or false, attaching token metadata when found.
4. Append the coin, precompute output coin IDs and recalculate transaction ID. Only then parse scriptmmr (default false); when true call the single-coin txnutils helper.
5. Return row JSON. Errors in the late scriptmmr parsing or proof/script helper leave the appended input and refreshed IDs in place.

## Translation and dependencies

The native counterpart now compiles. It reuses Coin conversion, TxPoWSearcher, Token scaling and the single-coin witness helper. The shared Coin handle preserves the input/proof relationship inside the command. The searcher already deep-copies the chain coin, so the command does not mutate that chain object.

## Understanding the tests

The Java/C++ experiment compares all source modes and precedence, floating:false field construction, missing tokens, malformed Coin bytes, search-result ID mutation and late witness failures. The single-coin helper does not re-resolve floating placeholders like its transaction overload; test that difference explicitly.

- [Single-coin helper and transaction overload](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnutils.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

The [transaction-input lab](../../../../../../../new-tests/transaction-inputs.md) explains the executed comparisons and their limits. Both builds pass the 671-program comparison and all 13 selected regressions; consult the lab for the scope and limitations.

## Limits

The help mentions coinexport, but this branch invokes Coin decoding, not CoinProof decoding. Exact accepted byte behavior therefore requires concrete fixtures. No coin was imported or spent.

## Check your understanding

Why does address/amount input construction produce an ELTOO coin even with floating:false?

Pinned Java SHA-256: `d6a8db605028bcde0dd104cd2f02f50f26dd4582fbca22c74c619d50e45f11c1`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
