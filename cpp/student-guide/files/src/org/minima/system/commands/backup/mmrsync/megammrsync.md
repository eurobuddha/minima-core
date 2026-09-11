# megammrsync — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/megammrsync.java) · [C++ counterpart](../../../../../../../../../src/org/minima/system/commands/backup/mmrsync/megammrsync.cpp)

## Purpose

megammrsync requests a chain snapshot and matching coin proofs using public wallet details, then imports them and replaces the saved peer list.

## Follow the command

1. mydetails displays address rows whose public key is not 0x00. This differs from getMyDetails used on the wire: it includes tracked addresses and all wallet public keys, as two separate lists.
2. resync first asks vault to stop ongoing key creation, validates host and probes the archive service with one block -1 request. A supplied phrase takes precedence over backup file, resets/derives wallet keys and sets use counters; otherwise a file runs restore shutdown:false then reopens/reset services, or existing keys are retained.
3. Build public sync data after reset and request MegaMMRIBD. Only after a nonnull response post MDS_RESYNC_START, clear MegaMMR when enabled and post IBD processing. Wait for the finished flag with no timeout, then run coinimport track:true for every received proof without inspecting its status.
4. Return a finished message and number of received proofs, save/shut down services, then update the P2P file after saveAllDB. updateP2PDB tokenizes comma-separated peers, catches individual parse failures, loads p2p.db, replaces its peer list and saves it.
5. On the serving side, getCurrentMegaMMRIBD obtains matching proofs, creates complete IBD and calls peers.getPeersList(20); that helper can return 21 peers because its guard is counter > max. getAllCoinProofs calls coinexport for each selected coin and reads response.data without checking status; a malformed result can fail mid-list.
6. searchMegaCoins walks tip-to-root, then MegaMMR only after reaching the root when enabled. Match coin address or address/key text in state first. For matching IDs, spent records suppress older copies; unspent records are deep-copied once and the ID marked seen. With an initially null tip the loop never enters MegaMMR at all.
7. The MegaMMR phase takes a read lock before copying/scanning its map and releases it at the end without finally. Nonmatching spent records are not added to the suppression set, making matching order/state relevant to edge cases.
8. sendMegaMMRSyncReq frames MSG_MEGAMMRSYNC_REQ in MiniData, uses 180-second connect/read timeouts, records payload byte counts, consumes a response type byte without checking it, and decodes MegaMMRIBD. Any caught exception logs and returns null; there are no retries in this helper.

## Translation and dependencies

The native class now maps the source methods and is registered in the real command runner. It reuses the existing MegaMMR and IBD codecs, database and wallet services, buffered file output, coin commands, and shutdown processing. The two commands retain their distinct reset and key-use rules. Main now supplies the serving helper as its default incoming MegaMMR callback. Full parity remains unverified.

## Understanding the tests

The [pre-test lab](../../../../../../../../../student-guide/new-tests/mega-mmr-commands.md)
and [driver](../../../../../../../../../tools/mega_commands_differential.py) compare
both commands with real H2 and the existing archive/peer fixtures. Synthetic wallet
rows make the selection difference visible: mydetails returns two rows with nonzero
public keys, while getMyDetails encodes two tracked addresses and all three key
rows, including the zero public key. These rows test selection, not usable signing keys.

Search cases vary tree shape, matching addresses, state text, public keys, duplicate
IDs and spent flags. A null initial tip must return no coins even with MegaMMR
enabled and populated. The fixture changes a returned coin afterward and compares
MegaMMR bytes to detect aliasing for coins selected from that map. Null lists and
null list elements exercise failure order. Peer-file cases compare exact files
for empty tokens, duplicates, invalid formats and valid loopback addresses.

Ten real loopback cases compare a hand-encoded empty-details request, matching
Java/native MegaMMRIBD bytes, ignored response type, malformed replies, null data
and missing Main/network/NIO receivers. Traffic assertions count payload bytes;
the unrelated total counters remain zero. The current-IBD fixture invokes Main's
new default callback rather than installing a successful response stub.

Both builds pass all 579 combined command/helper comparisons and ten network cases.
Those command fixtures alone do not establish successful live resync.
The request helper keeps the real 180-second timeout configuration, but delayed
read/connect timeout behavior has not been measured by these immediate replies.

## Limits

The [live-node lab](../../../../../../../../../student-guide/new-tests/mega-mmr-live-node.md)
uses actual Java/native programs and a saved genesis wallet. The production helper
serves its real owned coin proof, with identical Java/C++ bytes. Both resync runtimes
send the exact archive preflight and wallet-detail frames, receive that proof with
the real chain, shut down, and save the supplied loopback peers. Opposite-runtime
reopen preserves chain, coin proof and eight keys at 100 uses; signing works,
modified data fails, and only the signing key reaches 101.

This covers the existing-wallet branch and a genesis coin still present in the
tree. Phrase/file precedence, pruned-coin recovery, new heights, indefinite processor
waits and partial failures still need validation. The returned coins count is
received proof count, not verified successful imports. Reset can already have
occurred before the substantive synchronization request fails.

## Check your understanding

Why can mydetails omit an address that getMyDetails sends, and why does a null initial tip skip the MegaMMR scan?

Pinned Java SHA-256: `332dfeabffaff9d0f3cac438d31197f3269c0385c8069af8cca8d7de97504152`. [Mapping](../../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../../REVIEW.md).

## Backup/checker interruption follow-up

The 250 ms IBD-completion wait now uses NativeThread. Interruption propagates before importing coin proofs, saving peer state and final shutdown. The command retains its existing source order and network adapters. Direct interruption during a live MegaMMR resync remains separate validation work. See the [interruption lab](../../../../../../../../new-tests/backup-checker-interruption.md) for results and remaining limits. Full parity stays not_verified.
