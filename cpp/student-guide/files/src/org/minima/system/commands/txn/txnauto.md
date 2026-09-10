# txnauto — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnauto.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/txn/txnauto.cpp)

## Purpose

txnauto’s command creates a stored draft through txnaddamount, then optionally adds witnesses and signatures. Its separate static helper has materially different behavior.

## Follow the command

1. Parse id/address/amount/token/sign/mmrscript/burn. Reject positive token burns and an existing ID, then create the row before invoking nested txnaddamount.
2. If that nested result has status false, delete the row and throw its error. Otherwise retrieve the row and, by default, call strict txnutils.setMMRandScripts. An exception from this direct helper bypasses the explicit deletion branches.
3. If sign is true, run nested txnsign publickey:auto; a returned false status deletes the row. Return its current JSON after successful preparation. The command does not post or mine it.
4. The static createTransaction(address, amount) instead builds an unstored Minima-only row directly: validate amount, choose nonpending/nonmining relevant simple coins, add proofs/scripts and deduplicate signing keys.
5. That helper accepts zAddress but never uses it and creates only a positive change output. With inputs 10 and requested amount 3, output is change 7: the missing 3 behaves as a burn, not payment to the supplied address. It calculates IDs and signs before returning the temp row.

## Translation and dependencies

The native command reuses actual registered txnaddamount and txnsign, with CommandResult for typed child status/error handling. Proof preparation calls txnutils directly. The static helper shares the established burn builder through AutomaticTransaction.hpp: early amount validation is enabled, the link assignment is omitted, and signing is required. Database/miner lookup order and the unused address argument are preserved.

## Understanding the tests

The [automatic-transaction lab](../../../../../../../new-tests/automatic-transaction.md) uses real child commands, Java-generated coins/proofs/keys and recording SQL. It compares stored transaction/witness wire, replies, SQL effects, locks and errors. Independent checks distinguish deletion from retained proof failures and verify signatures. Static-helper checks cover unused addresses including null, change-only outputs, input/output difference and the default one-byte link hash.

- [Actual command construction path](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnaddamount.java)
- [Witness preparation](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnutils.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both builds pass 414 programs / 1,655 operations. Consult the lab and retained reports for regression results; finite tests do not certify every inherited API.

## Limits

Fixture transactions are created and signed using real components; durable storage and network admission remain unverified. Deleting a row after nested failure does not undo shared coin locks or signatures already consumed. The helper’s copied burn error text is another reason to test behavior rather than infer it from names.

## Check your understanding

For a 10-Minima input and requested 3, how do the command path and static helper differ in recipient outputs?

Pinned Java SHA-256: `a5c8afeaafb61976136574a220b41208bc0ac2e8895f84e4d11b252bdee07686`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
