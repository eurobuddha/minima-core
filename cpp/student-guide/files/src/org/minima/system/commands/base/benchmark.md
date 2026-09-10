# benchmark — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/benchmark.java) · `src/org/minima/system/commands/base/benchmark.cpp` (absent)

## Purpose

benchmark times a fixed Java header-hashing loop and one JNI mining call. Despite computing a returned nonce and validity internally, the response exposes only hash count and the two timings.

## Follow the command

1. Parse hashes with default 1000, targetdifficulty and testnonce. maxattempts is in valid parameters but is not read here.
2. Create a TxPoW, fix its body hash, set a deliberately wide serialized nonce and serialize two copies of the header.
3. For the Java loop, start a BigInteger nonce at zero, copy its variable-width bytes into the header at offset four, increment it and hash the mutated bytes. Every iteration runs regardless of target difficulty, and the hash result is discarded.
4. For JNI, initialize the other header’s nonce bytes, call sayHello, time hashHeaderWithDiff(test nonce bytes, amount, target, header), decode the returned MiniNumber and recalculate TxPoW ID.
5. Compute whether the final hash is below target, but do not include that boolean or nonce in the response. Return hashes and java/jni millitime objects.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Existing Crypto/header/MiniNumber routines supply the Java-side data model; the actual JNI library body is absent from this mapping and cannot be invented from its declaration.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests use a controlled JNI boundary to assert exact argument bytes and amount, malformed returned nonce handling and response field omissions. Separate real-library tests must establish comparable work. Timings should not be asserted equal because Java always loops amount times while native mining may stop on success.

- [Declared native boundary](../../../../../../../../../upstream-study/src/org/minima/utils/jni/jnifunctions.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

This command is a benchmark/diagnostic, not a cross-language correctness assertion. A successful response does not report the internally computed validity result.

## Check your understanding

Why can comparing these two timings be misleading if the JNI function finds a valid nonce early?

Pinned Java SHA-256: `ce2e6240626b1dfe1d928357d193e1898754ea06dbd3e98dae1247e5b2d25486`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
