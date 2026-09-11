# Testing the Java/text AES helper

This lab is written before the new AesUtil tests run. Results will be recorded
after verification; a proposed check is not a passing check.

Read [AesUtil.java](../../../upstream-study/src/org/minima/utils/encrypt/javajs/AesUtil.java).
It turns a password into an AES key, encrypts text, and prints the ciphertext in
Base64. Salt and IV arrive as hexadecimal strings. These are three separate
conversions: changing any one changes the result even when both programs say AES.

We reuse the native Cipher, MiniData hex parser, Base64 functions, and Unicode
converters. GenerateKey.secretKey cannot be reused: it fixes SHA-256 and 65536
iterations, while this class specifies SHA-1 and caller-selected iterations.

The oracle compiles the pinned Java class and calls its actual public methods.
Reflection also observes its private key and cipher helpers without replacing
their algorithms. The C++ fixture uses a friend accessor for the same observation.
Each program keeps one object across multiple operations, so a failed decryption
followed by successful encryption tests recovery of mutable cipher state.

Inputs carry UTF-16 code units, allowing lone surrogates as well as ordinary
Unicode. UTF-8 replaces a lone surrogate with a question mark when encoding;
malformed decrypted UTF-8 becomes the replacement character when decoding.
Passwords always use UTF-8. Plaintext uses Java's default charset, which is UTF-8
for the supported native/JDK configuration. Other default charsets need separate
runtime-wide work and are not certified by this lab.

We will compare exact ciphertext, key bytes, decoded bytes, null results and
exception classes/messages (excluding VM-dependent null-pointer prose). Cases
include empty text, block boundaries, all AES key sizes, non-byte-aligned key
lengths, zero/negative iterations, null inputs, malformed hex/Base64, altered
ciphertexts, and data encrypted by the opposite runtime. Cross-decryption must
recover the original independently specified text, not merely agree on null.
Direct raw AES encryption will supply invalid UTF-8 plaintext for the decode test.

Why test a 129-bit request? SunJCE divides by eight, so it derives sixteen bytes.
Rejecting every non-multiple of eight would change the source behavior. Why keep
null distinct from empty text? The former represents a caught failure; the latter
can be a successful result. A round-trip-only test could miss both errors.

The source demonstration main will also run in both languages. This utility has
no production caller at the pin; its library target and standalone executable
provide integration. A full node smoke test would not exercise this class.
Shared Cipher and Base64 regression tests will accompany the new differential
test. Concurrent access to the source's unsynchronized mutable cipher is outside
this deterministic corpus; no thread-safety guarantee is added.

## What the first comparison found

The first 1,253 programs disagreed in 123 programs. Three issues explained the
failures: empty ciphertext decrypts successfully to empty text in SunJCE; a null
byte buffer raises IllegalArgumentException; and the first unsuccessful Cipher
initialization exhausts its provider candidates. Once a provider has been selected
successfully, an invalid key or IV can be followed by a successful initialization.
That last distinction is a state machine, not an AES mathematics problem.

The native adapter now tracks provider selection for this supported SunJCE
configuration. Invalid hex fails before initialization and therefore does not
exhaust candidates. The expanded corpus tries each helper failure both before and
after a successful operation, then attempts recovery repeatedly. We also corrected
the shared Cipher's empty-decryption behavior and added three regression cases to
the existing utility corpus. Initial failures are retained in
[the evidence directory](../../validation/aes-util-work-in-progress).

## Verified results

Both complete builds pass. The expanded corpus passes in the normal build
(16.07 seconds) and ASAN/UBSAN build (27.65 seconds): 1,957 programs, 10,728
operations, 99 independent key checks and 3,270 plaintext/re-encryption checks.
The shared utility (1,364), network utility (5,668), decrypt-backup (422) and
restore (128) case groups also pass in both builds. Full-project parity and the
full 237-test CTest suite are not certified by this selected run.
