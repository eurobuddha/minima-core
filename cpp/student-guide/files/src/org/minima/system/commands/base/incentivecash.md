# incentivecash — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/incentivecash.java) · [Mapped C++ body](../../../../../../../../src/org/minima/system/commands/base/incentivecash.cpp) · [C++ header](../../../../../../../../src/org/minima/system/commands/base/incentivecash.hpp)

## Purpose

incentivecash optionally stores a user identifier, then asks the source’s configured remote rewards endpoint for details when an identifier is nonempty.

## Follow the command

1. If uid exists, trim/validate it, set UserDB’s incentive ID and save UserDB immediately.
2. Read the stored ID and put it in the response.
3. For a nonempty ID, concatenate it into the fixed HTTPS ping path and append the Minima version query, then call RPCClient.sendPUT.
4. Parse the reply and cast it to JSONObject before adding details. Invalid JSON/wrong top-level type/network failure propagates after a newly supplied ID may already be saved. The ID is concatenated without explicit URL encoding.

## Translation and dependencies

The native class reuses Command parameter/reply helpers, UserDB, MinimaDB.saveUserDB, RPCClient.sendPUT and JSONParser. Its protected requestPUT method defaults to the real RPC client; the test subclass only records the original URL and routes it to loopback. The registered command always uses the source HTTPS origin. The URL includes MINIMA_VERSION (1.1.2 here), not the separate full micro version (1.1.2.8).

Supplying uid: with no value does not clear the ID: getParam trims the supplied string and rejects a blank result before the setter runs. A missing or JSON-null stored value uses the empty-string default and skips HTTP. A JSON-null supplied parameter fails as a null String. These are different paths despite looking similar at the terminal.

## Understanding the tests

The [incentive lab](../../../../../../../new-tests/incentive-cash.md) was written before running the new test. Its [driver](../../../../../../../../tools/incentive_cash_differential.py) runs the pinned Java command and native counterpart with the existing wallet fixture. Both use actual UserDB files and a real loopback HTTP server. Java routes its URL connection; native overrides requestPUT and calls the real default RPC implementation with the routed destination.

Each request records the exact original HTTPS URL, current preferences and already-written file bytes. A fresh UserDB then reloads the file. This proves save-before-request order and persistence after HTTP/JSON errors. Disabling saving leaves memory changed while the old file survives. A nonempty directory blocking userprefs.db forces a real save error; the source logs it and continues to HTTP.

The corpus includes raw special-character IDs, wrong parameter/stored types, blank IDs, JSON null/arrays/scalars, malformed responses, HTTP status failures, repeated calls and runner permissions. It compares response data, exact serialized file bytes, request method/path/headers/body, logs and exception categories. Only explicitly listed runtime diagnostic wording and the test's substituted origin are normalized. Literal service-response strings are preserved.

Initial tests corrected assumptions about blank IDs, version suffixes and JSON control characters. They also exposed two shared native gaps: tab URL handling and direct-save file-open logging. Evidence and final results are recorded with the lab.

## Limits

No remote service was contacted. Modern service availability or rewards correctness cannot be inferred from this command source or RPC transport tests.

## Check your understanding

Why can the ID remain changed even though the command fails to return rewards details?

Pinned Java SHA-256: `a955fa8d051ef5595ca7f459f365d413ceaa54c7af28d718117b78401c772b66`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
