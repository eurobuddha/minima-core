# connect — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/connect.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java) | [src/org/minima/system/commands/network/connect.cpp](../../../../../../../../src/org/minima/system/commands/network/connect.cpp) |

## What this file does

Parses a requested peer endpoint and posts a connection request to the NIO manager.

## Follow the logic

1. Read host directly from the parameter object and cast it to String. Missing/null host throws the checked message No host specified; a wrong value type has a distinct cast failure.
2. createConnectMessage splits at the first colon, trims the two pieces using Java String.trim and parses the entire remaining portion as a signed int.
3. Missing colon or an unparsable port returns null. There is no port-range or nonempty-host validation here. Multiple-colon IPv6 text generally reaches the integer parser with extra colons.
4. Build an NIO_CONNECT Message containing host and port, post it to Main’s NIO manager, then return an Attempting to connect message. Posting is asynchronous intent, not confirmation of a successful socket connection.

## How the translation is approached

Native code reuses the Java-compatible integer parser and explicit trim boundary, returning a shared Message. A required postMessage callback supplies runtime integration. Java checked command errors are represented separately from generic standard failures.

## What the tests tell us

The [startup oracle](../../../../../../../../support/NetworkStartupOracle.java), [native harness](../../../../../../../../support/network_startup.cpp) and [driver](../../../../../../../../tools/network_startup_differential.py) test missing/wrong-type hosts, first-colon behavior, whitespace, Unicode integer digits, int bounds, negative/out-of-range socket ports, posting failures, command-runner sequences and metadata. They compare parsed message content and posted effects, never open a peer connection. The inspected startup report has 532 matching cases across connect and Greeting work.

## Dependencies and reading map

Direct Minima imports: [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java).

Method navigation (declarations with bodies found in this file): [connect](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java#L14), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java#L19), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java#L37), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java#L42), [createConnectMessage](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java#L64), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java#L90).

## Check your understanding

Why does a syntactically valid port of -1 pass createConnectMessage? Where must a real connection attempt enforce the socket’s valid range?

## Limits and traceability

Full network success, exact default-null optional errors and asynchronous failure delivery are outside this command fixture. The counterpart remains partial and parity not_verified.

Pinned Java SHA-256: `c7f635cf1cae6f26d1cf3592295a270965973e8c26e7346ad090980d2ab638b3`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
