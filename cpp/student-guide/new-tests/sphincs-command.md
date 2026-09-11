# SPHINCS command: connecting a cryptographic library to a command interface

This lab follows `org/minima/system/commands/base/sphincs.java`. Its native partner
is `cpp/src/org/minima/system/commands/base/sphincs.cpp`.

The wrapper delegates cryptography to the already translated SPHINCS classes.
Generation hashes the seed text before creating the key. Signing either returns
serialized hexadecimal bytes or writes exactly those bytes to a file. Verification
prefers a file when both a file and an inline signature are supplied.

For a transaction, the wrapper creates a temporary database row, asks a child
command to fund it, rejects more than eight input coins, and turns off output
state storage. It signs the special SPHINCS message and installs 66 state values.
It then requests scripts, proofs, and posting before deleting the temporary row.
Notice that the source ignores some child failure statuses. A faithful port must
preserve that behavior; automatically adding early returns would change it.

Before execution, the test plan is:

1. Run the actual pinned Java and native commands on identical parameters; compare
   metadata, parameter failures, seed derivation, and verification replies.
2. Reuse a real previously checked signature to test corruption, wrong messages,
   wrong public keys, file precedence, and missing files without building a new
   million-leaf forest for each small error case.
3. Execute genuine signing for inline output, replacement of an existing file,
   the source's `test` action, and one/eight-input transaction paths.
4. Record nested commands and actual database rows in an isolated fixture. Check
   call order, cleanup, all 66 signature state entries, and output storage flags.
   Controlled child commands make this a wrapper test, not a network test.
5. Run both ordinary and address/undefined-behavior sanitizer builds. Preserve
   reports and incomplete phases honestly; a compiled file is not proof of parity.

Driver: `cpp/tools/sphincs_command_differential.py`. `--quick` performs the helper
and boundary cases; the default additionally performs five full-size signing
operations per runtime. Results are pending at the time this lab is introduced.
