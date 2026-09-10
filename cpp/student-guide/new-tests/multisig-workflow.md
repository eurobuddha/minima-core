# Lab: multisig command workflows

Written before the new comparisons. Reuse the exact existing contract literal.
Both commands support getkey/list/spend/view/post; only multisig supports create
and sign. The read command uses a raw spend address, whereas multisig normalizes
it. A read-classified command can still run its source's nested export/post calls.

Run actual CommandRunner children with Java-created coins, state, MMR proofs,
wallet keys and temporary transaction files. Compare nested replies, retained
custom rows, exported bytes, signatures, SQL calls, files and mining queues.
Normalize only verified 32-byte random transaction IDs and clock-generated
filenames, preserving transaction bodies and signatures. Record controlled send
failures separately from real send/mining operations.

Check duplicate id hashes in multiple state ports, first-match spend selection,
Minima versus scaled-token change, full/partial/negative amounts, self-address
comparison, malformed keys and fractional/wrapped signature thresholds. The
source concatenates command strings, so spaces, quotes and separators require
explicit cases rather than silently adding escaping.

Signing imports first, uses every state value that matches a wallet public key,
then exports and deletes. Test missing/empty files, missing first input, repeated
matching keys, late child failures, password lock/unlock behavior and surviving
rows. The parent may return status true while a nested command reports false;
tests must inspect both layers. Finite local execution does not prove on-chain
acceptance, durable SQL or complete native-node parity.

## Observed checkpoint

The final corpus contains 340 programs, 1,089 operations and 22,269 recording-SQL
observations. The final normal and sanitizer comparisons both pass, including the complete
subdirectory-file observation. See the saved progress record for results. Four affected suites cover registration, transaction commands,
witness commands and the earlier offline send workflows. Both full builds pass.

A useful walk-through is the successful spend/sign/verify/post sequence. Java
creates a mature coin with the exact multisig script address and state containing
a known wallet key. Spend loads its proof, creates a state-free payment and a
state-storing change output, and writes a TxnRow file. Sign imports that file and
uses the actual TreeKey signer. A separate reader consumes the exported bytes
and verifies the witness signature against the transaction ID. Finally, the real
post command builds proofs/scripts, checks the contract, and invokes the existing
fixture miner. Verification also checks the signatures on the mined transaction.

The fixture supplies recorded SQL rows and a local chain; it does not contact a
live node or persist a production wallet. Key-use SQL writes are observations,
not proof of durable key-use accounting. Five vault-password programs exercise
correct, incorrect, empty, null and wrong-type password values. Existing vault
normalization compares decrypted state while excluding independently random
ciphertext bytes. A controlled send replacement tests unusual status/error shapes;
three additional creation programs execute actual send against native funding.

The first comparison found that the fixture converted a C++ exception through
`what()`, losing text after NUL. Using the exception's stored optional string fixes
the observer. A later malformed import exposed a production mismatch: txnimport
used a generic runtime error instead of Java's null-pointer exception. Replacement
ID parsing remains before dereferencing the decoded row. Both failures are saved.

Only traced, shape-checked random IDs are normalized, and binary replacement
requires the complete MiniString length-plus-ID field. Timestamp names are also
shape-checked. Subdirectory export uses an absolute path inside the temporary
observation root because source MiniFile treats slash-containing names as direct
paths. This ensures its complete bytes are included in the comparison.

The initial plan above also names experiments not exhaustively covered by this
finite corpus: an imported transaction with no inputs, exhaustive later signing
failures, all root/threshold contract combinations, every platform path rule and
reference/concurrency APIs. Those remain follow-up work, not claimed passes.
