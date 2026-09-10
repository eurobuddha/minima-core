# NotifyManager — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/webhooks/NotifyManager.java](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java) | [src/org/minima/system/network/webhooks/NotifyManager.cpp](../../../../../../../../src/org/minima/system/network/webhooks/NotifyManager.cpp) |

## What this file does

Queue notifications, call an in-process listener and deliver filtered copies to configured webhook URLs.

## Follow the logic

1. Construction loads a hook list from UserDB. PostEvent queues the supplied JSON object by reference, so changes before processing are visible.
2. addHook excludes a new empty string and duplicates, then saves the list even when nothing was added. removeHook and clearHooks also save after mutation. getAllWebHooks creates a detached string list and fails on null elements.
3. For NOTIFY_POST, retrieve the notification and capture its event string before invoking the optional listener. The listener receives the original Message and can mutate the JSON or replace the message’s notify field.
4. After the listener, serialize the originally captured notification object and take a hook snapshot. Listener edits to that object affect the body; the previously captured event still controls filtering.
5. Split each hook at its first #. A hook without # returns from the entire delivery operation, preventing later hooks from running. Check running before each send; an empty filter matches every event, otherwise use case-sensitive substring matching.
6. A listener Exception is logged and delivery normally continues; an error while logging can itself interrupt it. A POST Exception is optionally logged with notifications disabled to avoid recursive notification logging. Snapshot hooks isolate later hook-list changes during delivery.

## How the translation is approached

C++ reuses MessageProcessor, shared Message/JSONObject values, UserDB UTF-16 hook conversion and RPCClient. Required listener and service receivers make the still-unfinished Main integration explicit. A recursive hook lock permits inspected callback reentry; atomic stop/log flags prevent data races without claiming every Java interleaving.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/notify_manager_differential.py). NotifyManagerOracle and notify_manager compare FIFO retained message identity, mutations before processing, listener edits/replacement/errors, hook snapshots, malformed-hook early return, stop, database rebinding/failure and UTF-16 filters including surrogate halves. Java’s actual RPCClient meets a controlled HttpURLConnection; native delivery uses a controlled receiver. This is delivery logic evidence, not a claim that real external webhook requests were sent. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [RPCClient](../../../../../../../../../upstream-study/src/org/minima/utils/RPCClient.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageListener](../../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageListener.java), [MessageProcessor](../../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java).

Method navigation (declarations with bodies found in this file): [NotifyManager](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java#L28), [shutDown](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java#L35), [PostEvent](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java#L43), [getAllWebHooks](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java#L49), [addHook](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java#L61), [removeHook](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java#L72), [clearHooks](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java#L81), [processMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java#L91).

## Check your understanding

A listener changes event from MINING to SECOND. Which value chooses filters, and which value appears in the posted JSON? Trace the two capture points.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `346c61dbbd104d47d0b6583fb7f6a96215bdcf92f5e74802ab18dec0f6e0f4b7`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
