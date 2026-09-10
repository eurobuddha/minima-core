# decryptbackup — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/decryptbackup.java) · `src/org/minima/system/commands/backup/decryptbackup.cpp` (absent)

## Purpose

decryptbackup removes backup encryption and GZIP compression, leaving the concatenated length-framed payload for manual inspection.

## Follow the command

1. Read required file/default password, require the input to exist, choose an output name and delete/create the output before reading the input bytes. Choosing the same input and output path can therefore destroy the input before decoding.
2. Read MiniData salt and IV, derive the password key and create a decrypting cipher stream. Construct GZIPInputStream; exceptions at that construction are reported as Incorrect Password, even when caused by another format problem.
3. Copy decompressed bytes in 16384-byte chunks while read returns greater than zero. Exceptions inside this copy loop are swallowed, allowing a partial output to proceed to flush/close and possibly a success response.
4. Close streams and return input/output paths and sizes. The private section/TxPoWList helpers are not called by runCommand; the output retains the MiniData section framing.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse GenerateKey, MiniData headers and GZIP behavior. Preserve and document failure timing separately from any future hardening decision; an atomic validated export would have different side effects.

## Understanding the tests

The runner fixture establishes name classification only. Proposed direct tests use fixed encrypted fixtures, wrong password, malformed headers, truncation during copy, same input/output path and write failures. Assert partial-output bytes and whether the exception was swallowed, not merely the returned status.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No file was decrypted. A success response is not proof that all six backup sections are complete, because the command neither parses them nor propagates every copying failure.

## Check your understanding

Why can an output file exist even when the password is incorrect?

Pinned Java SHA-256: `bfa3112e48bedd5709a710467dd8d239ce7c2332dfa4490910978fb7f9f02d7a`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
