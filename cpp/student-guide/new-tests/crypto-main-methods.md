# Testing randomized crypto demonstrations

A random demonstration cannot be tested by demanding identical random bytes from
Java and C++. We still require meaningful agreement: the same complete fixed text,
the same field layout and sizes, and the same recovered plaintext bytes.

## Four different demonstrations

| Entry point | Logic | What the new test checks |
| --- | --- | --- |
| EncryptDecrypt | PBKDF2 key, salt/IV envelope, raw AES bytes | All six output labels, 1,024-byte input, 1,072-byte envelope, exact salt/IV fields and cross-decryption |
| PasswordCrypto | MiniData, gzip and password-derived AES | Original/recovered 32-byte value, envelope size/layout and cross-decryption |
| CryptoPackage | RSA wraps an AES secret; AES encrypts the payload | Complete fixed lines, 428-byte package and successful byte comparison; random private DER length is recorded |
| SignVerify | RSA signs a random message and verifies it | Exact `128` / `true` output from the real main |

The [Java wrapper](../../support/CryptoMainsOracle.java) and
[native wrapper](../../support/crypto_mains.cpp) only select the real entry point and
argument shape. They do not replace its cryptography. The
[driver](../../tools/crypto_mains_differential.py) invokes both, parses every printed
line, then passes the actual encrypted output to the existing
[utility oracle](../../support/UtilityOracle.java) and
[native utility](../../support/utility.cpp). Each must recover all original bytes.

## Why these assertions matter

If a salt field is written without its four-byte length, the parser rejects its
layout. If encryption accidentally uses serialized MiniData instead of raw data,
the recovered EncryptDecrypt plaintext differs. If the password demonstration
prints an invented success string, cross-decryption still examines its real envelope.
If a commented-out source read is activated, the unexpected extra lines fail.

The older [utility differential](../../tools/utility_differential.py) supplies
separate RSA encryption/package interoperability and deterministic signature tests.
The new demo check does not replace that broader evidence.

## Limits

Fresh private-key DER lengths are recorded, not equated across random key pairs.
The Java demonstrations register BouncyCastle; native code uses linked OpenSSL and
does not reproduce a Java provider registry. Provider failure injection, every
exception/resource outcome and full class parity remain unverified.

Results are recorded in [the batch evidence](../../validation/crypto-mains-work-in-progress/README.md).
The driver has 24 actual main runs and 24 cross-decryption checks per build when it
completes successfully. A build alone is not that result.

## Exercise

Explain the 1,072-byte raw AES envelope and the 428-byte hybrid package. Why can
we compare a fixed RSA signature byte-for-byte but not two randomly generated keys?
