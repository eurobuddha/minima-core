# txnauto — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnauto.java) · `src/org/minima/system/commands/txn/txnauto.cpp` (absent)

## Purpose

txnauto’s command creates a stored draft through txnaddamount, then optionally adds witnesses and signatures. Its separate static helper has materially different behavior.

## Follow the command

1. Parse id/address/amount/token/sign/mmrscript/burn. Reject positive token burns and an existing ID, then create the row before invoking nested txnaddamount.
2. If that nested result has status false, delete the row and throw its error. Otherwise retrieve the row and, by default, call strict txnutils.setMMRandScripts. An exception from this direct helper bypasses the explicit deletion branches.
3. If sign is true, run nested txnsign publickey:auto; a returned false status deletes the row. Return its current JSON after successful preparation. The command does not post or mine it.
4. The static createTransaction(address, amount) instead builds an unstored Minima-only row directly: validate amount, choose nonpending/nonmining relevant simple coins, add proofs/scripts and deduplicate signing keys.
5. That helper accepts zAddress but never uses it and creates only a positive change output. With inputs 10 and requested amount 3, output is change 7: the missing 3 behaves as a burn, not payment to the supplied address. It calculates IDs and signs before returning the temp row.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse nested command orchestration for runCommand, and preserve the separate static helper’s actual body when mapping. It must not be silently replaced by the superficially similar payment workflow.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed command fixtures assert creation/deletion order, nested arguments, returned-status versus thrown-exception cleanup and mmrscript/sign defaults. Separate static-helper cases assert unused address, only-change outputs, exact input-output difference, pending exclusions, proof-history selection and key-use changes. No existing runner fake-command case executes these absent bodies.

- [Actual command construction path](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnaddamount.java)
- [Witness preparation](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnutils.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No transaction was created or signed. Deleting a row after nested failure does not undo shared coin locks or signatures already consumed. The helper’s copied burn error text is another reason to test behavior rather than infer it from names.

## Check your understanding

For a 10-Minima input and requested 3, how do the command path and static helper differ in recipient outputs?

Pinned Java SHA-256: `a5c8afeaafb61976136574a220b41208bc0ac2e8895f84e4d11b252bdee07686`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
