# AesUtil — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/encrypt/javajs/AesUtil.java) · `src/org/minima/utils/encrypt/javajs/AesUtil.cpp` (absent)

## Purpose

AesUtil is a Java/text interoperability helper using caller-supplied hex salt/IV and Base64 ciphertext. It has different password derivation from GenerateKey: PBKDF2-HMAC-SHA1 with constructor-selected iterations and key size.

## Follow the algorithm

1. Construction stores key size and iteration count and creates one mutable AES/CBC/PKCS5Padding cipher. If obtaining the cipher fails, fail returns null and throw null becomes a NullPointerException.
2. generateKey converts the salt through MiniData hex parsing, derives the requested number of AES bits with PBKDF2-HMAC-SHA1, and wraps the result as AES. Its selected checked failures return null.
3. encrypt derives that key, converts plaintext using the JVM default charset, initializes the shared cipher with the decoded IV, encrypts, and Base64-encodes the bytes. Any Exception in the public method returns null.
4. decrypt Base64-decodes, decrypts with the same derivation/IV, and constructs text explicitly as UTF-8. Any Exception returns null. This asymmetric charset choice is observable outside UTF-8-default runtimes.
5. The static Base64 and hex helpers expose their own behavior directly; unlike the public encrypt/decrypt methods, their malformed-input failures are not all converted to null.

## Translation work still required

The mapped C++ file is absent: this class is unported. The existing AES and encoding components are candidate dependencies, but GenerateKey’s SHA-256/65536-iteration derivation cannot be substituted for this class’s SHA-1/configurable derivation. A faithful implementation must also preserve null-on-error public wrappers, charset behavior, and shared cipher state.

## Understanding the tests

The source main encrypts then decrypts fixed demonstration text using key size 128 and 1000 iterations and prints both strings; it contains no assertion. No native AesUtil comparison is claimed. Proposed tests should fix salt/IV/password/plaintext, compare Base64 bytes against Java, repeat with Unicode under multiple default charsets, and distinguish public null results from direct helper exceptions.


This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

The proposed tests have not been implemented or run for this absent counterpart. Existing PasswordCrypto/GenerateKey tests do not cover the different derivation or text envelope. Concurrent reuse of the single Cipher also needs explicit analysis.

## Check your understanding

Why would reusing GenerateKey.secretKey produce incompatible ciphertext despite both helpers ultimately using AES-CBC?

Pinned Java SHA-256: `480ce14c7856cb887c04620df7791203f5a5d8ef77bd90c04107a1925c371e46`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
