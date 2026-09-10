# magic — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/magic.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/magic.cpp)

## What this file does

Read and optionally update the node’s desired network capacity parameters. Report the current tip’s Magic values alongside the node’s requested values.

## Follow the logic

1. Get UserDB and process optional kissvm, txpowsize and txnsperblock parameters in that order. Parse and write each before processing the next.
2. A later parse failure preserves earlier writes. Likewise, a missing chain tip fails only after all supplied preference writes have happened.
3. Obtain lastblock from the actual tip TxPoW’s Magic.toJSON. This is separate from desired: updating preferences does not rewrite an existing block.
4. Read the desired values back through UserDB. JsonDB stores decimal strings, but the reply contains numeric MiniNumber JSON values. Stored non-string values must produce the source ClassCastException; null still selects the default.
5. Return the reply without saving a file. Explicit save/load tests exercise inherited preference persistence separately. A subsequently generated TxPoW reads these desired values through the actual generator.
6. The pinned source includes this class but does not register it in CommandRunner. Direct tests instantiate the class; public command dispatch must continue to report magic as unavailable.

## How the translation is approached

The translation reuses Command number parsing, UserDB getters/setters, JsonDB serialization, the real chain tip and Magic JSON. JSONValue.raw carries the MiniNumber decimal representation as a number. A type guard in JsonDB.getNumber retains Java’s cast behavior before the existing MiniNumber constructor. No extra command registration or automatic save is introduced.

## What the tests tell us

[The lab written before these tests](../../../../../../../new-tests/node-state-commands.md) and [the executable corpus](../../../../../../../../tools/node_state_corpus.py) use the existing wallet fixture with actual command bodies, chain/MMR objects, RAM entries, preferences and recorded SQL. Both implementations receive the same source-generated wire. Exact reply text, current entry lists, relevant coin/MMR wire, preference wire and saved preference bytes are observed after each operation. SQL tree IDs are independently random: the comparison checks their 32-byte shape and consistent use, then labels them by encounter order. Other SQL bindings and effects remain exact. Independent assertions check counts, tracking results and generator preferences. See the lab for final run evidence and the mismatches that motivated shared fixes.

## Check your understanding

After kissvm:3000 succeeds and txpowsize:bad fails, is the first write undone? No. Does compiling magic.cpp make the command publicly available? No: source registry membership is a separate behavior.

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. Supplied SQL rows do not establish durable storage. Full Main startup, live chain/network integration, arbitrary retained collection aliases, concurrent mutation and exhaustive platform behavior remain unfinished.

Pinned Java SHA-256: `d694f807ce4c873883a1e2a894b8bce75b7680b053c0b4e542a71211dbf67572`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
