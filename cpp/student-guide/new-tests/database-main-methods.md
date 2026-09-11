# Database demonstrations: from printed output to saved state

These three mains are small experiments inside existing database classes. Calling
them tests the source sequence, while reopening the actual files tests what survives
process exit. The [Java wrapper](../../support/DatabaseMainsOracle.java) calls the
pinned Java mains unchanged. The [C++ wrapper](../../support/database_mains.cpp)
supplies the existing native H2 provider and a temporary home directory. The
[driver](../../tools/database_mains_differential.py) gives each writer its own home
and lets both languages read every file.

## ArchiveManager: order changes identity

The main deletes `testfolder`, opens `sqlsync`, creates a block at height one,
sets its time, calculates its ID, and only then changes its first superparent to
`0xFFEEFF`. It closes the connection repeatedly between save/count/load/range calls.
The existing methods must reopen it. The range uses strict inequalities: height
one is outside `(0,1)`, so the range contains zero blocks.

The stored lookup ID describes the header before the parent change. Reading the
serialized block recalculates the ID from the changed header. The test decodes the
same wire with Java and C++, restores the initial parent in a separate decoded
object, and recalculates the old ID. It checks both relationships. Reordering those
two source statements would change behavior even though the database still stores
one row and the program still compiles.

The driver plants an old file inside the deletion scope and a sentinel beside it.
The first must disappear; the second must survive. No real user home is passed.

## TxPoWOnChainDB: an index is not necessarily unique

The main opens `testsql`, calls schema creation again, adds three block/TxPoW pairs,
and prints the first two lookup results. The schema has no unique constraint on
TxPoW ID. Running the main again therefore appends three rows: the observed counts
are three, six and nine. The test checks complete ordered row data and verifies
that earlier rows remain unchanged.

An initial test incorrectly derived row position from its auto-increment ID. The
real Java file contained IDs 1, 2, 3, then 33. The corrected assertion checks positive,
increasing IDs and uses actual row order for the expected three-value cycle. Both
readers must still agree on every exact stored ID. Identity allocation across the
Java process and C++ RAII lifecycle is recorded, but not claimed equivalent. This
is an example of why increasing identifiers must not be assumed consecutive.

Java has no explicit close in this main. Native owned handles are released when
the local database object dies. Durable rows and zero remaining native handles
are checked; equivalent garbage collection or shutdown-failure behavior is not.

## TxPoWSqlDB: the printed transaction must be the saved transaction

This main sets the parent before calculating the ID, prints the complete TxPoW,
inserts it as relevant, and explicitly saves the database. Both readers decode the
stored bytes, serialize them again, and compare the full JSON with the writer's
printed object. ID, parent, relevance, block/transaction flags and clock bounds are
checked. Repeating the main retains the earlier rows.

Each fresh TxPoW includes real random PRNG bytes. An initial comparison wrongly
expected two independently generated PRNG fields to be equal. The final comparison
checks the full 32-byte field through each writer's saved bytes and both readers,
then excludes that independently checked field from the cross-writer projection.
Time, formatted date, header-derived hash and superblock receive the same treatment.
No production randomness is replaced, and no distribution claim follows from a
few examples.

## What was adapted, and what remains open

The C++ mains accept `SqlDBServices` and an optional home path because native callers
already supply those dependencies explicitly. SQL text, codecs, retries, schema,
lookup methods and H2 implementation are reused. The database dependency is the
existing Java H2 compiled ahead of time, with no JVM process at runtime.

The cases include null, empty and unused arguments. Tests cover normal main behavior,
repeat runs and real saved-file interoperability. Arbitrary home properties, injected
provider faults, logger notifications, concurrency, random distributions and complete
class/API parity remain outside this focused evidence. The sanitizer instruments
C++, not the AOT H2 library.

Read the [saved results and limits](../../validation/database-mains-work-in-progress/README.md)
before reporting a pass. No new source counterpart is counted by adding these mains.

## Exercises

Why does the archive's stored ID differ from the decoded block's ID? Why does
`INSERT IGNORE` still append rows in the on-chain table? Which assertion would fail
if the C++ insertion forgot the relevance flag? Why can a test compare every saved
byte without requiring two independent random runs to produce identical bytes?
