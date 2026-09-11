# Lab: archive storage, streaming and synchronization

Written before the new tests. Follow the complete pinned archive.java, including
undocumented actions importold, importraw and inspectraw. The class holds static
temporary H2/raw sources used by recursively dispatched resync commands. An action
is not complete merely because a command name is registered or a file is written.

Reuse native ArchiveManager, RawArchiveInput, IBD, TxPoWProcessor, SqlDB and socket
adapters. Supply temporary archives through the existing MinimaDB SQL factory;
extract its existing IBDArchive adapter so temporary archives use the same code.
Raw export needs the same GzipOutput and BufferedFileOutput with explicit 65536-byte
buffers; retain their existing 512/8192 defaults for encrypted backups.

Compare integrity's two distinct conclusions: parent linkage error count and
cascade/genesis alignment. Test empty archives, gaps, wrong parents and cascades.
Export/import H2 gzip SQL in both runtimes. Decode raw gzip payloads independently
and in the opposite runtime; verify header, optional cascade, declared count and
actual TxBlock bytes. Include maxexport zero/negative and gaps: source may emit
one block despite a nonpositive declared count. Compare partial files on errors.

Use copied real-H2 fixture directories. Inspect validates suffix before deleting
the temporary folder, but deletes that folder before checking an accepted file's
existence. Check empty file parameter, missing files and obstructed directories.
Use actual nested CommandRunner calls to observe forwarding and source cleanup.
Test repeated format choices, static pointers and failures; do not add finally
cleanup to the production body just to make tests simpler.

Address lookup scans archive ascending and current tree descending. Test duplicate
overlap, created/spent records, state filters and Mx conversion. NotifyListener
must emit the exact nested ARCHIVEUPDATE envelope and propagate listener failures.

For network requests use only loopback fixtures. Compare MSG_ARCHIVE_REQ wire bytes,
MiniData length handling, payload-only traffic accounting, ignored response type,
null/error response and retry count. Tests with retries must preserve the ten-second
source delay unless a separately documented clock boundary controls both runtimes.

Synchronization must eventually be exercised with the real processor and a real
node: install a cascade, process successive batches, replace wallet keys when a
phrase is supplied, save and reopen. Distinguish resync's bounded waits and reset
every tenth batch from importraw's wait/reset every twentieth batch and lack of a
final-short-batch wait. A success message alone does not prove every block applied.
Record narrow comparisons and full-node evidence separately. Full parity remains
unverified until all methods, workflows and inherited dependency behavior are proven.

The first independent integrity assertion expected an empty archive to be invalid.
Both runtimes returned valid:true. The source initializes start/end to zero and
cascade start to -1; its alignment check accepts -1 in [-1,0]. The test assertion
was corrected and the failing observer report retained. Production code was unchanged.
This is another reason to read valid and errors as source-specific fields rather
than treating the word valid as proof of a usable archive.

A real comparison then exposed NUL truncation in a missing-file exception. The
legacy JavaCheckedException formatter rebuilds its message from what(), a C string.
The already-tested JavaTypedException carrier stores a length-aware std::string.
Archive's three missing-file exceptions now reuse that carrier with class
java.lang.Exception, preserving the entire path and the source exception category.
The original failing report is retained. Other legacy exception carriers still
need their own complete message-domain parity review.

The address fixture initially passed empty synthetic MMR proofs into tree-node
construction, which applies proofs before the command starts. That was not a usable
lookup fixture. It now constructs the node from the empty TxBlock first and then
attaches the query-only created/spent records through the existing mutable lists.
The archive command reads those records without recomputing MMR. These observations
test ordering/filtering, not consensus validity of the synthetic coin proofs. The
initial construction-failure report is saved; valid-chain processing needs the
separate actual-node test.
