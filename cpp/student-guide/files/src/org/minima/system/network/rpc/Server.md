# Server — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/rpc/Server.java](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/Server.java) | [src/org/minima/system/network/rpc/Server.cpp](../../../../../../../../src/org/minima/system/network/rpc/Server.cpp) |

## What this file does

Define the minimal base contract for port-bearing servers: retain the configured integer and require a shutdown implementation.

## Follow the logic

1. Construction assigns the supplied port directly. No range validation or socket binding occurs in this class.
2. getPort returns the configured value, including zero when the caller requests an ephemeral operating-system port.
3. shutdown is abstract. Listener creation, actual bound-port discovery, handler lifecycle and cleanup belong to concrete subclasses.

## How the translation is approached

The C++ base retains a signed 32-bit port, a pure virtual shutdown and a virtual destructor for deletion through a base pointer. The native HTTPServer separately exposes the actual bound port; getPort continues to mean the configured port.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/http_server_differential.py). The listener fixture checks getPort for automatic and manual startup at port zero and for a bind collision. It separately observes the ephemeral bound port and normalizes that OS-chosen number. Thus a working bind does not change the base object’s configured port. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Method navigation (declarations with bodies found in this file): [Server](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/Server.java#L8), [getPort](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/Server.java#L12).

## Check your understanding

Why should getPort return zero after a successful ephemeral bind? Which observation would a test need to connect to that listener?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `02d2ed722d842e7a1edb8c491cd060ad180f5c7a07327e26e8651ef8cea4aeb4`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
