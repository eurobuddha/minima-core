# decryptbackup — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/decryptbackup.java) · [Mapped C++ body](../../../../../../../../src/org/minima/system/commands/backup/decryptbackup.cpp)

## Purpose

decryptbackup removes backup encryption and GZIP compression, leaving the concatenated length-framed payload for manual inspection.

## Follow the command

1. Read required file/default password, require the input to exist, choose an output name and delete/create the output before reading the input bytes. Choosing the same input and output path can therefore destroy the input before decoding.
2. Read MiniData salt and IV, derive the password key and create a decrypting cipher stream. Construct GZIPInputStream; exceptions at that construction are reported as Incorrect Password, even when caused by another format problem.
3. Copy decompressed bytes in 16384-byte chunks while read returns greater than zero. Exceptions inside this copy loop are swallowed, allowing a partial output to proceed to flush/close and possibly a success response.
4. Close streams and return input/output paths and sizes. The private section/TxPoWList helpers are not called by runCommand; the output retains the MiniData section framing.

## Translation and dependencies

The mapped command now compiles and is registered. It reuses Command's parameter
and reply helpers, MiniData framing, MiniFile path/read behavior, MiniFormat sizes,
and GenerateKey's PBKDF2/AES implementation. GzipInput now accepts a callback in
addition to its unchanged file input. CipherInput exposes the existing Cipher's
output in the same 8192-byte input batches as the tested JDK. Gzip reads 512-byte
compressed chunks and the command copies up to 16384 plain bytes per call.
BufferedFileOutput follows the existing MiniFile/nodecount POSIX writer pattern
with Java's ordinary-thread 8192-byte buffer and explicit flushing.

The C++ object owns its file descriptor with RAII. An exceptional exit closes
that descriptor without silently flushing pending data. Java may retain unclosed
stream resources until cleanup; native resource lifetime is not a JVM emulation.
The unused private section/TxPoWList helpers are also present, forwarding to the
existing framing and serialization code.

## Understanding the tests

The [Java fixture](../../../../../../../../support/DecryptBackupOracle.java)
executes the unchanged pinned command and generates fixed-salt/IV encrypted
fixtures with the actual Java provider. The [native fixture](../../../../../../../../support/decrypt_backup.cpp)
executes the mapped class. The [driver](../../../../../../../../tools/decrypt_backup_differential.py)
compares command metadata, fresh function parameters, direct/registry replies,
exceptions, directories, and every byte of every fixture file.

Both normal and ASAN/UBSAN decryption runs pass 422 cases. Valid plaintext is
independently required for empty, random, repeated and large payloads, including
Unicode and embedded-NUL passwords. Other cases cover wrong passwords, malformed
salt/IV frames, same input/output paths, existing outputs, directory errors,
truncated ciphertext, padding errors, gzip damage, concatenated members and
trailing data. Exactly 113 damaged cases report success with nonempty partial
output, demonstrating the source behavior rather than assuming success means a
complete backup. For truncated ciphertext, returned bytes must remain a prefix
of the original plaintext.

Only EOF/NPE/class-cast diagnostic prose is normalized for direct exceptions.
Other compared exceptions and output bytes remain exact. Empty salt now carries
the PBEKeySpec diagnostic and invalid IV size carries the cipher's checked
InvalidAlgorithmParameterException category. The broader utility crypto tests
still compare most provider failures as ERROR; this command adds more precise
evidence for those two paths.

The initial test failure was a fixture path assumption: MiniFile does not apply
BASE_FILE_FOLDER to names containing a slash. The corrected nested output is an
explicit path inside the temporary test directory. The initial report is saved
in [checkpoint evidence](../../../../../../../../validation/decrypt-backup-work-in-progress).
See the [lab](../../../../../../../new-tests/decrypt-backup.md) for the plan written
before the test was implemented.

## Limits

A success response is not proof that all six backup sections are complete,
because the command neither parses them nor propagates every copying failure.
Virtual-thread buffering, resource exhaustion, special-device write failures,
private unused helpers and full platform/exception parity remain unverified.

## Check your understanding

Why can an output file exist even when the password is incorrect?

Pinned Java SHA-256: `bfa3112e48bedd5709a710467dd8d239ce7c2332dfa4490910978fb7f9f02d7a`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
