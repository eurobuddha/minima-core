# sphincs — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/sphincs.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/sphincs.cpp)

## Purpose

sphincs exposes this repository’s custom hash-signature helpers and a multi-command transaction workflow. The name alone does not establish compatibility with a standardized SPHINCS implementation.

## Follow the command

1. generate hashes the trimmed seed’s default-charset bytes, initializes SPHINCS and returns its private/public keys, full locking script and Mx address. sign initializes from private bytes and signs data, then either serializes inline or writes a requested file after signing.
2. verify initializes from public bytes, prefers file over inline signature when both are supplied, decodes SPHINCSSig and returns its verification boolean. File path handling and decoding failures happen before that result.
3. The transaction action reads amount/address/token/private/mine, derives the signature address and creates a random custom transaction ID. It runs txncreate then txnaddamount through runMultiCommand, taking the first result each time.
4. On txnaddamount failure, delete the row and throw its error. Reject more than eight inputs with deletion. Mark every output storeState false, calculate the signature message and use SPHINCSUtils to populate signature state and transaction IDs.
5. Run txnscript and txnmmr without checking their status, then txnpost. Delete the custom row after normal posting returns, before checking its status; return its response on success. Exceptions between these explicit cleanup points can leave a row because there is no finally cleanup.
6. test uses fixed seed xxx and data 0x00, signs, serializes, decodes and verifies, returning a boolean. Unknown actions throw. The private output-string helper is separate from the active utility-based transaction setup.

## Translation and dependencies

The native counterpart delegates to the existing Command parameter/reply helpers, SPHINCS signer, SPHINCSUtils transaction helpers, and MiniFile file operations. It is registered in the actual native CommandRunner. The private first-result helper reuses support/CommandResult.hpp, preserving Boolean unboxing, String casts and nullable error messages. The unused private coin formatter delegates to the shared SPHINCS formatter. The algorithms and complete forest sizes remain unchanged.

## Understanding the tests

The differential driver executes the actual pinned Java and native command classes.
It compares help text, parameter order, errors, Unicode seed derivation, valid and
malformed signatures, file precedence, and missing files. Python independently
computes the seed/private-key hashes. An embedded NUL path must not access or
remove an existing file with the same prefix.

The fixture replaces child command prototypes only within its isolated process.
Actual txncreate/txndelete and real TxnDB rows are retained; recording children
supply input coins and capture each transaction before subsequent calls. We check
funding-error cleanup, more-than-eight-input rejection, and empty-input failure.
Full mode also signs inline, overwrites a file, executes the test action, and signs
one/eight-input transactions. Exact signature bytes, 66 state values, output flags,
posting order and row deletion are observed. It does not test network admission.

See the [test lab](../../../../../../../../student-guide/new-tests/sphincs-command.md).
The full normal run passes 131 cases/34 independent assertions, including all
five full-size signing paths. The full sanitizer run also passes 131 cases and 34
assertions, including all five signatures, in 8,573.312 seconds. Consult the [current evidence](../../../../../../../../validation/sphincs-command-work-in-progress/progress.json)
for the saved results and their precise scope. Network admission remains unverified.

- [Signature script and transaction helpers](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCSUtils.java)
- [Custom signature construction](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCS.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

The new native command fixture compiles. Test results are recorded separately so unfinished full-size runs are not presented as passes.

## Limits

Only test keys, temporary files and isolated transaction rows are used. A returned test boolean is a diagnostic, not a security audit. Nested commands do not inherit MiniDapp identity through this helper.

## Check your understanding

At which failure points is the custom transaction row explicitly deleted, and which exceptions bypass that cleanup?

Pinned Java SHA-256: `12256a1e04b166ffecc71063ad99ab74ee5acc1213845548a61800e68bbb8228`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
