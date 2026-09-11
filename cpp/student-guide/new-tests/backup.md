# Lab: making a six-section encrypted node backup

Written before the new test. Read the pinned backup.java from top to bottom.
A backup is a snapshot of six different stores: wallet SQL, cascade, chain tree,
user preferences, peer preferences, and relevant transaction history. Each gets
a four-byte length prefix. Gzip compresses these frames; password-derived AES
then encrypts the compressed stream. Two cleartext frames carry the salt and IV.

Reuse the existing MinimaDB stores, MiniData serialization, GenerateKey cipher,
and file operations. A streaming gzip writer adapts the zlib setup already used
by Gzip.hpp to the Java output stream's 512-byte chunks, header and CRC trailer.
BufferedFileOutput supplies an unbuffered write entry point for FileOutputStream.
The command must preserve ordering: the key-readiness gate comes before auto,
auto:false returns early, auto:true changes preferences before later failures,
state is saved under a read lock, and the temporary folder is cleaned afterward.
The source reads complete despite omitting it from the valid parameter list.

The test will run the unmodified Java command and native command with actual H2
storage, restoring the same baseline database before each run. Random salt/IV
make ciphertext unequal by design: decrypt both files in both runtimes and
compare all six decoded sections instead. Compare replies, preference changes,
cleanup, lock counts and error categories. Require successful normal backups;
equal errors do not prove backup functionality. Exercise password/confirm gates,
auto, history limits, filename aliases, directories, missing parents and Unicode.
The default timestamp filename is checked against the measured execution window.
Metadata and the real command registry must also agree.

Record mismatches and any observer normalizations explicitly. These tests cannot
alone certify every public method, resource-exhaustion behavior or concurrent
node mutation. The source map remains honest about those gaps.

Before adding the stream subtest: observe GZIPOutputStream's downstream writes
with a recording OutputStream, including an injected IOException on a selected
write. Compare compressed bytes and individual write sizes with GzipOutput.
Use empty, repeated and random inputs around 512-byte and larger boundaries.
This isolates header timing, the final CRC/length trailer and error propagation
from the command's random AES wrapper. Decompress successful streams independently.


## Results and lessons learned

Normal and ASAN/UBSAN builds each pass 96 command cases plus 384 gzip stream
cases. Each build produces 66 surviving Java/native files and verifies 132
cross-decryptions. The comparison checks exact plaintext, all six section sizes
and hashes, saved state files, destination preservation/replacement, cleanup,
lock counts, exceptions and responses. Both complete C++ builds and seven selected
regression suites pass (54.08 seconds normal, 55.43 seconds sanitized).

The first observer incorrectly expected WALLET in the SQL script; the actual
wallet schema has KEYS, SCRIPTS and SEED. We corrected that assertion. The first
implementation mismatch was numeric block 12 versus string "12". Another failure
showed that a blank maxhistory must retain two nested CommandException names.
Both were fixed. Source-dependent helpful-NPE/class-cast prose is normalized in
direct and wrapped exceptions, with categories/nesting still checked. This is a
recorded limitation rather than a claim of exact diagnostic text.

Review also preserved four-byte/payload write boundaries for salt/IV and the
source's cipher-close attempt after a gzip-finish IOException. Normal source
cleanup is explicit; RAII releases native descriptors and zlib state if execution
throws. We do not emulate Java resource leaks. Mutable date-formatter APIs,
resource exhaustion, concurrent database mutations and special-device failures
remain uncertified. The full port is still incomplete.

See [saved reports](../../validation/backup-work-in-progress/progress.json).
