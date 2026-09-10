# jniminingtest — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/jniminingtest.java) · `src/org/minima/system/commands/base/jniminingtest.cpp` (absent)

## Purpose

jniminingtest performs a Java nonce search and a JNI search from comparable header bytes, then reports both outcomes and timings without asserting they agree.

## Follow the command

1. Parse maxattempts with default one million, target and testnonce; prepare two header byte copies using the wide initial nonce.
2. Java search copies BigInteger nonce bytes at offset four, increments the counter, hashes the current bytes and stops at the first hash strictly below target. Decode the successful modified header to obtain its actual MiniNumber nonce; if none succeeds the result nonce remains zero.
3. Set that Java nonce into TxPoW and calculate its ID even when no nonce was found, then save the Java ID for reporting.
4. Restore the wide nonce on TxPoW, initialize the untouched native header bytes, call sayHello and time hashHeaderWithDiff. Decode its returned nonce, set it and independently calculate its final hash/target success.
5. Return separate java/jni objects: Java includes attempts/found/nonce/ID, JNI includes found/nonce/ID, and each has elapsed milliseconds. No branch raises a mismatch error.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse exact nonce/header representation and Crypto. The JNI implementation remains a separate missing dependency; Java’s reported attempt counter increments before the successful test completes.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests use fixed headers and easy/hard targets to check zero/one/multiple attempts, no-hit reporting, strict comparison, malformed JNI return bytes and intentionally disagreeing Java/native outcomes. Assert both records instead of treating a returned JSON object as proof of parity.

- [Missing native function bodies](../../../../../../../../../upstream-study/src/org/minima/utils/jni/jnifunctions.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

Timings differ naturally and result fields can describe a default zero nonce after no hit. The source reports differences rather than failing a test.

## Check your understanding

Why is java.attempts one when the first zero nonce succeeds, and why can java.txpowid still be populated when java.found is false?

Pinned Java SHA-256: `c87bcae3173abad172c140740c56e7aeb7546639e401fd230dfac048e05ec5c4`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
