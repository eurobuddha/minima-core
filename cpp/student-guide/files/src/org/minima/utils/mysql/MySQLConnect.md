# MySQLConnect — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/mysql/MySQLConnect.java) · [Native counterpart](../../../../../../../src/org/minima/utils/mysql/MySQLConnect.cpp)

## Purpose

MySQLConnect persists archive blocks, cascades, coins, and TxPoWs through a MySQL JDBC connection. It owns prepared statements and translates stored binary objects and row metadata into Minima objects or JSON.

## Follow the algorithm

1. init builds the MySQL autoReconnect URL and opens a connection. Unless readOnly, it creates four tables; readOnly only skips DDL and does not prevent later writes. It then prepares inserts, point lookups, ascending block ranges limited to 250, counts, and cascade/coin/TxPoW queries.
2. saveBlock serializes the full TxBlock first, clears statement parameters, binds ID/number/current time/blob, and executes INSERT IGNORE. SQL exceptions log and return false, but a duplicate ignored insert can still return true. saveTxPoW follows the same pattern with ID/blob.
3. Block lookups decode stored syncdata, while range loading appends rows in ascending block order. SQL exceptions can return an already accumulated partial list. loadFirstBlock uses descending order and therefore selects the greatest block; loadLastBlock uses ascending order and selects the least. loadCascade selects the smallest cascade tip.
4. insertCoin binds 14 values: exact amount text plus approximate double, token/state metadata, MMR index, spend/create blocks and date. searchCoins executes arbitrary supplied SQL, omits null cells, optionally replaces token with _hidden_, expands exact Clob metadata, and formats exact Double metadata with locale-sensitive %f.
5. Wipe methods issue DROP TABLE statements, close, then reinitialize. shutdown catches SQLException but does not protect a null connection. Many methods catch only SQLException, so decoding/type/null failures propagate differently. Result sets/prepared statements are generally not explicitly closed per operation.

## How the translation works

The C++ class composes the existing SQL connection interfaces and the actual TxBlock, TxPoW, Cascade and Coin codecs. A connection callback replaces JDBC driver discovery. The SQL text and binding order come directly from the pinned Java file. Optional strings retain null credentials; shared pointers retain null decoded objects; a recursive mutex covers the source's synchronized methods. Initialization assigns each statement as it succeeds, so a later failure preserves earlier assignments.

The sibling SQLite wrapper cannot replace this class: it has different SQL syntax, metadata and connection behavior. The existing SqlDB query helper also catches different failures and returns different error JSON. We reuse their small compatible interfaces and fixtures, then translate this class's own control flow.

For example, an MMR entry is an arbitrary decimal. Java's longValue truncates its fractional part and keeps the low 64 bits. The native helper performs that operation with unsigned arithmetic so overflow is defined. Double output first uses the existing Java decimal conversion, rounds to six places using the JDK's decimal rounding rule, and inserts ICU locale digits and separators. This preserves negative zero and avoids imposing MiniNumber's magnitude limits on database doubles.

## Understanding the tests

[The lab](../../../../../../../student-guide/new-tests/mysql-connection.md) was written before the tests. [The comparison driver](../../../../../../../tools/mysql_connect_differential.py) sends identical programs to the original Java class and the C++ class. Both use recording SQL connections, but the serialization and class methods are real. We compare results, exceptions, logs, SQL text and the order of every recorded effect.

All 1,598 programs, 5,399 operations and 60,287 SQL observations matched in the normal build (40.98 seconds) and the AddressSanitizer/UndefinedBehaviorSanitizer build (110.76 seconds). The corpus covers initialization and replacement failures, read-only DDL skipping, all public database operations, truncated wire data, partial range results, 14 coin bindings, decimal overflow, null owners and metadata, hidden token cells, Clob lengths, and 316 double values in seven locales. Independent assertions check the query directions and binding positions. Clock bindings are checked against each operation's actual before/after times before their values are normalized.

All five selected SQL regression suites also pass in both builds (42.11/118.87 seconds), covering SqlDB, TxPoW SQL, CoinDB, ArchiveManager and wallet commands.

A useful trace is a range containing one valid block followed by invalid wire bytes. The decoder can log an IOException and return null; the source appends that null to the returned list. Throwing immediately or dropping the entry would change observable behavior. A separate SQL failure after the first row returns the already accumulated prefix.

## Limits and a useful next test

This is an implemented_partial counterpart, not certified full parity. The recording driver proves calls and observable wrapper behavior, not MySQL's execution of those calls. No native MySQL driver is installed by this class; unconfigured initialization throws an explicit SQL error. Isolated live-server persistence, duplicate INSERT IGNORE behavior, demo-main execution, driver loading, full Main integration and exhaustive concurrency/platform parity remain open. The source demonstration main is not an assertion test. The readOnly flag skips table creation; it does not prohibit writes.

## Check your understanding

Why can saveBlock return true even when INSERT IGNORE adds no new row? Which ordering clause determines what loadFirstBlock actually returns?

Pinned Java SHA-256: `eea5c8d3727a177cfb6e67359830ad53b90d5f061cef5c723b2e0ae461af46e2`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
