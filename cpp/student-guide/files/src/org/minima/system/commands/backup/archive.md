# archive — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/archive.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/backup/archive.cpp)

## Purpose

archive coordinates archive inspection, export, import, resynchronization and historical address lookup. Its actions share static temporary sources, so the whole class is more than a stateless command wrapper.

## Follow the command

1. integrity starts at the archive’s oldest block and loads ascending ranges of roughly 256 heights. For each consecutive pair check height increment first, then parent hash; an else-if counts at most one error per pair. Separately decide whether genesis or the cascade tip aligns with the block range. valid describes this alignment, while errors reports linkage problems.
2. resync requires completed keys, resolves either a network host or the special archiverestore source, and probes a remote host at block -1 before resetting. Post MDS_RESYNC_START, clear MegaMMR when enabled, then either retain wallet keys or clean/derive a supplied phrase, reset the wallet, create keys and set their use count.
3. Start at block zero and repeatedly wipe RAM TxPoWs and request/create the next IBD. Install a cascade only on the initial zero request. Advance to last received height+1, post archive processing, then poll for a tip and exact end height with attempt limits. Null responses become empty IBDs. Empty batches and timeout errors both leave the loop; the code then reports completed, saves and shuts down.
4. export delegates H2 gzip SQL backup. exportraw writes a GZIP stream containing a MiniByte cascade flag, optional cascade, MiniNumber declared block count and consecutive TxBlock encodings. If the archive begins at one it omits the cascade; otherwise a cascade moves export start to tip+1. The count comes from height difference and optional maxexport, not actual loaded row count.
5. Raw export writes a block before testing whether the count limit has been reached, so zero/negative maxexport can still emit one block with an inconsistent declared count. Missing ranges can also make declared count differ from records. Stream-close exceptions are logged rather than necessarily failing the command.
6. import selects raw only for filenames ending exactly .dat; all others restore an H2 temporary database. Set the matching static source and run nested archive resync, forwarding phrase/keys/keyuses/anyphrase. Normal completion closes/deletes the temporary source and clears its pointer; exceptions can skip cleanup. importold uses the older H2 path but does not explicitly reset the static format flag.
7. The separate importraw action resets the chain, posts batches directly and waits only every twentieth batch with unbounded exact-height polling. It does not explicitly wait for the final short batch before shutdown. inspectraw checks sequential heights beginning from zero, even when a cascade exists; a mismatch is logged and the response only contains duration.
8. inspect reads a supplied H2 backup through a temporary database, or current archive when file is absent, and reports cascade/endpoints/size. It rejects .dat and deletes the temporary folder even when inspecting current state. addresscheck scans archive ascending then retained chain descending, appending matching created/spent events under coins without deduplicating overlap. Optional statecheck normalizes Mx except strings containing @.
9. NotifyListener synchronously sends NOTIFY_POST containing ARCHIVEUPDATE and message data when a listener exists. sendArchiveReq frames an MSG_ARCHIVE_REQ in MiniData, uses ten-second connect/read limits, records payload byte counts, then reads a MiniByte and IBD without validating the response type byte. Exceptions retry up to the supplied count, sleeping ten seconds between attempts; exhausted/zero attempts return null.

## Translation and dependencies

The native class now maps all action branches, metadata/factory, NotifyListener
and both sendArchiveReq overloads. It reuses ArchiveManager/H2, RawArchiveInput,
IBD/TxBlock codecs, TxPoWProcessor, command parsing, date formatting and native
socket adapters. Temporary archives use MinimaDB's existing factory and the same
extracted IBDArchive adapter as normal runtime requests.

The two temporary source pointers use the existing mutex-protected shared-owner
pattern, retaining an object across a call while allowing later replacement. The
format flag is atomic. This prevents pointer-storage data races; it does not prove
concurrent stream or command parity. Static values are not unconditionally reset by
the production body. Native RAII releases resources without adding database rollback.

GzipOutput and BufferedFileOutput accept buffer sizes while preserving previous
512/8192 defaults. Raw export selects 65536 for both. Gzip finishing still attempts
to close the destination after a finish failure; the source catch logs ordinary
close failures. Exact source file/API parity remains unverified.

## Understanding the tests

The [pre-test lab](../../../../../../../../student-guide/new-tests/archive.md) explains
the distinct storage, parser, network and processor requirements. The
[Java oracle](../../../../../../../../support/ArchiveCommandOracle.java) and
[native fixture](../../../../../../../../support/archive_command.cpp) use copied
real H2 databases. Block fixtures reuse the existing ArchiveCache/ArchiveFixture
constructors, with linked parents, gaps, wrong parents and created/spent coins.
Native fixture block bytes are checked against Java before command comparisons.

The [driver](../../../../../../../../tools/archive_command_differential.py) compares
exact H2/raw gzip exports in both directions, integrity/inspect replies, malformed
files, partial outputs, parameter ordering, address records and listener messages.
A scoped recursive-resync probe records forwarded parameters and static state; it
returns an explicit failed result so source cleanup and nested failure handling are
observable. The registry is restored after each request. This probe does not test
real synchronization. The production archive body remains the full implementation.

Direct EOF/NPE/class-cast prose is normalized. Default filename timestamps must fall
within measured command bounds before replacing the name for file comparisons.
Reported inspectraw duration must match the source format and measured time bound.
Content hashes, error categories and static state remain compared. The first empty
integrity assertion was corrected: source defaults permit valid:true even with zero
blocks. Saved failed observer reports document that correction.

Both builds pass all 509 cases each, including 40 raw exports and six H2 exports.
Eight selected dependency regression suites also pass in both builds.

See [evidence](../../../../../../../../validation/archive-command-work-in-progress)
for the current validation results; these do not certify every synchronization or
network branch. Real-node import/resync and loopback request/retry tests remain
separate required work.

## Limits

Integrity checks adjacency/alignment, not all consensus rules. Static temporary
sources and cleanup without finally make concurrent/repeated invocation important.
Several success messages survive incomplete processing. Real-node synchronization,
network retries/timeouts, cascaded exports, concurrent streams, malformed object
serialization failure boundaries and full API parity remain unverified.

## Check your understanding

Construct an archive with consecutive heights and a wrong parent hash: how can integrity.valid be true while integrity.errors is nonzero?

Pinned Java SHA-256: `a7bdbe6083655474292fc8782885efd98ec37fa87c92b89864f07696b223ae31`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
