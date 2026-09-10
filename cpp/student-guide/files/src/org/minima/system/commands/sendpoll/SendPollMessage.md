# SendPollMessage — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/sendpoll/SendPollMessage.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollMessage.java) | [src/org/minima/system/commands/sendpoll/SendPollMessage.cpp](../../../../../../../../src/org/minima/system/commands/sendpoll/SendPollMessage.cpp) |

## What this file does

Keep a stable queue UID and command string for one retryable send request.

## Follow the logic

1. The one-argument constructor creates sixteen random bytes and stores their 0x string as UID, retaining the supplied command.
2. The two-argument constructor accepts the supplied UID and command, including null; it generates no replacement identity.
3. toJSON writes uid followed by command, preserving nulls and JSON escaping.
4. copy creates a separate message with the same two string references/values. Copying a queue snapshot does not assign new UIDs.

## How the translation is approached

C++ uses optional strings to represent Java null and value copies to preserve immutable string semantics. It reuses MiniData random generation and actual JSONObject rendering; independent entropy is not expected to match byte for byte.

## What the tests tell us

[Read the native polling checks](../../../../../../../../support/sendpoll.cpp). The harness checks exact null JSON, escaped Unicode command copy equality, sixteen-byte UID width and uniqueness among 1000 samples. This sample uniqueness check does not prove collision impossibility or Java entropy equivalence. Those checks are native assertions. The new [command comparison](../../../../../../../../tools/sendpoll_command_differential.py) also runs 367 programs and 3,216 operations against compiled Java in normal and ASAN/UBSAN builds. It checks stable UID shape/identity, copied queue snapshots, additions/removals and actual worker success/failure through CommandRunner. Timer publication is recorded; delivery and logs are outside that comparison.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [SendPollMessage](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollMessage.java#L11), [SendPollMessage](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollMessage.java#L16), [getUID](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollMessage.java#L21), [getCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollMessage.java#L25), [toJSON](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollMessage.java#L29), [copy](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollMessage.java#L38).

## Check your understanding

Why must copying a retry entry preserve its UID?

## Limits and traceability

The mapped file is partial and parity remains not_verified. The inspected tests have a finite native scope and do not certify full wallet command execution or every thread schedule.

Pinned Java SHA-256: `487af02407c2bcf7aa05abff331d48a99fad2419c547bb490df0498802f7edba`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
