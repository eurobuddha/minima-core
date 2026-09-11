# Teaching ZIP extraction by comparing effects

Written before running the new tests. The pinned
[ZipExtractor](../../../upstream-study/src/org/minima/utils/ZipExtractor.java)
processes entries in order and writes each positive 1,024-byte chunk immediately.
It can leave earlier files and a partial current file after an error.

We reuse JavaUnixFile path construction/mkdirs, BufferedFileOutput's direct-write
method, MinimaLogger, and the GzipInput raw-DEFLATE/CRC pattern. ZIP needs a separate
reader for local headers, STORED entries, data descriptors and ZIP64. The native
reader follows the installed JDK source and retains its GPLv2/Classpath notice.
The archive's central directory is not read by ZipInputStream.

The path check uses canonical paths, including existing symbolic links, and a
separator after the parent path. An entry such as `../outside` must fail before
opening a file. Existing destinations are logged and then overwritten; they are
not rejected. On Unix, Java's File(parent, absoluteChild) keeps the parent, unlike
the ordinary C++ path-join operator. The existing JavaUnixFile helper handles this.

The fixtures will extract identical bytes into a reset temporary tree. We compare
all file bytes, directories, symlink targets, ordered log messages, exception type
and message, input-read position and input-close calls. Controlled short reads and
read/close failures make stream consumption and error timing visible. We also call
the actual private path helper through reflection/a friend accessor.

The corpus will include Python-generated ZIPs, hand-encoded local headers with
known CRCs, both compression methods, optional signed/unsigned data descriptors,
ZIP64 size fields, malformed names, truncations, CRC/size errors and unsupported
methods. Path cases include missing parents, duplicates, traversal, existing files
and symlinks inside/outside the destination. Independent assertions require the
expected plaintext files for successful cases and unchanged outside files after
rejection. A test that compares only errors could miss a data-loss bug.

Watch the CRC timing: STORED verifies its last chunk before returning it to the
caller; DEFLATED verifies at the next read after the last output chunk. Therefore
the same corrupted CRC can leave different partial-file lengths depending on the
compression method. Empty STORED entries skip their CRC check in the source.

The source closes input on successful completion, with no finally block on error.
The native reader preserves that observable callback boundary while RAII releases
native zlib state and output descriptors during exceptions. Deterministic file
effects are tested; Java garbage-collection timing is not reproduced.

This standalone utility has no production callers at the pin. Its linked fixture
exercises the actual native library. Full node startup would not exercise it.
Filesystem races, special-device output failures and multi-gigabyte archives need
separate validation; the corpus is not a complete project parity claim.

## Findings and verified results

The first run found a shared mkdirs bug: a canonical path ending in a separator
made parent_path refer to the directory itself. The helper created the directory,
then attempted to create it again and returned false. Trimming the separator
matches Java's canonical File representation. This is observable for missing/..
paths, where the later output open still fails because the original path is used.

The next 671-case run found two self-referencing-symlink mismatches. The new Unix
canonicalization adapter follows the inspected OpenJDK prefix-resolution boundary:
initial realpath failure does not always mean canonicalization must throw.
We added 37 path/helper cases, including component-length boundaries.

All 708 cases pass in normal (4.01 seconds) and ASAN/UBSAN (5.43 seconds) builds.
Both complete builds and the utility, SQL-database and decrypt-backup regression
groups pass. Final reports and the initial mismatches are in
[the evidence directory](../../validation/zip-extractor-work-in-progress).
The full 238-test CTest suite was not run; project parity remains unverified.
