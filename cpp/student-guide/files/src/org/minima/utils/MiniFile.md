# MiniFile — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/MiniFile.cpp) · [C++ header](../../../../../../src/org/minima/utils/MiniFile.hpp)

## Purpose

MiniFile centralizes byte/object persistence, copying, directory traversal, content-type lookup, and gzip helpers. Some methods throw IO errors while others log and return. Matching only successful file bytes would miss those differences.

## Follow the algorithm

1. createBaseFile uses BASE_FILE_FOLDER only for names without either slash character. A name containing a slash is used as supplied, even if relative. It attempts to create parent directories. writeDataToFile creates parents and either appends or replaces the destination before writing and flushing.
2. writeObjectToFile serializes into memory before writing. saveObject uses the same basic approach, while saveObjectDirect opens/truncates a destination and lets the object write through buffered streams. A serialization failure can therefore affect an existing file differently in the two paths.
3. readCompleteFile allocates from the file length narrowed to int and performs a buffered read. loadObject reads those bytes before decoding; loadObjectSlow decodes through a buffered file stream. Missing files are logged and ignored. Plain wrappers catch IOException; encrypted wrappers catch broader Exception after password decrypt/encrypt.
4. copyFile streams chunks; copyFileOrFolder recurses into directories. Deletion recurses first, then permits deletion when the absolute filename starts with the supplied parent string. isChild separately normalizes absolute paths but also uses a textual prefix comparison, not a component-aware descendant check.
5. Size routines recurse and add file lengths, optionally adding formatted directory totals up to a reporting depth. Content types are selected by a case-sensitive extension table. readAllBytes loops to EOF; gzip helpers copy through compression/decompression streams and report IO failures.

## How the C++ translation works

Native filesystem/stream primitives replace Java File and streams, with explicit signed-width conversions and shared MiniData/PasswordCrypto/Gzip helpers. createBaseFile now reuses JavaUnixFile normalization, parent-child joining and mkdirs. readCompleteFile narrows the stat length before allocation and raises typed file-open errors. The writer follows Java’s parent creation, optional deletion, exclusive createNewFile, output open, write and close sequence; it distinguishes IOException during creation from FileNotFoundException during opening. Object templates use serialise or readDataStream/deserialise. Important limits remain: native loadObjectSlow currently reads the whole file, saveObjectDirect serializes in memory after opening the destination, gzip helpers buffer entire files, and logging goes through std::clog. These are meaningful stream, memory, and failure differences despite normal-file compatibility.

## Understanding the tests

The utility fixture creates an isolated tree, writes a payload, appends bytes 1,2,3, copies files/directories, compresses/decompresses, and compares bytes and recursive totals. It saves/loads a MiniData object through plain, direct/slow, and password-encrypted wrappers and checks directory removal. The driver repeats payload lengths 0,1,16,256,2048, compares content-type extensions including HTML versus html, and explicitly tests prefix-path cases such as /tmp/a versus /tmp/ab/file. Gzip mutation cases include truncation, trailing bytes, and concatenated streams.

- [Java utility fixture](../../../../../../support/UtilityOracle.java)
- [Native utility fixture](../../../../../../support/utility.cpp)
- [Utility differential driver](../../../../../../tools/utility_differential.py)

The [offline-file lab](../../../../../new-tests/offline-send-files.md) adds actual command and direct-writer tests with exact error categories, message text, recursive file bytes and directory observations. Initial missing-file and output-parent mismatches exposed the generic native stream errors. Corrections preserve errors and the signing effects that occur before a write failure. The normal utility suite was rerun and passes all 1,361 cases; the lab records the separate offline comparison results.

## Limits and a useful next test

The original utility fixture describes normal persistence; the new lab adds selected I/O failures. Neither proves arbitrary races, symlink behavior, permission combinations or large streaming workloads. The textual path-prefix behavior must not be mistaken for a proven security boundary. A proposed failure fixture should force object serialization to fail after a prefix and compare file contents, resource cleanup, and logs for both save paths.

## Check your understanding

Why can saveObjectDirect destroy previous content before serialization finishes while saveObject may leave it alone? Does a textual startsWith check distinguish /tmp/a from /tmp/ab?

Pinned Java SHA-256: `240fda1137363bb3e52760d4c48aa3f032361e52d2108ee167b103ebaaa605c5`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
