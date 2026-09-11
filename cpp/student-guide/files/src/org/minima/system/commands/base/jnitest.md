# jnitest — native-library caller logic

**Teaching status:** Authored logic, translation and validation explanation; full parity remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/jnitest.java) · [C++ caller](../../../../../../../../src/org/minima/system/commands/base/jnitest.cpp)

## What the file does

Calls the external function, decodes its returned nonce, recomputes TxPoW ID and validity, and returns signed byte formatting. Defaults to 1,000 maximum attempts.

## Follow the logic and tests

The [JNI caller lab](../../../../../../../new-tests/jni-command-callers.md) walks through exact parameter defaults, reserved
nonce bytes, fixed offset four, serialization, strict target comparison, return
formatting and exceptions. It explains the 129 Java/native cases, including all
117 initial cases plus twelve target-mutation/cache checks, independent SHA3 ID
validation, full ordinary loops and precise normalization limits.

[Terminal results](../../../../../../../../validation/jni-command-callers-work-in-progress/progress.json).
The unchanged Java command runs against a recording JNI fixture; native uses an
explicit provider. Neither fixture proves the unavailable external algorithm.
The JNI library remains missing, and this command is not newly registered in the
runner because the pinned source does not register it. Full runtime parity is open.

Pinned Java SHA-256: `0597b2ef98837644186606bbce57c0aa8949e4df3275a70b5e57ffa968f36fe1`.
