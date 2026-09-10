# megammrsync — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/megammrsync.java) · `src/org/minima/system/commands/backup/mmrsync/megammrsync.cpp` (absent)

## Purpose

megammrsync requests a chain snapshot and matching coin proofs using public wallet details, then imports them and replaces the saved peer list.

## Follow the command

1. mydetails displays address rows whose public key is not 0x00. This differs from getMyDetails used on the wire: it includes tracked addresses and all wallet public keys, as two separate lists.
2. resync first asks vault to stop ongoing key creation, validates host and probes the archive service with one block -1 request. A supplied phrase takes precedence over backup file, resets/derives wallet keys and sets use counters; otherwise a file runs restore shutdown:false then reopens/reset services, or existing keys are retained.
3. Build public sync data after reset and request MegaMMRIBD. Only after a nonnull response post MDS_RESYNC_START, clear MegaMMR when enabled and post IBD processing. Wait for the finished flag with no timeout, then run coinimport track:true for every received proof without inspecting its status.
4. Return a finished message and number of received proofs, save/shut down services, then update the P2P file after saveAllDB. updateP2PDB tokenizes comma-separated peers, catches individual parse failures, loads p2p.db, replaces its peer list and saves it.
5. On the serving side, getCurrentMegaMMRIBD obtains matching proofs, creates complete IBD and attaches up to twenty peers. getAllCoinProofs calls coinexport for each selected coin and reads response.data without checking status; a malformed result can fail mid-list.
6. searchMegaCoins walks tip-to-root, then MegaMMR only after reaching the root when enabled. Match coin address or address/key text in state first. For matching IDs, spent records suppress older copies; unspent records are deep-copied once and the ID marked seen. With an initially null tip the loop never enters MegaMMR at all.
7. The MegaMMR phase takes a read lock before copying/scanning its map and releases it at the end without finally. Nonmatching spent records are not added to the suppression set, making matching order/state relevant to edge cases.
8. sendMegaMMRSyncReq frames MSG_MEGAMMRSYNC_REQ in MiniData, uses 180-second connect/read timeouts, records payload byte counts, consumes a response type byte without checking it, and decodes MegaMMRIBD. Any caught exception logs and returns null; there are no retries in this helper.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse the native public-data/response wire containers plus archive probe framing, wallet selection and CoinProof services. Those containers do not implement this command’s reset, selection, lock or network orchestration.

## Understanding the tests

The inspected MMR-sync driver compares the three container formats, aliases and partial reads. Proposed command/helper tests should cover displayed-versus-sent detail selection, phrase/file precedence, preflight and post-reset failures, null tip with MegaMMR data, duplicate/spent/state matches, lock release on failure, malformed coinexport, ignored coinimport status, wrong response type and peer replacement after database save.

- [Java serialization/state oracle](../../../../../../../../../support/MMRSyncOracle.java)
- [Native comparison fixture](../../../../../../../../../support/mmrsync.cpp)
- [Concrete mutation and truncation cases](../../../../../../../../../tools/mmrsync_differential.py)
- [Deterministic Java TxPoW fixture](../../../../../../../../../support/ArchiveCacheOracle.java)
- [Native TxPoW fixture](../../../../../../../../../support/ArchiveFixture.hpp)
- [Shared parameter parsing and reply envelope](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No wallet details were transmitted and no peer/database change occurred. The returned coins count is received proof count, not verified successful imports. Reset can already have occurred before the substantive synchronization request fails.

## Check your understanding

Why can mydetails omit an address that getMyDetails sends, and why does a null initial tip skip the MegaMMR scan?

Pinned Java SHA-256: `332dfeabffaff9d0f3cac438d31197f3269c0385c8069af8cca8d7de97504152`. [Mapping](../../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../../REVIEW.md).
