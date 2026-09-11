# SPHINCS: authenticating a forest root with a tree key

The pinned class connects two existing mechanisms. TreeKey authenticates the FORS
root hash; FORS authenticates the message. We reuse the native implementations of
both, including the full sixteen trees of 65,536 leaves. A different SPHINCS library
would not reproduce this repository's construction or wire format.

Before executing the new tests, we plan to compare the actual Java and C++ signers:

- Derive private seeds independently using Python SHA3(seed + ASCII WOTS/FORS).
  Check the public root and total 192^4 = 1,358,954,496 possible WOTS uses.
- Try every too-short private-key length, before and after initialization. The
  input reference changes immediately, but the old seeds and public key survive
  a failed copy. Copy order is interleaved: length 1 fails at index 32, not 1.
- Sign real messages with the full forest. Compare every serialized byte and JSON
  field, then pass both writers' signatures to both actual verifiers.
- Reconstruct the selected index independently as unsigned SHA3(message) modulo
  192^4. Parse its decimal text as hexadecimal, just as MiniData does. Check the
  sixteen selected secrets and all inner/outer proof paths independently.
- Alter WOTS, root hash, root sum, FORS, message and public key separately. Observe
  which verification stage returns false or throws, including null components.
- Compare the actual source mains and require empty native sanitizer stderr.
  Regress the shared TreeKey and Winternitz changes through object tests.

The signature container's wire/error tests are described in the
[envelope lab](sphincs-signature-envelope.md). These tests examine this pinned
implementation; they do not establish cryptographic security. Results will be
recorded after execution, and incomplete parity will remain explicit.
