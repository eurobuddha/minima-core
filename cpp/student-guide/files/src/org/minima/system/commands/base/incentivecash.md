# incentivecash — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/incentivecash.java) · `src/org/minima/system/commands/base/incentivecash.cpp` (absent)

## Purpose

incentivecash optionally stores a user identifier, then asks the source’s configured remote rewards endpoint for details when an identifier is nonempty.

## Follow the command

1. If uid exists, trim/validate it, set UserDB’s incentive ID and save UserDB immediately.
2. Read the stored ID and put it in the response.
3. For a nonempty ID, concatenate it into the fixed HTTPS ping path and append the Minima version query, then call RPCClient.sendPUT.
4. Parse the reply and cast it to JSONObject before adding details. Invalid JSON/wrong top-level type/network failure propagates after a newly supplied ID may already be saved. The ID is concatenated without explicit URL encoding.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse UserDB persistence, RPCClient PUT semantics and the pinned JSON parser. The URL is historical source behavior; this guide does not assert the service is currently available.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests use a controlled endpoint/transport callback, assert save-before-request order, empty-ID no-request behavior, exact URL construction with special characters, and malformed/nonobject JSON failures. Test persisted ID after a request failure.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No remote service was contacted. Modern service availability or rewards correctness cannot be inferred from this command source or RPC transport tests.

## Check your understanding

Why can the ID remain changed even though the command fails to return rewards details?

Pinned Java SHA-256: `a955fa8d051ef5595ca7f459f365d413ceaa54c7af28d718117b78401c772b66`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
