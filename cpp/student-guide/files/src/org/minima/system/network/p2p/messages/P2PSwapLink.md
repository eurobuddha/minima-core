# P2PSwapLink — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/messages/P2PSwapLink.java](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PSwapLink.java) | `src/org/minima/system/network/p2p/messages/P2PSwapLink.cpp` (planned) |

## What this file does

Preserve the source inventory entry for an entirely commented-out historical swap-message sketch. This pinned file declares no active Java package, class or method.

## Follow the logic

1. Every line is a comment, including package, imports, class and constructors. Java compilation therefore produces no P2PSwapLink class from this file.
2. The historical sketch describes secret and destination serialization plus two flags. Because the body is inactive, it is a reading aid rather than executable protocol behavior.
3. Its commented address stream calls are not the active InetSocketAddressIO JSON API. The working JSON swap message is P2PDoSwap.

## How the translation is approached

The source map still requires a unique destination for this Java file, and currently records that counterpart as absent. A future inventory-only translation must make the inactive status explicit; reviving the commented design would add behavior rather than preserve the pin.

## What the tests tell us

There is no active class here to instantiate or test. A meaningful future mapping check would verify that its counterpart exists without registering a new protocol type. P2PDoSwap tests cannot establish coverage for this inactive sketch.

## Dependencies and reading map

## Check your understanding

Explain the difference between “a Java file exists” and “a Java class is compiled.” Why should a 1:1 file inventory retain this entry?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `6568b163f8646bda023657d5c6344f9308160d5fd686d2932175ef544e5eb3b3`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
