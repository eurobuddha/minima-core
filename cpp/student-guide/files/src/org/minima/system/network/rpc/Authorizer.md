# Authorizer — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/rpc/Authorizer.java](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/Authorizer.java) | [src/org/minima/system/network/rpc/Authorizer.cpp](../../../../../../../../src/org/minima/system/network/rpc/Authorizer.cpp) |

## What this file does

Parse the pinned Basic authorization format and return a JSON decision using the main password and configured RPC users.

## Follow the logic

1. Retrieve UserDB and count RPC users before entering the parsing try/catch. Missing database state can therefore escape instead of becoming a normal invalid-credentials reply.
2. Only auth style basic is supported. Search for the case-sensitive substring Basic anywhere in the header, decode everything after it with the Basic Base64 decoder, decode UTF-8 and trim Java control/space characters.
3. Split decoded text at its first colon. The remaining password may itself contain colons. Record username before checking credentials.
4. For username minima, authentication disabled or an equal configured password yields valid=true and mode=write. Failed main-user credentials return the minimal valid=false object.
5. Additional users are checked only when main authentication is enabled. Iterate all matching records without breaking, so a later duplicate can replace mode. An unmatched extra user retains username with valid=false; a caught malformed record discards that partial reply.
6. Parsing exceptions are logged and return the minimal rejection. Mode selection here does not prove that CMDHandler enforces read-only command execution.

## How the translation is approached

C++ reuses actual UserDB, MiniString and the checked Base64 adapter. The one-argument method now defaults to the current MinimaDB UserDB lookup on every call; the direct overload accepts the database explicitly for fixtures. Earlier isolated tests supplied this callback and hid its missing production default. The main password comparison now reads a locked optional-string snapshot. Java password.equals(null) is false; converting null to the text "null" would accidentally accept credentials and is deliberately avoided. The authentication flag uses atomic storage, preserving the source short circuit. Logging currently uses native diagnostic output and full exception/log parity remains open.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/network_utilities_differential.py). NetworkUtilityOracle and network_utilities compare exact decision JSON for prefixed/case-varied headers, padding errors, missing colons, Unicode credentials, duplicate users, null/wrong-type records and auth disabled. The driver also exhaustively varies short Base64 strings. It does not execute an RPC server or wallet command. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [UserDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/UserDB.java), [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [checkAuchCredentials](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/Authorizer.java#L15).

## Check your understanding

Why can a later malformed user record turn an earlier successful match into the minimal rejection object?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `a33c816e83c3de9954294b2d9faf7a8e4b0b14b4a82d738655b2d6ef5b57365e`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Live integration follow-up

The [live RPC lab](../../../../../../../../student-guide/new-tests/rpc-live-command.md)
first reproduced a missing production database binding, then passed in normal
and sanitizer builds after adding the source-equivalent default lookup. Replacing
MinimaDB while the listener stays active rejects the old extra user, proving
the lookup does not retain the original UserDB. The default password remains
a separate global setting. This establishes the tested live behavior only.
