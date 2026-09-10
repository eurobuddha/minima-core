# MinimaRPCClient — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/MinimaRPCClient.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/MinimaRPCClient.cpp) · [C++ header](../../../../../../src/org/minima/utils/MinimaRPCClient.hpp)

## Purpose

This is the command-line RPC client, not the node entry point. It parses options, sends encoded command text to an RPC endpoint, formats JSON replies, and optionally reads commands interactively.

## Follow the algorithm

1. Initialize the default endpoint, username, password, SSL-key text, and empty one-shot command. Scan options left to right; repeated options overwrite earlier values. Help and unknown options print text and exit with status 1. Missing option values fail at array access.
2. The source SSL-context setup is commented out, so the local ssl flag remains false. Ensure the host ends with a slash. An empty command prints a banner through MinimaLogger. The bpass flag affects that banner; HTTP Basic auth still receives the configured password in either mode.
3. A one-shot command is form-URL-encoded in UTF-8 without trimming, appended to the host, sent using sendGETBasicAuth, parsed as a JSONObject, and pretty-printed. Only ParseException from parsing is caught here; wrong JSON top-level types are different failures.
4. Interactive mode reads lines. Nonempty lines are trimmed first; exit breaks before sending, while quit is sent and breaks only after successful reply handling. Other exceptions are logged and the loop continues. Null from EOF does not break the source loop.
5. After a normal interactive break, close the buffered reader and then the underlying reader. An IOException during those closes is logged.

## How the C++ translation works

Native main receives optional UTF-16 argument strings and returns an exit status for source System.exit paths. It uses the shared RPCClient, parser, pretty printer, logger, and SocketCharacterReader. Its form encoder preserves the Java rules: spaces become+, and unsafe UTF-8 bytes become percent escapes. Platform input/output and stack printing are explicit Runtime boundaries. The implementation preserves the inactive SSL setup and EOF loop rather than silently changing behavior.

## Understanding the tests

The dedicated driver compares actual HTTP request traces alongside stdout/stderr, exit/exception results, and close events. Cases include missing/null/repeated options, Unicode and malformed surrogate arguments, raw-byte stdin, empty/malformed/nonobject JSON replies, and HTTP statuses. Real subprocess tests check help/unknown-option statuses and prove a command is processed before EOF. Separate pipe and terminal tests verify that EOF does not exit and that terminal input can resume with exit. Timing/heap prefixes and automatic diagnostic wording are normalized, while presence of actual stack frames is required.

- [Java main fixture](../../../../../../support/MinimaRPCClientOracle.java)
- [Native main fixture](../../../../../../support/minima_rpc_cli.cpp)
- [Loopback, stdin, process, and terminal assertions](../../../../../../tools/minima_rpc_cli_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The test server is loopback and does not start MinimaCore. A working minima-rpc executable therefore does not establish a native node implementation. Native/JVM frame symbols differ and the source’s busy EOF behavior remains an intentional compatibility limitation. A useful focused regression is quit followed by a malformed JSON reply: the client should log the failure and continue to a later exit.

## Check your understanding

Why does -command " status " differ from entering the same text interactively? Why does a malformed response to quit keep the interactive loop alive?

Pinned Java SHA-256: `4f06512d6cc29c5698271b5d93254f955723cc92d60dd3f510b81d766fe71398`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
