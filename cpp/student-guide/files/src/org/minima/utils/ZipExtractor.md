# ZipExtractor — logic and a future translation

**Teaching status:** Authored source algorithm and proposed validation; native counterpart remains unported.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/utils/ZipExtractor.java](../../../../../../../upstream-study/src/org/minima/utils/ZipExtractor.java) | `src/org/minima/utils/ZipExtractor.cpp` (planned) |

## Teacher's worked explanation

ZipExtractor expands a ZIP archive into a destination directory. The input is an
InputStream, so the archive can arrive as a stream rather than an already-open
ZIP file. This pinned Java file has one public operation, `unzip`, and a private
destination-path check. **Its mapped C++ counterpart is still absent.** The
following describes source behavior and the work needed for a future translation.

### Follow one entry from archive to disk

`unzip()` creates a 1,024-byte buffer and wraps the supplied stream in Java's
ZipInputStream. It gets the first entry, then repeats these steps:

1. Construct the entry's destination path and validate canonical containment.
2. For a directory entry, ensure that directory exists. If it cannot be created,
   throw an IOException naming the path.
3. For a file entry, ensure its parent directory exists. This also supports
   archives that contain files without separate entries for every parent folder.
4. Open the file for output, read archive data into the buffer, and write each
   positive-length chunk. Close the output after the entry is exhausted.
5. Ask for the next entry. After the loop, close the current entry and ZIP stream.

This algorithm can create and overwrite real files. It does not build a complete
in-memory directory tree and then commit it atomically. If a later entry fails,
earlier extracted files remain. An error while reading a file can leave a partial
file. The source has no `finally`/try-with-resources around these operations, so
successful-path closing must not be mistaken for a guarantee on every error path.

### Why the destination check matters

The helper creates `new File(destinationDir, entryName)`. If that file already
exists, it logs an informational message; existence is not a rejection. It then
obtains canonical paths for both the destination directory and the candidate.
The candidate must start with the destination's canonical path **plus the file
separator**, otherwise extraction throws an IOException.

The separator prevents a sibling path such as `/work/app-copy` from being accepted
merely because it starts with `/work/app`. Canonicalization also matters for `..`
segments and existing symbolic links. A lexical string-prefix comparison is not
an adequate replacement for this source check. This check happens before opening
the output file. It is not an atomic defense against a concurrently changed
filesystem; the source does not provide such a guarantee.

### What a native translation can reuse

The repository already has JavaUnixFile helpers for Unix File constructor
normalization, parent/child resolution, and mkdirs behavior. In particular, Java's
Unix parent/child constructor does not behave exactly like joining an absolute
child with C++ `std::filesystem::path`'s `/` operator.

The existing GzipInput helper demonstrates incremental raw DEFLATE decoding,
zlib cleanup and CRC handling. ZIP is not GZIP, however: its entry headers,
compression methods, optional data descriptors and ZIP64 size fields differ.
Reusing the entire GZIP decoder as if the formats were identical would be wrong.
MiniFile's present `isChild()` uses lexical prefix checking and cannot supply
ZipExtractor's canonical containment rule.

No native ZIP implementation was added during this lesson. The installed JDK's
ZipInputStream source was inspected for relevant dependencies, but this is not a
claim that its full API or all formats have already been translated.

### How to test the eventual port

The pinned file contains no test methods, and its mapped native file is absent.
The scenarios below are **proposed validation**, not reported passing tests.

A Java oracle should extract identical archive bytes in an isolated temporary
directory and record files, contents, directory structure, ordered logs, and any
exception. A native fixture should do the same in a separate directory. Compare
relative paths and bytes while accounting explicitly for the different temporary
directory prefixes.

Start with empty archives, nested files with and without directory entries,
stored and deflated data, empty files, Unicode names, repeated entries, and
preexisting files. Then test malformed headers, truncated payloads, CRC errors,
unsupported compression, optional descriptors and ZIP64 size fields. Include
path traversal, sibling-prefix paths and symlinks pointing both inside and
outside the destination. After failure, compare partial files and stream-close
observations as well as the exception; returning the same error is insufficient
if one implementation already wrote different data.

**Exercise:** an archive's first file is valid and its second entry escapes the
destination. Predict which files remain after the failure. Then explain why a
test that checks only the thrown exception would miss part of the behavior.

## Dependencies and reading map

Method navigation (declarations with bodies found in this file): [unzip](../../../../../../../upstream-study/src/org/minima/utils/ZipExtractor.java#L12), [newFile](../../../../../../../upstream-study/src/org/minima/utils/ZipExtractor.java#L48).


Pinned Java SHA-256: `d32637c33ca47eb0a64f172b9318ed121e2e2dc3286aae7240a20e6e5c2ccb19`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
