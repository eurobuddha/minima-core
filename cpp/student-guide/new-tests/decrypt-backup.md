# Lab: decrypting a backup, including damaged files

Written before the new tests. Follow the pinned decryptbackup.java body. It
validates the password, resolves the input, creates/truncates the output, then
reads salt and IV frames. PBKDF2 derives the AES key; CipherInputStream feeds
GZIPInputStream. Failure constructing gzip becomes Incorrect Password!, while
exceptions inside the payload-copy loop are swallowed. The later flush still
runs. A successful reply can therefore describe partial output.

Reuse GenerateKey, MiniData, MiniFile, MiniFormat and the existing GzipInput
decoder. Extend the decoder to accept a reader callback, preserving its file
caller. Reuse the MiniFile/nodecount POSIX open/write pattern for the buffered
output; source buffering and stream chunk boundaries affect failure timing.
The installed JDK 26 source supplies the platform-stream behavior to compare.

Test the unmodified Java command and C++ counterpart against identical files:
password/parameter gates, default and explicit output, existing files/directories,
same input/output, malformed salt/IV frames, valid empty/large/Unicode payloads,
wrong passwords, padding damage, truncated/corrupt/concatenated gzip and partial
output. Compare reply/exception categories and complete output bytes. Test direct
calls and the real command registry. Preserve the first mismatch and explain any
normalization rather than hiding differences. Re-run utility and raw archive
checks because their existing cipher/gzip building blocks are reused.


## Results and the first correction

The command matches 422 Java cases in normal and ASAN/UBSAN builds. Of these,
113 damaged ciphertext cases return success with a nonempty partial file. The
comparison checks all output bytes, and truncated ciphertext must preserve a
prefix of its original plaintext. Valid empty/random/large plaintext is required
independently so identical failure is not mistaken for a valid decryption.

The first failure was in the observer: nested/output.bin does not live under
BASE_FILE_FOLDER, because MiniFile treats slash-containing names as explicit
paths. The fixture now uses an explicit path inside its temporary directory. The
source behavior was retained, the stray test output was checked and removed,
and the initial report is saved.

The reader callback is an addition to the existing GzipInput, not a replacement
for its file path. CipherInput reuses GenerateKey's withholding of the final CBC
block, consumes up to 8192 encrypted bytes per refill, and only finalizes at EOF.
Gzip's 512-byte reads can therefore expose data before a later padding/trailer
error. BufferedFileOutput retains 8192 pending bytes and flushes only where the
command asks or a buffer fills. Its destructor closes without flushing during
exception unwinding. This preserves file effects while using native RAII.

The comparison normalizes only direct EOF/NPE/class-cast diagnostic prose.
Empty salt and invalid IV now retain their precise Java validation meanings.
The source copy loop swallows ordinary exceptions, but Errors/allocation failures
must escape, so native catches exclude JavaError and std::bad_alloc.

Utility, raw-archive, command-runner, runner-error and launcher regressions are
included because these building blocks and command registration are shared.
The final review also clears consumed cipher output before a failing refill and
rejects callback byte counts beyond the supplied gzip buffer.

This evidence does not cover virtual threads, resource exhaustion, special-device
write failures or the two unused private decoding helpers. Full file/API parity
remains not_verified. See [checkpoint evidence](../../validation/decrypt-backup-work-in-progress).
