# jniminingtest — native-library caller logic

**Teaching status:** Authored logic, translation and validation explanation; full parity remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/jniminingtest.java) · [C++ caller](../../../../../../../../src/org/minima/system/commands/base/jniminingtest.cpp)

## What the file does

Runs the ordinary search until a valid hash or the maximum, decodes its raw edited header to recover the fractional nonce, then compares both ordinary and external results. Defaults to 1,000,000 attempts.

## Follow the logic and tests

The [JNI caller lab](../../../../../../../new-tests/jni-command-callers.md) walks through exact parameter defaults, reserved
nonce bytes, fixed offset four, serialization, strict target comparison, return
formatting and exceptions. It explains the 129 Java/native cases, including all
117 initial cases plus twelve target-mutation/cache checks, independent SHA3 ID
validation, full ordinary loops and precise normalization limits.

[Terminal results](../../../../../../../../validation/jni-command-callers-work-in-progress/progress.json).
The unchanged Java command runs against a recording JNI fixture; native uses an
explicit provider. Neither fixture proves the unavailable external algorithm.
The JNI library remains missing. This command is registered in the pinned
CommandRunner.java and now in the native registry. The earlier claim that the
source did not register it was incorrect. Full runtime parity is open.

Pinned Java SHA-256: `c87bcae3173abad172c140740c56e7aeb7546639e401fd230dfac048e05ec5c4`.
