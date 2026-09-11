# ZipExtractor — extracting entries without losing source behavior

**Teaching status:** Authored explanation of the pinned algorithm, native translation, actual comparison tests and remaining limits.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/ZipExtractor.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/ZipExtractor.cpp) · [Native header](../../../../../../src/org/minima/utils/ZipExtractor.hpp)

## Follow an entry from stream to disk

The class exposes unzip and has one private destination-check helper. It wraps
its input stream in ZipInputStream, reads the first entry and repeats this sequence:

1. Build the output path using Java's File(parent, child) rules. If that path
   already exists, log the destination directory. Existence is not a rejection.
2. Canonicalize the destination and output path. Require the latter to begin with
   the former plus a separator. This rejects traversal and existing symlinks that
   resolve outside the destination, before the output file is opened.
3. Create the directory entry, or ensure a file entry's parent exists. Failed
   directory creation raises an IOException naming that path.
4. Open a file entry for output, truncating existing content, and write each
   positive read from the 1,024-byte buffer. Close the file when its entry ends.
5. Move to the next entry, consuming any unread directory payload. At the end,
   close the entry and input stream.

This is not an atomic extraction. Earlier files remain if a later entry fails,
and read errors can leave the current file partially written. There is no source
finally block that closes input on every exceptional path.

## How the native code reuses existing work

JavaUnixFile handles Unix parent/child construction and mkdirs. An absolute child
still stays under a non-null Java parent, so ordinary C++ path joining would be
wrong here. BufferedFileOutput supplies direct writes through writeUnbuffered;
MiniFile.writeDataToFile would remove an existing file first, changing the source's
in-place truncation behavior. MinimaLogger supplies the actual log calls.

ZipInput is a native dependency adapter following the installed OpenJDK reader.
It reuses the GzipInput raw-DEFLATE/zlib pattern, but parses ZIP local headers,
STORED entries, CRCs, optional data descriptors and ZIP64 lengths. It retains the
512-byte compressed input and pushback buffer. It does not use the central
directory, whose information Java ZipInputStream also ignores. UTF-8 entry names
are validated strictly rather than replaced on decoding errors. Reader callbacks
return zero for EOF; a separate callback records the source's explicit close.

The canonical-path adapter follows OpenJDK's realpath-prefix search. A final
self-referencing symlink can remain unresolved after its parent is canonicalized;
opening that file then raises FileNotFoundException. A cyclic intermediate prefix
can instead fail during canonicalization. This distinction affects error timing.
The support adapters retain OpenJDK notices and the GPLv2/Classpath license.

## Why partial output is part of correctness

For a STORED entry, Java checks CRC before returning its last chunk. A bad CRC
therefore prevents that chunk from being written. For a DEFLATED entry, the last
output chunk is returned first and CRC/size validation happens on the next read.
The corrupt file can therefore contain all decompressed bytes. The corpus uses
2,050-byte inputs to prove these different outcomes: 2,048 bytes for the STORED
case and 2,050 for DEFLATED with the chosen reader buffer. Empty STORED entries
skip CRC validation in the source; the native reader preserves that behavior.

Truncated initial headers can mean end-of-archive, while truncated entry names or
descriptors raise EOF errors. Unsupported compression can be discovered only after
the output file has been created. Those effects would disappear in an implementation
that validated every archive entry before writing anything.

## Understanding the actual tests

The Java fixture calls the unmodified pinned class. Its custom InputStream records
requested read sizes, returned counts, consumed bytes and close calls, and can
inject read or close errors. The native fixture invokes the real library through
the corresponding callbacks. Reflection/a friend accessor observes the private
path helper without replacing its algorithm.

Both fixtures extract into the same reset temporary tree. Python snapshots every
file's bytes, directory and symlink target. It compares those snapshots, ordered
logs, exception categories/messages, input-read traces, input position and close
counts. Only VM-dependent null-pointer prose is normalized. Independent checks
require expected successful files, the specific CRC partial outputs, and unchanged
files outside the destination.

Both builds pass all 708 cases: standard Python-generated archives, hand-encoded
headers, both compression methods, descriptors with/without signatures, ZIP64,
extra fields, invalid names, lengths, CRCs, truncation, short reads, injected errors,
existing files/directories, duplicates, traversal and symlinks. The normal run took
4.01 seconds and ASAN/UBSAN took 5.43 seconds. Three shared regression groups also
pass in both builds: utility, SQL database and backup decryption.

The first run found a trailing-separator error in shared mkdirs: for missing/..,
the native helper created the right directory but returned false. Removing the
canonical trailing separator fixed it. The next complete run found two cyclic-link
error-boundary mismatches; the source-derived canonicalization adapter fixed both.
Initial failure reports are preserved with the final results.

[Java fixture](../../../../../../support/ZipExtractorOracle.java) · [Native fixture](../../../../../../support/zip_extractor.cpp) · [Comparison driver](../../../../../../tools/zip_extractor_differential.py) · [Student lab](../../../../../new-tests/zip-extraction.md) · [Saved evidence](../../../../../../validation/zip-extractor-work-in-progress)

## Limits and next questions

There are no production callers at this source pin, so actual extraction through
the linked library is the integration test; node startup would not exercise it.
The reader supports streaming, but the corpus does not certify actual archives
larger than four gigabytes. Concurrent filesystem replacement, special-device
write failures and all operating systems remain unverified. Native RAII releases
zlib state and output descriptors on exceptions; Java garbage-collection timing
is not reproduced. Input close callbacks preserve the source's explicit boundary.
No complete-node or complete-class parity certification follows from these tests.

Predict what remains after a valid first entry and a second entry named
../outside/marker. Why is checking only the exception insufficient? Why would
replacing canonical containment with a lexical prefix comparison be incorrect?

Pinned Java SHA-256: `d32637c33ca47eb0a64f172b9318ed121e2e2dc3286aae7240a20e6e5c2ccb19`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
