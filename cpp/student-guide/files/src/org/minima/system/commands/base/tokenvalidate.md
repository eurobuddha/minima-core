# tokenvalidate — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/tokenvalidate.java) · `src/org/minima/system/commands/base/tokenvalidate.cpp` (absent)

## Purpose

tokenvalidate independently checks a token’s optional creator signature and optional web file against its token ID.

## Follow the command

1. Reject tokenid exactly 0x00, then scan known tokens for an exact canonical-ID string match. Parse the selected name as a JSON object; a nonobject or malformed name fails before validation results are assembled.
2. Only when both signature and signedby fields exist, decode Signature and compare its root public key to signedby. A mismatch produces valid:false with a reason without running signature verification.
3. When roots agree, initialize TreeKey with that public key and verify the signature over the token coin ID, not the entire description. Missing either field produces signed:false.
4. If webvalidate exists, read its URL string and fetch it with RPCClient.sendGET. Trim the response and require exact equality with the token’s canonical ID. Download/trim exceptions become web valid:false and a reason; URL field casting occurs outside that try.
5. Return separate signature and web records. A false validation boolean does not change the common command status to false.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse Signature wire decoding, TreeKey verification and RPCClient GET behavior. Preserve the signed message (coin ID) and the distinction between command failure and a negative validation result.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed fixtures cover missing fields, malformed signatures, mismatched roots, wrong signed message, valid signature and nonobject metadata. A controlled transport should return whitespace-wrapped matching ID, wrong case/ID and exceptions. Assert that invalid signature does not prevent the subsequent web check when decoding succeeds.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No URL was fetched. These checks establish particular metadata relationships, not an endorsement of the creator, token value or contract safety. Description fields themselves are not all signed by this verifier.

## Check your understanding

Why can status remain true while both signature.valid and web.valid are false?

Pinned Java SHA-256: `219a43c1a314215b702f2283f194e314de04439494917aa0b5ac70a996448770`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
