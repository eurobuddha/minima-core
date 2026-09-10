# sendview — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file compiles; complete API and node integration remain unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/sendview.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendview.java) | [src/org/minima/system/commands/send/sendview.cpp](../../../../../../../../src/org/minima/system/commands/send/sendview.cpp) |

## What this file does

Read a serialized TxPoW transaction file and return its JSON for inspection.

## Follow the logic

1. Require file and resolve it through MiniFile.createBaseFile, then read the complete byte array.
2. Wrap bytes as MiniData and decode using TxPoW.convertMiniDataVersion.
3. Return response.txpow as the decoded object’s JSON. The unused local response object has no effect.
4. The method does not explicitly recalculate the TxPoW ID, add signatures or call a miner. File and decoding failures escape to the command framework.

## How the C++ translation works

The C++ command reuses Command parameter/reply handling, MiniFile and the existing complete-TxPoW decoder. An empty optional decode result raises the native Java-null exception before JSON conversion. The nested response remains a snapshot of the decoded object.

## What the tests tell us

The offline-file fixture produces nine Java TxPoWs with zero, one or two ordinary and burn inputs. It compares complete JSON with the original fixture, then reads the signed files created by sendsign. Missing, NUL, directory and malformed paths, truncated bytes, trailing bytes, wrong parameter types and restricted callers exercise failure paths. Recursive file/directory observations detect parent creation even when opening fails.

See the [experiment and measured results](../../../../../../../new-tests/offline-send-files.md) and [actual corpus](../../../../../../../../tools/offline_send_corpus.py).

## Dependencies and reading map

Direct Minima imports: [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [sendview](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendview.java#L14), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendview.java#L19), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendview.java#L24), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendview.java#L44), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendview.java#L68).

## Check your understanding

Which operation determines whether a malformed file reaches toJSON?

## Limits and traceability

Full native Main startup and durable SQL storage remain absent. The fixture compares isolated command behaviour, not network acceptance or every inherited Java API. Filename clocks and independently random vault ciphertext are normalized as described in the lab. Full parity remains not_verified.

Pinned Java SHA-256: `389dc3992dca47b6872838a5197996ebbc276ea87838ab1801e5f80315387d6c`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
