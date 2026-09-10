# ping — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/ping.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/ping.java) | [src/org/minima/system/commands/network/ping.cpp](../../../../../../../../src/org/minima/system/commands/network/ping.cpp) |

## What this file does

Sends one protocol ping and formats its returned Greeting as a command reply.

## Follow the logic

1. Read the required host through Command, which trims strings and rejects missing, blank, null or wrong-type values.
2. Find the first colon. A host without one returns null immediately, before looking up Main. This differs from a status=false object.
3. Trim both pieces and parse the complete port string as a signed Java int, including supported Unicode digits. Bad syntax or overflow becomes CommandException with Invalid port. A valid int outside the socket range fails later in transport.
4. Evaluate Main.getInstance().getNIOManager(), then call the static sendPingMessage(ip, port, false). A missing Main or network fails before transport; a null NIO return does not itself prevent a static call.
5. Return response.host, response.port and response.valid. When a Greeting exists, also include its version and nullable extra data. Transport failure produces valid=false inside the otherwise successful command reply.

## How the translation is approached

Native ping reuses Command, JavaStringTrim, JavaInteger, Main and the actual NIOManager transport. The no-argument Command.runCommand API now returns the existing JSONValue, which represents an object or null. All existing concrete command bodies keep building their previous JSONObject replies. Greeting.getExtraDataOptional exposes the already-stored optional JSON without dereferencing it, preserving a null extra-data value in this command. The sibling Greeting stores raw JSON text and a different default version, so it does not replace this pinned decoder.

## What the tests tell us

The [ping driver](../../../../../../../../tools/ping_command_differential.py) runs 95 programs through the actual Main/command fixtures, checking metadata, fresh prototypes, typed parameters, first-colon parsing, integer boundaries, ownership and runner behavior. Nine isolated loopback cases exercise normal, fragmented, null/nested extra data, ignored type, trailing bytes, empty/type-only frames and truncation. The shared [transport fixture](../../../../../../../../tools/nio_manager_transport_differential.py) independently asserts the exact SINGLE_PING plus ZERO_TXPOWID request bytes. Successful fields and failed-reply omissions are asserted separately from Java/C++ equality. The [runner error driver](../../../../../../../../tools/command_runner_errors_differential.py) expands to 1,680 cases, including null replies and absent/null/wrong/false status; it checks escaped exception categories and that later commands do not run. See the [lab](../../../../../../../new-tests/nullable-command-replies.md) for final build results.

## Dependencies and reading map

Direct Minima imports: [Greeting](../../../../../../../../../upstream-study/src/org/minima/objects/Greeting.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [ping](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/ping.java#L14), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/ping.java#L19), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/ping.java#L30), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/ping.java#L35), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/ping.java#L75).

## Check your understanding

Why does a direct ping with host:example return null, while the same command through runMultiCommand throws? Why can valid=false still appear inside a reply whose status is true?

## Limits and traceability

These finite tests do not certify a complete node. Socket/wire diagnostic prose and enhanced Java null/cast prose are excluded; log count/order and exception categories remain compared. Full startup, every malformed Greeting branch, logger failures inside NIO and exhaustive mutable-reference behavior remain open. The counterpart is partial and parity not_verified.

Pinned Java SHA-256: `9d9aa3528645f5d05d0c313b34d6f45c9cd311dae7795ca9e806db4bd9c05065`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
