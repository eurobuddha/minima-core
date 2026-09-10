# coinimport — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/coinimport.java) · `src/org/minima/system/commands/base/coinimport.cpp` (absent)

## Purpose

coinimport installs a coin and its MMR proof into retained chain history, optionally marking it relevant. It has an important current-proof shortcut before the historical-proof update.

## Follow the command

1. Decode required data into CoinProof, get its coin and reject a spent coin. Ask the current tip MMR to generate a proof for that entry and check the generated proof against the imported coin leaf.
2. If that current proof is valid and the coin already has a tree node, reject an already relevant entry; otherwise append the entry to relevance, recalculate relevant coins and return the original supplied proof JSON. This shortcut does not read track and does not validate the supplied proof itself.
3. Otherwise check the supplied historical proof against the tip. Find the retained node at its proof block time; reject an invalid proof or missing old block.
4. Save the old MMR root, unfinalize the historical MMR, update the entry using the supplied proof and leaf, finalize it and add the coin to the node.
5. Only now parse track with default true. If true, append the coin and its entry to relevance lists. Finally compare the new root with the old root, throwing on a change without undoing earlier mutations, and return the proof JSON.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse CoinProof decoding, MMR time-valid checks and tree relevance operations. Preserve mutation and parameter-reading order: moving track validation earlier would change failure side effects.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests cover spent coins, malformed/partial decoding, valid current proof with an invalid supplied proof, shortcut track:false, already relevant entries, too-old proof blocks, historical root invariance and invalid track after mutation. Inspect node/MMR/relevance state after every failure, not only the response.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

A root-invariance error is detected after updates and has no rollback in this body. A response containing the original proof does not imply the shortcut checked that exact proof.

## Check your understanding

Why can track:false still result in tracking on the current-proof shortcut?

Pinned Java SHA-256: `2dae62827d0cbcba71a3d73b513bb89ba1793155b82a3d48c59e15b909c374be`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
