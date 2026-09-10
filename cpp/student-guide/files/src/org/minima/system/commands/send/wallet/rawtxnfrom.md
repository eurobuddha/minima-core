# rawtxnfrom — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/wallet/rawtxnfrom.java](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/rawtxnfrom.java) | [src/org/minima/system/commands/send/wallet/rawtxnfrom.cpp](../../../../../../../../../src/org/minima/system/commands/send/wallet/rawtxnfrom.cpp) |

## What this file does

Assemble an unsigned custom transaction from separate input-ID and script arrays, output objects and optional state.

## Follow the logic

1. Parse inputs and outputs as arrays and require the scripts array, then optional state as an object. Wrong types fail here; null arrays remain null until their loops.
2. Generate a random 32-byte ID and issue txncreate before entering any input loop.
3. For each input, cast to String and issue txninput. Complete all inputs before proceeding to outputs. Null input elements concatenate as the text null.
4. For each output object, read address then amount, optional tokenid (default 0x00), then optional Boolean storestate (default true). Null Boolean unboxing fails; the string true is not a Boolean. Issue txnoutput.
5. After all outputs, cast each script to String and issue txnscript. This differs from rawfrom’s alternating input/script sequence.
6. Traverse state keys in LinkedHashMap insertion order. Parse the integer port first, convert the value as Java object-to-string concatenation, construct StateVariable, and issue txnstate with normalized port/data.
7. Export with showtxn:true, delete, and return export.response. No child status is checked, no MMR command runs, and no signing occurs. A thrown exception can skip the final deletion.

## How the translation is approached

The native wrapper calls the existing CommandRunner through CommandResult.first. RawTxnCommands shares only the identical output and state loops. StateText is extracted unchanged from the proven send.cpp conversion: strings keep their text, doubles use Java formatting, and other JSON values use existing JSON text. JSONObject’s native insertion order matches the pinned LinkedHashMap. Typed casts preserve null-versus-wrong-type failure categories without substituting transaction construction logic.

## What the tests tell us

[Read the raw corpus](../../../../../../../../../tools/raw_transaction_corpus.py), [the shared Java/native driver](../../../../../../../../../tools/wallet_wrappers_differential.py) and [the lab](../../../../../../../../new-tests/raw-transaction-commands.md) compare exact child-call order/text, row bytes, metadata, failures and cleanup. Real children construct and export rows, including output storestate flags and normalized state. Independent assertions distinguish alternating versus separated script order, duplicate port normalization, retained rows on throws, and ignored input failures. The final raw group passes 656 programs and 3,991 child calls in normal and ASAN/UBSAN builds (4.06s/4.55s). After the Unicode Boolean correction, the object regression passes 9,464 cases per build (25.10s/91.34s). The earlier seven-check batches also pass, including funding and wallet-wrapper regressions.

## Dependencies and reading map

Direct Minima imports: [StateVariable](../../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Command](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandRunner](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [rawtxnfrom](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/rawtxnfrom.java#L15), [getValidParams](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/rawtxnfrom.java#L20), [getFullHelp](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/rawtxnfrom.java#L25), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/rawtxnfrom.java#L49), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/rawtxnfrom.java#L147), [getFunction](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/rawtxnfrom.java#L154).

## Check your understanding

Where does a null array fail relative to txncreate, and how does that differ from passing a JSON object where an array is required?

## Limits and traceability

The counterpart is implemented_partial and parity remains not_verified. Tests normalize only checked generated IDs and runtime-specific NPE/CCE prose; other observed bytes/replies/logs are compared. Exporting an unsigned row does not prove a funded or accepted transfer. Native Main, durable SQL, complete nullable/reference APIs and full node integration remain unfinished.

Pinned Java SHA-256: `22309c42c345ed9bde3e106ab20edcd7a761a255fe0884a54c69e5c6ae3633b4`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
