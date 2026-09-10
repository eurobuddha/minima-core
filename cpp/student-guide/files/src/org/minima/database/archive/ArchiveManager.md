# ArchiveManager — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/database/archive/ArchiveManager.java) · [Native body](../../../../../../../src/org/minima/database/archive/ArchiveManager.cpp)

## Purpose

ArchiveManager coordinates the SQL archive of serialized TxBlocks and a separately stored Cascade. It chooses archive intervals, preserves insertion order at the SQL boundary, decodes returned bytes and decides when older rows may be removed.

## Follow the logic

1. Construction captures MAX_KEEP_BLOCKS as 2000 times NUMBER_DAYS_ARCHIVE. createSQL creates the syncblock and cascadedata tables, their indexes and prepared statements. Both TxPoW ID and block number are unique columns; INSERT IGNORE asks the engine to tolerate duplicates, but the Java method does not inspect the affected-row count.
2. saveBlock invokes the private insertion routine, catches any Exception and retries exactly once. Each attempt checks the connection, serializes the block, binds ID, height, current wall-clock insertion time and bytes, then executes. The second failure escapes. A first attempt may already have reached the engine before the retry, so SQL uniqueness matters.
3. loadBlock and loadBlockFromNumber query and decode the first result. SQL exceptions are logged and return null. exists similarly returns the recorded block height or -1. getSize reads count through getInt and returns -1 after an SQL failure; this is a failure sentinel, not an empty archive.
4. loadFirstBlock means the highest block, while loadLastBlock means the lowest. The active implementations first query a height, then query the full row at that height; the older single-query implementations are commented out. This two-step observation is not an atomic snapshot.
5. loadBlockRange asks for heights strictly greater than start and strictly less than end, then sorts the decoded list descending by default or ascending when requested. loadSyncBlockRange asks for heights below start and at least max(start minus 256, 1), checks Main shutdown while consuming rows, then sorts descending. SQL failure can leave a partial list; a malformed decoded null can fail later during comparison.
6. checkCascadeRequired ignores an empty incoming Cascade. With no stored Cascade, it validates the incoming object before saving it. With a stored Cascade and nonempty archive, it keeps the stored object when its tip lies between oldest archive height minus one and newest height. Otherwise it replaces it with the incoming Cascade without repeating that incoming validation branch.
7. The private cascade save first requests DELETE of existing cascade rows through executeGenericSQL, ignores that method's result, then serializes and inserts the new object. A failed delete can therefore be followed by an insert; a successful delete followed by a later failure can lose the old row. loadCascade queries without its own checkOpen and decodes the first stored row.
8. checkForCleanDB skips deletion in archive mode. Otherwise it reads the highest archived height, subtracts MAX_KEEP_BLOCKS and deletes rows below that cutoff, returning the update count; SQL failure returns zero. hackShut closes without clearing the connection reference. closeAndReopen saves/closes, clears the reference, then checks/reopens it, logging SQL failures.

## Translation and dependencies

The native implementation keeps exact SQL statements and parameter order behind SqlDBServices, with shared ownership of rows/blocks, recursive locking for synchronized calls and stable sorting. It explicitly preserves long overflow when calculating retention. Clock, debug and shutdown callbacks replace ambient Java services. It reuses native TxBlock and Cascade serialization; recording SQL supplies no H2 storage engine. Its first/last helper refers to the prepared-statement field so reopening can replace the statement before use.

## Understanding the tests

The fully read Java/native harnesses execute actual ArchiveManager bodies against recording SQL. Initial fixture bytes for blocks and cascades are compared before method cases. The driver sweeps checked and runtime failures at calls 1 through 29 for operations and 1 through 24 for schema/load; null connection/statement/prepared/results; missing, duplicate and null rows; ascending/descending lists; shutdown after zero through five checks; strict interval boundary inputs including fractional and overflowing MiniNumbers; retention overflow; retry and cascade-delete failures. Every truncated prefix of three block and three cascade fixtures is supplied, along with malformed and appended bytes. One hundred seeded random ten-operation programs check sequences. For insertion times it first asserts that the bound timestamp lies between the operation's before/after clocks, then normalizes the clock for comparison. Values, exact wire bytes, SQL events, partial lists and exception categories are compared.

- [Actual Java archive harness](../../../../../../../support/ArchiveManagerOracle.java)
- [Native archive harness](../../../../../../../support/archive_manager.cpp)
- [Archive corpus and comparison](../../../../../../../tools/archive_manager_differential.py)
- [Recording JDBC boundary](../../../../../../../support/SqlDBOracle.java)
- [Native recording SQL boundary](../../../../../../../support/SqlRecordingFixture.hpp)
- [Native fields and services](../../../../../../../src/org/minima/database/archive/ArchiveManager.hpp)

No build or test was run while writing this lesson. The inspected driver defines the evidence scope; it does not certify full parity.

## Limits

The supplied SQL rows are not filtered by a real engine, so matching events do not establish SQL execution, uniqueness enforcement, durable archive persistence or concurrent lifecycle behavior. Enhanced null-pointer expressions and logger formatting are normalized. The Java main method is a separate H2 experiment that deletes its own test directory; it is not the automated corpus and was not run for this lesson.

## Check your understanding

If the archive holds heights 10 through 20, which endpoints does loadBlockRange(10,20) request? Why can saveBlock return true even if INSERT IGNORE inserted no row?

Pinned Java SHA-256: `dabc050ed2de002f5abe95957812c372ad1a99a6766601f5b4bf128eae2d6da8`. [Mapping](../../../../../../../source-map.json) · [Project limitations](../../../../../../../REVIEW.md).
