# PasswordCrypto — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/encrypt/PasswordCrypto.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/encrypt/PasswordCrypto.cpp) · [C++ header](../../../../../../../src/org/minima/utils/encrypt/PasswordCrypto.hpp)

## Purpose

PasswordCrypto writes a password-encrypted, compressed MiniData value. The outer bytes start with clear salt and IV fields; the encrypted inner bytes are a gzip stream containing a serialized MiniData, including its length.

## Follow the algorithm

1. Generate an 8-byte salt and 16-byte IV and write each as MiniData to the output. Derive the AES key with GenerateKey’s password/salt routine.
2. Layer streams so writing MiniData goes through DataOutputStream, gzip compression, AES encryption, and then the outer output stream. Flush/close finalizes compression and padding before returning the resulting bytes.
3. Decryption reads salt and IV first, derives the same key, then layers cipher input, gzip input, and data input. It reads exactly one MiniData from the uncompressed content.
4. After that one object is read, close the streams and return it. The method does not explicitly read the entire decrypted/gzip tail to EOF, which makes early data availability and late padding/trailer failures observable.

## How the C++ translation works

Native encryption reuses MiniData serialization, Gzip, and GenerateKey in memory. Decryption deliberately feeds ciphertext through Cipher.update in chunks and attempts to decode an available MiniData before doFinal, reflecting Java’s early-return stream behavior. If no object is available, it finalizes and retries. This is more nuanced than always decrypting/validating every byte first, but does not reproduce every stream resource/memory effect.

## Understanding the tests

The utility driver encrypts payloads of lengths 0, 1, 255, 512, 1024, and 4096 with a Unicode password in both implementations. It cross-decrypts every resulting envelope in both directions and independently checks original plaintext bytes. Truncated and appended encrypted inputs are compared. File utility cases also save/load an encrypted MiniData through MiniFile.

- [Java utility fixture](../../../../../../../support/UtilityOracle.java)
- [Native utility fixture](../../../../../../../support/utility.cpp)
- [Utility differential driver](../../../../../../../tools/utility_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Exact failure messages collapse to ERROR; corruption can be accepted when enough inner data was already read, so successful decryption is not evidence of an authentication check. Further tests should place damage at the gzip trailer, padding block, and MiniData body separately, explaining which layer actually consumes those bytes.

## Check your understanding

Why is encrypting raw data different from encrypting data.serialise()? Which outer fields let the receiver derive the key and initialize the cipher without decrypting first?

Pinned Java SHA-256: `5b6e205f3a2ece2b6a1d495d23d3c06f1ce61afcb9b46ef908e954a2861bccf3`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
