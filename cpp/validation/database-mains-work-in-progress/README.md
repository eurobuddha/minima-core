# Three database source main methods

ArchiveManager, TxPoWOnChainDB and TxPoWSqlDB now have native demonstration bodies.
Both normal and ASAN/UBSAN focused runs PASS: 18 actual main calls, 36 reads of saved
H2 files, nine main output comparisons and 465 assertions each. Null, empty and
unused arguments run against separate temporary homes. No user data is touched.

Both languages read every writer's exact rows, IDs, flags, timestamps and blob
bytes. Codecs reproduce the wire and full TxPoW JSON. Tests preserve the archive's
ID-before-parent ordering and repeated reopen sequence, strict empty range, on-chain
repeat insertions, relevant TxPoW storage and adjacent file preservation.

Initial fixture failures wrongly equated independent PRNG bytes and assumed dense
auto-increment IDs. Their reports are retained. The corrected fixture verifies
32-byte PRNG data through both readers and increasing exact stored identities.
No production sequence was changed to satisfy these tests. C++ RAII can differ from
Java's no-explicit-close lifetime; identity allocation and shutdown failure parity
are not claimed. The AOT H2 library is not sanitizer instrumented.

No new mapped files: 456/457; JNI absent and complete parity not_verified. The
finite missing-main candidate list is empty, not a full API completeness certificate.
Three file lessons and the [database lab](../../student-guide/new-tests/database-main-methods.md)
explain the logic, tests and limits.
