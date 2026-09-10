# SqlDB — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/SqlDB.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/SqlDB.cpp) · [C++ header](../../../../../../src/org/minima/utils/SqlDB.hpp)

## Purpose

SqlDB is a lifecycle and result-formatting wrapper around H2 JDBC. Subclasses supply createSQL. It is not itself a SQL engine: opening a connection, executing a statement, and reading metadata are delegated to a driver.

## Follow the algorithm

1. loadDB records the base filename, creates parents, derives the .mv.db filename, connects with MySQL compatibility and autocommit enabled, then calls createSQL. The encrypted path sets a sticky flag and adds CIPHER=AES plus the file/user password combination.
2. checkOpen returns whether reopening was needed, not whether the database is currently open. A missing or closed connection is recreated using the saved encrypted flag and password, then schema setup is repeated. A later plain load does not clear that encrypted flag.
3. hardCloseDB closes a live connection. saveDB executes SHUTDOWN or SHUTDOWN COMPACT before closing; both catch and log exceptions. Backup deletes an existing target and issues SCRIPT TO, while restore first executes DROP ALL OBJECTS and then RUNSCRIPT FROM. Restore failure can therefore happen after destructive SQL has already executed.
4. executeGenericSQL holds the object monitor, checks/reopens the connection, and executes the supplied SQL. Query results use metadata column names; null cells are omitted, exact java.sql.Clob metadata triggers getSubString, and all other nonnull values become strings through toString. Duplicate names overwrite earlier cells.
5. An update returns status=true and results=false without rows/count. Any caught exception logs without notification and resets count to zero, rows to empty, results/status to false, and adds error text. Statement close occurs after result construction, so its failure also replaces an otherwise successful result. JSON conversion simply encodes/decodes MiniString bytes and casts the parsed top level to JSONObject.

## How the C++ translation works

The native wrapper reproduces these calls through SqlConnection/Statement/ResultSet interfaces and requires a connection provider at construction. Shared references model nullable driver objects; a recursive mutex preserves synchronized subclass reentry. Explicit unsigned conversions preserve Java narrowing and count overflow. There is no native H2 storage/encryption implementation supplied by this wrapper. GC is a JVM hint with no native equivalent, and the fallback logger is std::clog.

## Understanding the tests

The Java fixture deregisters real JDBC drivers and installs a recording driver; native fixture objects record equivalent events. The driver compares connection URLs/credentials, every SQL/metadata/CLOB call, logs, returned JSON, and partial filename/file-deletion state. Cases inject failure at successive calls, test sticky encrypted reopening, quote-containing filenames, duplicate columns, null rows/metadata, CLOB length narrowing, and malformed UTF-8 conversion. The separate native concurrency fixture runs eight producers with 128 calls each and two reentrant schema probes; it requires 1026 statements, two connections, two schema calls, no overlap, and zero failures.

- [Recording Java JDBC fixture](../../../../../../support/SqlDBOracle.java)
- [Native fixture](../../../../../../support/sql_db.cpp)
- [Lifecycle, rows, and failure comparison](../../../../../../tools/sql_db_differential.py)
- [Concurrency and reentry assertions](../../../../../../support/sql_db_concurrent.cpp)
- [Explicit native driver boundary](../../../../../../src/support/SqlConnection.hpp)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The recordings establish wrapper call sequences only; they do not execute SQL, encrypt a database, recover a backup, or prove persistence. NPE/class-cast wording and logger prefixes/stacks are normalized. Actual native H2 compatibility and a real encrypted reopen/restore test remain required.

## Check your understanding

Why can checkOpen return false for a healthy database? Trace the returned JSON when closing the statement fails after all query rows have been read.

Pinned Java SHA-256: `3b7a0ccc8e0dea2e638df47273b1088c10a0777d4d515bcf8baf4e23b9c9678f`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
