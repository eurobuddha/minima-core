# Four crypto source demonstration entry points

Added main to EncryptDecrypt, PasswordCrypto, CryptoPackage and SignVerify by
reusing the existing native crypto and stream helpers. No new mapped files:
456 counterparts remain, with external jnifunctions absent. Full parity is unverified.

Both normal and ASAN/UBSAN focused builds/tests passed: 24 actual main runs,
24 cross-decryptions and 222 assertions per build. The normal node target builds.
Raw results and logs are in the normal/sanitized folders. Full normal testing of
the earlier node candidate and full sanitizer testing are tracked separately in
the delivery evidence; these focused passes do not stand in for those results.

The source random bytes are not replaced with fixed success strings. Each printed
AES/password envelope is parsed and decrypted by both languages to its complete
original byte string. CryptoPackage prints variable private-key DER length; that
one field is recorded, not equated across independently random key generation.
Java BouncyCastle registration has no native Java registry equivalent and its
side effects/failure behavior are not certified. No existing helper was rewritten.

An initial compile failed on a missing ByteIO include in the new EncryptDecrypt
main; the include was added before both successful builds. The eight remaining
entry-point candidates are recorded separately; this is not a full API inventory.
See the [student lab](../../student-guide/new-tests/crypto-main-methods.md).
