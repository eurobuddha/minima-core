# NIOClientInfo — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/minima/NIOClientInfo.java](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java) | [src/org/minima/system/network/minima/NIOClientInfo.cpp](../../../../../../../../src/org/minima/system/network/minima/NIOClientInfo.cpp) |

## What this file does

Expose a mostly copied connection summary while retaining selected live access to the underlying NIOClient.

## Follow the logic

1. The explicit UID/host/port constructor marks connected true but stores no underlying client. Welcome, advertised port and validity keep their defaults.
2. The client-based constructor retains the client reference and copies welcome, greeting validity, host, ports, UID, incoming flag and the supplied connected flag.
3. Ordinary metadata getters use copied fields. getTimeConnected and getExtraData delegate to the live client, and setExtrasData modifies that same client.
4. toJSON writes copied metadata and includes a formatted connected timestamp only when a client exists. Thus a later host change can be invisible while a later connection-time change is visible.
5. An explicit summary can serialize normally but its live-delegating getters fail because no client was supplied.

## How the translation is approached

C++ uses shared ownership for the retained client and optional strings for nullable copied fields. This is intentionally neither a wholly frozen snapshot nor a wholly live view. Returning value copies of NIOClientInfo does not sever the underlying shared client.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/nio_client_differential.py). The client experiment repeatedly creates summaries, mutates original client metadata and timestamps, and writes extras through the summary. It captures summary JSON, live time/extras and copied validity separately. Explicit and null-client construction cases identify operations that do or do not require the underlying client. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L33), [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L41), [getUID](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L53), [isIncoming](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L57), [isConnected](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L61), [getHost](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L65), [getPort](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L67), [getMinimaPort](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L71), [getTimeConnected](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L75), [getExtraData](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L79), [setExtrasData](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L83), [toJSON](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L87), [ismValidGreeting](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java#L105).

## Check your understanding

After creating an info object, change client host, validity and extra data. Which info observations change without constructing another info object?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `3de24bdb8e23452c2b56f8ab664323378bfdb0dfaadc17a50bccb1058ebab3da`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
