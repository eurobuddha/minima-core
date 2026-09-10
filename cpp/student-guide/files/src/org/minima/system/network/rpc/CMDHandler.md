# CMDHandler — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/rpc/CMDHandler.java](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/CMDHandler.java) | [src/org/minima/system/network/rpc/CMDHandler.cpp](../../../../../../../../src/org/minima/system/network/rpc/CMDHandler.cpp) |

## What this file does

Handle exactly one socket request, turn its path or POST body into a command string, and write the pinned HTTP-style response.

## Follow the logic

1. Read the first line as text, tokenize method/path, uppercase the method, remove at most one leading and trailing slash, URL-decode the path, then trim it. Even methods other than GET/POST proceed through this parser.
2. Default authorization is write access, but authentication enabled or a nonempty RPC user list makes it invalid until an Authorization header succeeds. Header matching uses case-sensitive substring searches, and later matching headers replace earlier values.
3. On rejection write the 401 body then throw into the outer request logger. Otherwise POST allocates contentlength characters and reads UTF-16 character units, not raw byte counts. A short body retains zero-filled unused characters.
4. Run CommandRunner on the selected string. One result is unwrapped; zero or multiple results remain an array. A caught command exception leaves the default status=false JSON. The read/write mode enforcement block is commented out in the pin.
5. Write 200 headers with optional CRLF, UTF-8 content byte length and a final body newline that is not included in that length. Close the request resources afterward.
6. An exact quit command sets a flag before execution and calls process halt after cleanup. The ordinary response suite deliberately does not establish this process-lifecycle path.

## How the translation is approached

C++ reuses SocketCharacterReader for incremental UTF-8/UTF-16 behavior, JavaURLDecoder, integer parsing and CommandRunner. Socket shared ownership replaces separate Java stream/socket objects. PrintWriter-style output IO failures are swallowed. Full cleanup failure order, logger text and Main quit integration remain review boundaries.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/cmd_handler_differential.py). The experiment uses actual Java/native HTTPServer and CMDHandler over TCP and actual mapped commands. It varies LF/CR/CRLF, malformed URL/UTF-8, duplicate/case-varied headers, authentication configurations, short/long UTF-16 body counts and multibyte fragments around the reader boundary. Response bytes and lengths are compared after validating and normalizing only the wall-clock Date. Some diagnostic details are compared by failure site rather than exact text. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [CommandRunner](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [CMDHandler](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/CMDHandler.java#L37), [run](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/CMDHandler.java#L43).

## Check your understanding

For POST body π😄, compare UTF-8 bytes with UTF-16 units. Why would reading Content-Length as bytes silently change this pinned behavior?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `6650ffe507498a8f647d85eb5b59284a0f9e538502304dff617cdf67ef95dc4f`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
