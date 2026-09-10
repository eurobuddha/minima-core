# sphincs — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/sphincs.java) · `src/org/minima/system/commands/base/sphincs.cpp` (absent)

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

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse the pinned SPHINCS, SPHINCSSig and SPHINCSUtils algorithms and wire format, which are themselves unported. The workflow also needs real custom transaction commands; substituting a standard cryptography package would not reproduce this source.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests cover fixed seed bytes, Unicode charset assumptions, file precedence, malformed signatures and inline/file round trips. For transactions, record exact subcommand order, eight-input boundary, output storeState changes, ignored intermediate failures and row existence after each injected exception. The Java utility experiment checking one tree is not proof of the full sixteen-tree script.

- [Signature script and transaction helpers](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCSUtils.java)
- [Custom signature construction](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCS.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No keys were generated and no transaction or filesystem action was executed for this lesson. A returned test boolean is a diagnostic, not an automated assertion suite or security audit. Nested commands do not inherit MiniDapp identity through this helper.

## Check your understanding

At which failure points is the custom transaction row explicitly deleted, and which exceptions bypass that cleanup?

Pinned Java SHA-256: `12256a1e04b166ffecc71063ad99ab74ee5acc1213845548a61800e68bbb8228`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
