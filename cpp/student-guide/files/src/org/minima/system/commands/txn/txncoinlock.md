# txncoinlock — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncoinlock.java) · [native implementation](../../../../../../../../src/org/minima/system/commands/txn/txncoinlock.cpp)

## Purpose

txncoinlock toggles the shared in-memory exclusion list used by txnaddamount for offline transaction construction.

## Follow the command

1. Read action with absent default empty. Only exact lock and unlock values invoke a change; unknown values behave like a query.
2. lock calls enableCoinLock(true), retaining any existing list. unlock calls enableCoinLock(false), which clears the list as well as disabling checks.
3. Return coinslocked from the shared enable flag. The command does not lock a database, reserve network coins or inspect a particular transaction.

## Translation and dependencies

The native command uses txnaddamount’s actual static list and flag. Both command and helper paths share that state; there is no separate mock lock implementation.

## Understanding the tests

The new [coin-selection and funding lab](../../../../../../../new-tests/coin-selection-and-funding.md) tests enable, add duplicate IDs, query, enable again and disable/re-enable, asserting both flag and list contents. Unknown action should preserve state; blank supplied text should fail through Command. Concurrent selection needs separate tests because a flag is not a mutex.

- [Shared coin-lock implementation](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnaddamount.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both builds pass the shared 736-program corpus with 3,418 operations and 39,016 recording SQL calls. These are suite-wide totals, not assertions for this file alone.

## Limits

The flag reports whether exclusion is active, not the number of reserved coins. It affects cooperating construction code only.

## Check your understanding

Why does unlock followed by lock forget previously reserved coin IDs?

Pinned Java SHA-256: `5d934449671a1cdd94f67b6a2b1b7f1fde165d3045cfcd497c925bc944c79a7c`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
