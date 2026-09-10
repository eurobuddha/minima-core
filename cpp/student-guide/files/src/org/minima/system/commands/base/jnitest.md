# jnitest — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/jnitest.java) · `src/org/minima/system/commands/base/jnitest.cpp` (absent)

## Purpose

jnitest prepares a serialized header for the native mining helper, decodes its returned nonce and independently evaluates the resulting TxPoW hash.

## Follow the command

1. Parse maxattempts with default 1000, difficulty with the Magic default, and a testnonce with a fixed decimal default. Serialize testnonce as MiniNumber bytes.
2. Create TxPoW, set body hash, select the wide START_NONCE_BYTES value and serialize its header. Copy BigInteger zero bytes into offset four without reserializing the full header.
3. Create jnifunctions, call sayHello, and time hashHeaderWithDiff with test nonce, limit, target and header.
4. Decode returned bytes through MiniNumber.ReadFromStream, set TxPoW nonce and calculate ID. Compare ID data strictly below target for success.
5. Return raw result bytes as signed decimal values with trailing commas, decoded nonce, elapsed time, target, ID and success. outputByteArray uses Java signed-byte text, not unsigned 0..255.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse the exact header/nonce serialization and hashing rules. This source provides caller expectations but not the missing native library’s attempt/mutation behavior.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed boundary tests assert serialized arguments, offset-four mutation, default/negative limits, signed-byte formatting, returned nonce truncation and independent strict target comparison. A real native-library fixture is required to verify the search rather than a mock result.

- [Native declarations](../../../../../../../../../upstream-study/src/org/minima/utils/jni/jnifunctions.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The command does not call loadNativeLib itself and has no equality assertion with another implementation. Successful nonce decoding does not prove the native helper obeyed its limit.

## Check your understanding

What additional check does recalculating TxPoW ID provide beyond parsing the returned bytes?

Pinned Java SHA-256: `0597b2ef98837644186606bbce57c0aa8949e4df3275a70b5e57ffa968f36fe1`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
