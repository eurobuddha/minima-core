# P2PSwapLink — source logic and validation

**Teaching status:** Authored source and validation explanation; full parity remains open.

**Port status:** `inactive_source_preserved`; parity `not_verified`.

[Original Java](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PSwapLink.java) · [Native counterpart](../../../../../../../../../src/org/minima/system/network/p2p/messages/P2PSwapLink.cpp)

## Purpose and logic

The complete source is commented out. The native file preserves those comments and declares no protocol type. It is inventory preservation, not functional implementation credit.

## Concrete test evidence

[Student lab](../../../../../../../../new-tests/original-processor-test.md) explains all active logic, the zero original assertions,
ten supplemental delay observations, twenty known-outcome checks, the selective
exception guard and four inactive-source checks. Normal and sanitizer tests pass.
The native interruption tests inject the runtime callback; no built-in native
Thread.interrupt facility is claimed. The source map explicitly distinguishes the
inactive file from an active partial implementation.

[Evidence and limits](../../../../../../../../../validation/original-processor-test-work-in-progress/progress.json). Passing these checks does not
establish TxPoWProcessor behavior, active swap serialization or full node parity.

Pinned Java SHA-256: `6568b163f8646bda023657d5c6344f9308160d5fd686d2932175ef544e5eb3b3`.
