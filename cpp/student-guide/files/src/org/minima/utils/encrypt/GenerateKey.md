# GenerateKey — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/encrypt/GenerateKey.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/encrypt/GenerateKey.cpp) · [C++ header](../../../../../../../src/org/minima/utils/encrypt/GenerateKey.hpp)

## Purpose

GenerateKey selects the exact cryptographic algorithms and key encodings used by these utilities. It generates RSA/AES material and creates configured ciphers; the algorithm names and parameters are part of compatibility with the pinned source.

## Follow the algorithm

1. RSA key generation requests 1024 bits from a secure random generator. Public bytes are X.509 SubjectPublicKeyInfo and private bytes are PKCS#8; conversion asks an RSA KeyFactory to decode those containers.
2. Random secretKey generates 128 AES bits; IvParam generates 16 random bytes. convertSecret wraps supplied bytes as AES without deriving a password key.
3. Password secretKey uses the supplied character password and salt with PBKDF2-HMAC-SHA256, 65536 iterations, and a 128-bit result. Salt and IV serve different roles: salt changes derivation; IV initializes CBC chaining.
4. Cipher factories select RSA/ECB/PKCS1Padding and AES/CBC/PKCS5Padding. getCipherSYM wraps the secret and IV, obtains the AES cipher, initializes the requested mode, and returns the configured mutable cipher.

## How the C++ translation works

The native port reuses OpenSSL EVP for key generation, ASN.1 encoding, PBKDF2, RSA padding, and AES-CBC. RAII wrappers own OpenSSL objects; public key conversion strips private material by re-encoding/decoding. The native Cipher adapter checks key/IV sizes and preserves SunJCE update buffering by withholding a full padding block plus partial tail during decryption. Successful doFinal resets AES state for reuse. This is a selected provider surface, not the entire Java Cipher API.

## Understanding the tests

The utility driver derives keys for empty, Unicode, embedded-NUL, and spaced passwords across salt lengths including invalid zero. It asks each implementation to generate two key pairs, then checks decoding/re-encoding, wrong public/private container use, truncation, and appended bytes. AES cases cross plaintext block boundaries with 16/24/32-byte valid keys plus invalid lengths and IV sizes. Ciphertext produced by each language is decrypted by both, and plaintext equality is required independently.

- [Java utility fixture](../../../../../../../support/UtilityOracle.java)
- [Native utility fixture](../../../../../../../support/utility.cpp)
- [Utility differential driver](../../../../../../../tools/utility_differential.py)

The utility suite has been rerun for the decryptbackup integration. The added evidence below preserves a narrower distinction between specific provider exceptions.

## Limits and a useful next test

Random key bytes should not match between runs; tests require interoperability and valid encoding instead. Provider-specific exceptions collapse to ERROR. The source algorithm parameters are documented for reproduction, not presented as a new cryptographic recommendation. Full update/reset/failure-state behavior and every provider/key encoding remain unverified.

## Check your understanding

Why can two runs derive exactly the same password key but generate different RSA keys? What breaks if the same raw RSA integers are encoded in the wrong ASN.1 container?

Pinned Java SHA-256: `07d825bb6355b69010a21100d014eebd6e8523c64badcdb36a5ef634d896fdc4`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).


## Backup decryption integration

The [decrypt-backup lab](../../../../../../new-tests/decrypt-backup.md) reuses this
implementation through a streaming CipherInput adapter. The command comparison
requires the exact empty-salt IllegalArgumentException message and the invalid-IV
InvalidAlgorithmParameterException type/message, so those two native validations
now retain their Java meanings. It exercises update/final buffering through
truncated and damaged encrypted gzip files. This adds selected failure timing
coverage; it does not certify every Cipher provider operation or reset state.


## Restore error-cause update

The [restore lab](../../../../../../../student-guide/new-tests/restore.md) exposes
cipher errors after earlier sections have already been written. AES finalization
now distinguishes an input whose length is not a multiple of 16 from an aligned
input with invalid padding. It retains the inspected SunJCE IllegalBlockSizeException
or BadPaddingException text. CipherInput wraps the full cause in IOException.
Previously a generic AES padding message hid this observable distinction.
Successful encryption/decryption and padding calculations are reused unchanged.
