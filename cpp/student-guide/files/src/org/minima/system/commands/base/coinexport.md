# coinexport — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/coinexport.java) · [Mapped native body](../../../../../../../../src/org/minima/system/commands/base/coinexport.cpp)

## Purpose

coinexport finds a coin and exports it with an MMR proof selected from retained history.

## Follow the command

1. Parse required coinid text as MiniData and search for the coin. A missing coin raises a CommandException containing the supplied ID text.
2. Read its creation block and current tip. Choose a history distance of 256 normally or 8 when TEST_PARAMS is enabled.
3. Compute back=tip height−history and clamp it upward to the coin creation block. Select tip.getPastNode(back), obtain its MMR proof-to-peak at the coin’s entry number, and construct CoinProof.
4. Return both coinproof JSON and hex of its exact serialized bytes. No coin ownership or spending ability is transferred by this data.

## Translation and dependencies

The native body reuses TxPoWSearcher, tree history, MMRProof and CoinProof serialization. Service callbacks replace MinimaDB globals. It preserves the history clamp and TEST_PARAMS branch, adding explicit runtime errors for missing tip/history node; those errors are not exact Java diagnostics.

## Understanding the tests

The twelve-block Java-generated MMR fixture supplies four coin IDs created at successive early blocks and runs actual coinexport in normal and test parameter modes. IDs include found coins, missing 0x99, malformed hex, Mx, empty hex and blank text. The driver compares complete JSON/serialized proof bytes, command errors, metadata/fresh factory replies and lock counts. The shorter test history exercises a different selected proof block.

- [Java real history and command fixture](../../../../../../../../support/SearcherOracle.java)
- [Native history and command fixture](../../../../../../../../support/searcher.cpp)
- [Searcher and three concrete command comparisons](../../../../../../../../tools/searcher_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

Non-command failures collapse to ERROR. The command still relies on correct search selection and retained history. Full live-node locks and archive fallback are not established by the fixture.

## Check your understanding

With tip 11, history 8 and a coin created at block 4, which block should supply the proof and why?

Pinned Java SHA-256: `247ddce68a0c9d6a1e1cebaf7a6f371e8f17573dda94e5b0d71a3ba5b486d558`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
