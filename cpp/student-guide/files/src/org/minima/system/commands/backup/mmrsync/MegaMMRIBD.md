# MegaMMRIBD — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/MegaMMRIBD.java) · [Mapped native body](../../../../../../../../../src/org/minima/system/commands/backup/mmrsync/MegaMMRIBD.cpp)

## Purpose

MegaMMRIBD carries an initial IBD, ordered coin proofs and a peer-list string in a synchronization response.

## Follow the command

1. The populated constructor aliases the provided IBD and proof list; peers initially equals empty text. Getters expose these references and the peers setter accepts its assigned string.
2. Write version 2, IBD bytes, MiniNumber proof count, each CoinProof in order, and MiniString peers. No independent framing surrounds the IBD or each proof.
3. Read but ignore version. Replace IBD before reading its body. Only after that succeeds replace the proof list, read its count and append each fully decoded proof.
4. Assign peers only after its MiniString read succeeds. A negative narrowed proof count executes no loop and proceeds straight to peers; truncated reads retain whichever old fields were not yet replaced.

## Translation and dependencies

The native class uses shared_ptr for IBD/proof-list aliases, shared_ptr elements for nullable Java entries and optional<string> for nullable peers. Its staged assignments reproduce partial state: IBD first, then list, peers last. Null serialization throws explicitly rather than dereferencing a null pointer.

## Understanding the tests

The driver’s make fixture constructs the object before adding proofs to the same list, so matching populated output checks alias preservation. It includes the UTF-8 peer text π.example, exact IBD/proof bytes, appended trailing data, ignored-version mutations and truncated reads into existing/default objects. Results compare consumed byte offsets, reference replacement and surviving peers/partial proof lists as well as error classes.

- [Java serialization/state oracle](../../../../../../../../../support/MMRSyncOracle.java)
- [Native comparison fixture](../../../../../../../../../support/mmrsync.cpp)
- [Concrete mutation and truncation cases](../../../../../../../../../tools/mmrsync_differential.py)
- [Deterministic Java TxPoW fixture](../../../../../../../../../support/ArchiveCacheOracle.java)
- [Native TxPoW fixture](../../../../../../../../../support/ArchiveFixture.hpp)
- [Shared parameter parsing and reply envelope](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The response format reader does not validate version 2, proof correctness, peers syntax or consistency with IBD. The inspected fixture does not directly test null individual proof elements or null peers, despite those being representable in the API.

## Check your understanding

Why can peers retain its old value while IBD and the proof-list object have already changed after a failed read?

Pinned Java SHA-256: `b9c03c5e38960d009a1f36da173250a9e3bd81dc71a8ff79670ba13118ef6736`. [Mapping](../../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../../REVIEW.md).
