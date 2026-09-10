# coincheck — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/coincheck.java) · [Mapped native body](../../../../../../../../src/org/minima/system/commands/base/coincheck.cpp)

## Purpose

coincheck decodes an exported CoinProof and checks whether its unspent coin proof is time-valid against the current tip MMR.

## Follow the command

1. Read required data as trimmed text, parse it directly as MiniData hex, and use CoinProof.convertMiniDataVersion. This path does not use getDataParam’s Minima-address conversion.
2. Get the decoded coin and reject a spent coin with a specific CommandException before consulting the chain tip.
3. Obtain the tip, reconstruct an MMR leaf by serializing the coin with its amount, and call checkProofTimeValid with the coin entry number, reconstructed leaf and supplied proof.
4. Return proofblock, the coin JSON and the boolean valid. A false proof check returns a normal command response rather than a command exception.

## Translation and dependencies

The native body reuses CoinProof/MMRData/MMR and Command. TxPoWSearcher service callbacks provide the current tip in place of global MinimaDB access. It uses raw numeric JSON text for MiniNumber proofblock. Null decoded proof/tip paths use native runtime errors, so exact Java exception diagnostics remain incomplete.

## Understanding the tests

The differential fixture builds twelve linked MMR snapshots, creates coins at blocks 0–3, and spends one coin at block 6. At tip 11 it runs this actual command on original proofs, a spent proof, appended bytes, truncated bytes and a mutated prefix, in both normal/test parameter modes. It compares full replies, CommandException messages and read-lock counts. Non-command exceptions are collapsed to ERROR; command metadata and fresh getFunction replies are also compared.

- [Java real history and command fixture](../../../../../../../../support/SearcherOracle.java)
- [Native history and command fixture](../../../../../../../../support/searcher.cpp)
- [Searcher and three concrete command comparisons](../../../../../../../../tools/searcher_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The fixture uses a real MMR history but controlled database/chain access. It does not establish all malformed proof inputs, provider/null error messages or live chain races. A decoded proof being syntactically valid is weaker than valid=true against the current MMR.

## Check your understanding

Why can an old proof for an originally unspent coin become invalid after the fixture spends it at block 6?

Pinned Java SHA-256: `d0a8dd9e72541fec0812468b61c160d7c2bb78a4f1dd30896ee3a6d71c4958f5`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
