# CryptoPackage — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/encrypt/CryptoPackage.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/encrypt/CryptoPackage.cpp) · [C++ header](../../../../../../../src/org/minima/utils/encrypt/CryptoPackage.hpp)

## Purpose

CryptoPackage uses hybrid encryption: AES encrypts the potentially large data, and RSA encrypts the small AES key. The serialized package carries the IV, encrypted key, and encrypted data in that order.

## Follow the algorithm

1. encrypt first generates/stores an IV, then generates a random AES secret. It encrypts and stores the data with AES before attempting RSA encryption of the secret, so a bad public key can leave IV/data fields updated while secret remains old or absent.
2. decrypt first uses the private RSA key to recover the AES secret, then decrypts the data using the stored IV. It propagates key/padding errors.
3. writeDataStream serializes three MiniData fields: IV, RSA-encrypted secret, AES-encrypted payload. Each MiniData carries its own length; getCompleteEncryptedData wraps those serialized bytes in another MiniData value.
4. readDataStream assigns each field immediately after reading it. A truncated second or third field can therefore leave a partially updated object. ConvertMiniDataVersion parses one package from the supplied bytes and does not require exhaustion of trailing bytes.

## How the C++ translation works

The native class holds optional MiniData fields to model the initially unset state and uses the existing EncryptDecrypt, GenerateKey, and ByteIO routines. Assignment order and three-field serialization are preserved. A default-constructed object cannot serialize/decrypt successfully until fields are populated. Native vectors/spans replace Java stream objects; failure categories and stream-close effects are not identical by construction.

## Understanding the tests

The utility driver generates packages from both Java and native implementations for payload lengths 0, 1, 32, 117, 118, and 1024, then decrypts each with both implementations using the corresponding private key. It independently requires every successful cross-decryption to equal the original bytes. Truncated and appended serialized packages are also compared. This tests actual wire interoperability despite random IV/key/RSA padding.

- [Java utility fixture](../../../../../../../support/UtilityOracle.java)
- [Native utility fixture](../../../../../../../support/utility.cpp)
- [Utility differential driver](../../../../../../../tools/utility_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## The added source demonstration entry point

The source main generates a fresh RSA pair and 256 bytes of data, encrypts
through CryptoPackage, prints five lengths, and prints the result of comparing the
decrypted bytes with the original. Its serialized package is 428 bytes: three
four-byte length fields, 16-byte IV, 128-byte RSA-encrypted secret and 272-byte AES
ciphertext. A fresh PKCS#8 private key has variable DER length because its integer
encodings vary. The test records that length and checks the other complete lines;
it does not pretend two independently random keys are identical.

The new [crypto main test](../../../../../../../tools/crypto_mains_differential.py)
runs each actual Java and native main with null, empty and unused argument arrays.
For the two printed AES envelopes, each language decrypts output from both languages
and checks every original byte. See the [student lab](../../../../../../new-tests/crypto-main-methods.md)
and its linked evidence for actual execution results and limitations.

Java registers BouncyCastle before this demonstration. C++ reuses the linked
OpenSSL backend; it has no Java Security provider registry. Registry side effects
and provider-registration failures are not claimed as matching behavior.

## Limits and a useful next test

The fixture mostly creates a new object per operation; it does not fully establish partial state after reusing an object whose encrypt/read fails. A useful next fixture should initialize a valid package, then inject truncation at every field boundary and observe which old fields remain.

## Check your understanding

Which field is written second on the wire even though it is computed last during encryption? Why must the wire format preserve that distinction?

Pinned Java SHA-256: `6b5fd3ab11c774372aedb24941171f57543bd50ed72bd4a4d0a45107aa56b8aa`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
