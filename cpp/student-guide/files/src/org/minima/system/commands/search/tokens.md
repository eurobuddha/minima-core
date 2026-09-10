# tokens — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/search/tokens.java) · [Mapped native body](../../../../../../../../src/org/minima/system/commands/search/tokens.cpp)

## Purpose

tokens lists known token metadata and imports or exports serialized Token objects. Exact action strings choose the branches; an unknown action falls through to normal listing/lookup.

## Follow the command

1. Read tokenid and action with absent defaults of empty text. A supplied blank value is rejected by Command even though an absent one is allowed.
2. For export, parse tokenid as MiniData, look it up, fail if absent, then return the original tokenid text and serialized token hex.
3. For import, decode data through Token.convertMiniDataVersion and call TxPoWSearcher.importToken before producing newtok.toJSON. If decoding returns null or a partially valid value, mutation can precede later formatting failure.
4. For all other actions, an empty tokenid produces an array beginning with a synthetic Minima entry, followed by getAllTokens order. A nonempty tokenid returns one token or raises Token not found.
5. getFunction creates a fresh command and help/valid-parameter methods expose fixed metadata. Some help descriptions are copied from key commands; the active branches implement token import/export.

## Translation and dependencies

The native body uses existing Token wire/JSON and TxPoWSearcher storage, preserving import-before-formatting and fallback action behavior. It converts the decoded optional token to shared ownership and imports null before throwing on a missing object. Native chain token reference identity remains a known incomplete searcher behavior.

## Understanding the tests

The inspected driver invokes the real tokens command across empty/one/four-node chain shapes, actions absent/export/import/IMPORT/bad/blank, and present/missing/malformed token IDs. It imports normal, appended, truncated and specially altered serialized token bytes, then lists and exports afterward to observe failure state. It compares exact results and CommandException messages, while other exceptions collapse to ERROR. Metadata and fresh getFunction replies are checked too.

- [Java real history and command fixture](../../../../../../../../support/SearcherOracle.java)
- [Native history and command fixture](../../../../../../../../support/searcher.cpp)
- [Searcher and three concrete command comparisons](../../../../../../../../tools/searcher_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

These cases cover concrete command execution, but controlled token/chain services are not full node integration. Imported metadata does not prove that a token exists or is valid on a live chain. Exact arbitrary failure diagnostics and identity mutation remain incomplete.

## Check your understanding

Why might action:IMPORT list tokens instead of importing them, and why must a failure test inspect token storage after the call?

Pinned Java SHA-256: `f5ac4157eae0515d70424031405071853d8ea5cdafe40d2d028e617dd5a872f8`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
