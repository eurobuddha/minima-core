# AesUtil — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native translation, comparison tests and remaining limits. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/encrypt/javajs/AesUtil.java) · [Mapped C++ body](../../../../../../../../src/org/minima/utils/encrypt/javajs/AesUtil.cpp) · [Native header](../../../../../../../../src/org/minima/utils/encrypt/javajs/AesUtil.hpp)

## Purpose

AesUtil encrypts text with a password and returns Base64 ciphertext. The caller
supplies a hexadecimal salt and initialization vector (IV). It is a standalone
interoperability helper; the pinned production tree has no caller outside its own
main demonstration.

## Follow the algorithm

1. Construction stores the requested key size and iteration count, and creates
   one mutable AES/CBC/PKCS5Padding cipher. Construction does not validate the key
   size or iteration count. The source fail helper returns null; throwing that
   value would become a NullPointerException if cipher acquisition failed.
2. generateKey obtains PBKDF2WithHmacSHA1, evaluates passphrase.toCharArray before
   decoding the salt, and builds a PBEKeySpec. Empty salt and nonpositive counts
   raise argument errors. SunJCE derives floor(keySize / 8) bytes. SecretKeySpec
   rejects an empty result, while AES key-length validation happens later.
3. encrypt derives the key, encodes plaintext with the default charset, initializes
   CBC with the decoded IV, encrypts with padding, and Base64-encodes the result.
   Any Exception reaching the public wrapper becomes null.
4. decrypt derives the key, decodes Base64, decrypts, and converts the bytes to
   UTF-8 text. Malformed UTF-8 is replaced during decoding. Empty ciphertext is
   accepted as empty plaintext by the supported SunJCE cipher.
5. doFinal catches specific key, IV, length and padding exceptions and returns
   null. Other failures propagate to the public wrapper. A null input byte array
   raises IllegalArgumentException, while an invalid hexadecimal IV raises
   NumberFormatException before cipher initialization.
6. The public Base64 and hex helpers expose failures directly. Hex delegates to
   MiniData, including its odd-digit padding and Unicode uppercase behavior.

## How the C++ translation works

The translation reuses the existing Cipher implementation in GenerateKey.cpp,
MiniData hex parsing, Base64 helpers and Java Unicode conversion. AesUtil stores
nullable UTF-16 strings so a null input and a lone surrogate remain representable.
OpenSSL performs PBKDF2-HMAC-SHA1 with the source parameters. We do not reuse
GenerateKey.secretKey, which selects SHA-256 and a fixed iteration count.

The class also preserves a less obvious state transition. Before the first
successful initialization, Java searches available cipher providers. An invalid
key or IV can exhaust that search, leaving subsequent attempts unsuccessful even
with corrected inputs. After a successful selection, the same invalid arguments
can be followed by recovery. Two state flags retain this behavior for the supported
SunJCE configuration. Hex/Base64 parsing errors occur before that transition.

The shared native Cipher needed one correction: it returns empty bytes for empty
padded decryption without passing through OpenSSL's rejecting padding finalizer.
The guard applies only when no input has been accumulated. Normal padded encryption
still emits a full block for empty plaintext.

## Understanding the tests

The differential fixture keeps one object across each program. It calls actual
public methods and observes private helpers through Java reflection and a native
friend accessor. Input strings are arrays of UTF-16 code units. Exact byte outputs,
text, null values, exception classes and messages are compared; only VM-dependent
null-pointer prose is excluded.

The corpus contains 1,957 programs and 10,728 operations. It covers AES sizes
128/192/256 and neighboring bit lengths, empty/Unicode/NUL/lone-surrogate passwords,
block boundaries, malformed codecs, null arguments, invalid IVs, damaged padding,
raw malformed UTF-8, initial provider failure and recovery after selection.
Ninety-nine keys are checked independently with Python's PBKDF2 implementation.
Another 3,270 assertions require the original decoded plaintext or repeated
ciphertext, so agreement on two failures cannot pass as a successful round trip.
The main demonstration must print matching ciphertext and `helloyou!`.

The first comparison found 123 mismatching programs. Those failures exposed the
empty-decryption, null-buffer and initial-provider-state differences described
above. The expanded corpus passes in both normal and ASAN/UBSAN builds. Build-specific
results are recorded in the linked evidence directory.

[Java fixture](../../../../../../../../support/AesUtilOracle.java) · [Native fixture](../../../../../../../../support/aes_util.cpp) · [Test driver](../../../../../../../../tools/aes_util_differential.py) · [Student lab](../../../../../../../new-tests/aes-text-interoperability.md) · [Saved evidence](../../../../../../../../validation/aes-util-work-in-progress)

## Limits and a useful next test

The supported runtime default charset is UTF-8; Java can be launched with another
default, affecting plaintext encoding but not password derivation or decrypted
text decoding. Other default charsets and provider configurations remain open.
The corpus does not certify cipher acquisition failures, allocation exhaustion,
private modes unused by the source callers, or concurrent use of the mutable
cipher. Compilation and these tests do not establish full-project parity.

## Check your understanding

Why can an invalid IV prevent all future encryptions on a fresh object, yet permit
recovery on an object that has already encrypted successfully? Why does 129-bit
PBKDF2 still produce a usable 128-bit AES key on this provider?

Pinned Java SHA-256: `480ce14c7856cb887c04620df7791203f5a5d8ef77bd90c04107a1925c371e46`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
