# EncryptDecrypt — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/encrypt/EncryptDecrypt.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/encrypt/EncryptDecrypt.cpp) · [C++ header](../../../../../../../src/org/minima/utils/encrypt/EncryptDecrypt.hpp)

## Purpose

These four wrappers perform a single complete encryption or decryption operation. RSA operates directly on one bounded input block; AES-CBC operates on bytes with caller-supplied key and IV.

## Follow the algorithm

1. Asymmetric encryption decodes the public key, obtains the RSA padding cipher, initializes encrypt mode, and returns doFinal(input). Asymmetric decryption follows the same sequence with a private key and decrypt mode.
2. Symmetric methods wrap the raw AES key and IV, select the AES-CBC padded cipher, initialize the direction, and return doFinal. They do not create a salt, compress data, attach a header, or authenticate a message.
3. Key conversion, initialization, and final padding checks can throw. The wrappers propagate exceptions instead of returning an empty successful result. The demonstration main contains commented experiments and is not a unit test.

## How the C++ translation works

C++ delegates to the existing GenerateKey/Cipher implementation and returns byte vectors. AES wrappers reuse getCipherSYM. The RSA native code obtains the cipher before decoding the key, whereas Java decodes first; ordinary operation results can match while exceptional provider-allocation ordering differs. Spans cannot represent Java null arrays separately from empty bytes.

## Understanding the tests

The utility fixture compares deterministic AES ciphertext bytes for fixed key/IV/plaintext, then tests decryption after truncation and appended/corrupted bytes. RSA encryption is randomized, so the driver cross-decrypts outputs from both implementations and requires the original plaintext. At the source’s 1024-bit key size, direct RSA inputs of 117 bytes are exercised alongside 118 and 1024; oversized direct RSA encryption may fail while a hybrid CryptoPackage can handle the same larger data.

- [Java utility fixture](../../../../../../../support/UtilityOracle.java)
- [Native utility fixture](../../../../../../../support/utility.cpp)
- [Utility differential driver](../../../../../../../tools/utility_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## The added source demonstration entry point

The source demonstration creates 1,024 random bytes, an 8-byte salt and a
16-byte IV. It derives the AES key from `hello`, writes salt and IV as length-prefixed
MiniData, then encrypts the raw bytes. Closing the output cipher emits its padding
block. The printed envelope is 1,072 bytes: 12 for salt, 20 for IV and 1,040 for the
padded payload. Native code reuses `Cipher.update`, `doFinal` and MiniData readers.
The final source input-stream read is commented out. Native main initializes the
decrypting cipher but deliberately does not decrypt or print extra output.

The new [crypto main test](../../../../../../../tools/crypto_mains_differential.py)
runs each actual Java and native main with null, empty and unused argument arrays.
For the two printed AES envelopes, each language decrypts output from both languages
and checks every original byte. See the [student lab](../../../../../../new-tests/crypto-main-methods.md)
and its linked evidence for actual execution results and limitations.

## Limits and a useful next test

ERROR comparison does not prove exact exception types or which operation failed first. Successful round trips alone would be weak evidence, which is why the driver also compares deterministic results and cross-language outputs. Null arrays, provider initialization failures, and all malformed padding patterns remain incomplete.

## Check your understanding

Why can a CryptoPackage encrypt 1024 bytes while encryptASM cannot directly encrypt the same bytes with the generated key?

Pinned Java SHA-256: `506b6c94647b7c1b95129ccfb97e140c83e16ffd2147be170500e913a59f14fe`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
