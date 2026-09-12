# RSA cipher direction: a factory returns more than its current callers use

`GenerateKey.getAsymetricCipher()` returns Java's `RSA/ECB/PKCS1Padding` cipher.
The usual encryption wrappers pass a public key for encryption and a private key
for decryption. The native adapter supported those two combinations, but rejected
private-key encryption and public-key recovery. The Java factory supports them.
Testing only the wrappers therefore missed behavior available through the factory.

The new test operations call the original Java factory and its C++ counterpart
with an explicit key direction. They reuse the existing utility fixtures, key
encodings and differential runner. Each language generates two temporary RSA key
pairs; the test checks deterministic private-key results for messages of 0, 1, 32,
116 and 117 bytes, plus oversized 118/128-byte rejection. Public-key operations
recover those messages and check truncated, extended and invalid ciphertext. Twenty
additional checks require the recovered bytes to equal the original plaintext,
so two implementations returning the same wrong result cannot pass.

The first run reported 80 mismatches in 1,492 cases. Extending the existing OpenSSL
EVP dispatch fixed most of them, but the full corpus exposed twelve empty-message
observations that still differed: Java returned an empty byte array while the
high-level padded EVP recovery rejected it. The final code uses EVP to recover the
encoded block and OpenSSL's type-1 padding checker to validate and remove padding.
That checker returns a negative number for failure and zero for successful empty
plaintext. Treating every non-positive result as failure would recreate the bug.
No custom modular arithmetic or padding implementation was introduced.

The existing SignVerify helper cannot substitute for this operation: it hashes
with SHA-256 before signing. These cipher calls operate on the supplied message
bytes. OpenSSL documents [sign/recover operations](https://docs.openssl.org/3.0/man3/EVP_PKEY_sign/)
and the [padding checker](https://docs.openssl.org/3.0/man3/RSA_padding_add_PKCS1_type_1/).
The latter is a deprecated OpenSSL 3 API, retained here for its required
empty-result behavior. Builds report that warning; supporting a provider that
removes it requires another compatibility review.

All 1,492 cases pass in Debug, optimized and ASan/UBSan builds. The earlier AES,
public-encrypt/private-decrypt, signatures, password packages, formatting, gzip
and isolated file checks remain in the same runner. See
[the before/final evidence](../../validation/rsa-cipher-directions/README.md).
The keys in these reports are generated test fixtures, not wallet keys.

This change does not certify every Java Cipher overload, RSA update buffering,
wrap/unwrap mode, provider exception text, key format or concurrency behavior.
The general utility fixture still collapses exceptions to ERROR. Those remain
separate parity work; GenerateKey is not relabelled as fully verified.
