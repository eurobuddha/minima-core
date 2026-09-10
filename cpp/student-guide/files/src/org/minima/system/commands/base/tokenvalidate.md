# tokenvalidate — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/tokenvalidate.java) · [Native C++](../../../../../../../../src/org/minima/system/commands/base/tokenvalidate.cpp)

## Purpose

tokenvalidate independently checks a token’s optional creator signature and optional web file against its token ID.

## Follow the command

1. Reject tokenid exactly 0x00, then scan known tokens for an exact canonical-ID string match. Parse the selected name as a JSON object; a nonobject or malformed name fails before validation results are assembled.
2. Only when both signature and signedby fields exist, decode Signature and compare its root public key to signedby. A mismatch produces valid:false with a reason without running signature verification.
3. When roots agree, initialize TreeKey with that public key and verify the signature over the token coin ID, not the entire description. Missing either field produces signed:false.
4. If webvalidate exists, read its URL string and fetch it with RPCClient.sendGET. Trim the response and require exact equality with the token’s canonical ID. Download/trim exceptions become web valid:false and a reason; URL field casting occurs outside that try.
5. Return separate signature and web records. A false validation boolean does not change the common command status to false.

## Translation and dependencies

The mapped native counterpart compiles and is registered. It reuses Command, token search, Signature wire decoding, TreeKey verification, JSON parsing and RPCClient GET behavior. The signed message remains the coin ID. Command failure and a negative validation result remain distinct.

## Understanding the tests

Both builds pass 126 Java/C++ programs / 254 operations, including 33 real loopback HTTP requests per runtime. The [student lab](../../../../../../../../student-guide/new-tests/token-validation.md) explains missing/typed fields, malformed signatures, mismatched roots, wrong signed messages, valid signatures and nonobject metadata. HTTP checks include matching/mismatching text, whitespace, line removal, redirects, status errors and failed connections. Invalid decoded signatures still permit the web check; decoding errors stop it.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

The [saved results](../../../../../../../../validation/token-validation-work-in-progress/progress.json) retain the expectation correction and null-import defect as well as passing comparisons.

## Limits

Only controlled loopback URLs were fetched. These checks establish particular metadata relationships, not an endorsement of the creator, token value or contract safety. Description fields themselves are not all signed by this verifier.

## Check your understanding

Why can status remain true while both signature.valid and web.valid are false?

Pinned Java SHA-256: `219a43c1a314215b702f2283f194e314de04439494917aa0b5ac70a996448770`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
