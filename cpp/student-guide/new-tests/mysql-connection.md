# Lab: the MySQL dependency used by Main

Written before new tests. Main directly constructs MySQLConnect when backing up
TxPoWs, and its startup configuration helper checks a read-only connection. Read
the pinned MySQLConnect.java (763 lines), not the later working checkout’s Main.
The current archive/coin/TxPoW SQL components and recording drivers supply tested
connection, statement, row and wire handling. The sibling SQLite wrapper has a
different dialect and storage contract; it cannot serve as this MySQL class.

Preserve initialization order, four table definitions, fourteen prepared queries,
and the read-only branch that skips table creation but still prepares queries.
Compare partial initialization: a failed replacement leaves previously assigned
fields in place. Do not automatically reopen a closed connection; this source
does not use SqlDB.checkOpen. First/last names select descending/ascending heights,
respectively. Range reads use block>=start, ascending order and LIMIT 250.

The source catches SQLException in selected methods, while runtime errors escape.
A failed range read returns the list accumulated before the SQL error. A wire
decode can return null and that null remains in the list. saveCascade propagates
SQL errors and appends a row without deleting older cascades. The generic SqlDB
query helper cannot be reused whole: its catch scope and error JSON differ.

Coin inserts have fourteen ordered bindings, including amount/tokenamount as
IEEE-754 doubles. Search results use metadata names as-is, omit null cells, handle
Clob text, and format declared Double cells with six fractional digits. Hiding the
exact lowercase token column substitutes text before checking the metadata type:
this can intentionally change a later cast’s result. Follow the real source.

Reuse the existing Java floating decimal conversion and SQL metadata/clob paths.
The local JDK26 FormattedFPDecimal source confirms that %f rounds the decimal
selected by Double.toString, then applies fixed precision and locale symbols.
Test ties, negative zero, extreme finite values, NaN/infinity, locales, wrong
metadata types and failures after partial SQL effects. Keep fixed-format rounding
separate from MiniNumber’s blockchain magnitude limits.

Use source-generated TxPoW/TxBlock/Cascade/Coin wire and recorded SQL. Independently
check schema/query order, first/last ordering, counts, binding values, optional
credentials, read-only initialization, null decoding and failure boundaries.
Compare exact JSON plus logs/events. Real timestamps must lie between fixture
before/after observations; normalize only those clock-dependent bindings.
These checks do not establish a live MySQL driver, durable storage, full Main
startup or exhaustive concurrent database behavior. The full runtime remains the
goal; this class is a prerequisite for it.

## Observed result

Both full builds succeeded. The complete 1,598-program corpus matched Java in
normal and sanitizer builds: 5,399 operations and 60,287 SQL observations per
runtime, taking 40.98 and 110.76 seconds respectively. No production discrepancy
was found by these cases. The first build exposed a missing complete JSONArray
include; adding that include fixed compilation. The SQL recording fixture was
extended with a genuine Double value and setDouble event, retaining the existing
Clob/string and SQL failure paths. Those recording tests did not exercise a live server. The later [native MySQL lab](native-mysql-storage.md) adds a real compiled driver and isolated storage checks.

The shared adapters also passed five regression suites in both builds: SqlDB
(464 programs), TxPoW SQL (2,592), CoinDB (1,350), ArchiveManager (3,333) and
wallet commands (675). Their normal/sanitizer batches took 42.11/118.87 seconds.
This checks for accidental changes to existing SQL users after adding Double
cells and double bindings; it does not add a database engine.
