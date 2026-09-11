# One interface, different byte formats

An interface promises operations, not a universal encoding. The Java Streamable
interface has two methods: append an object's bytes to an output stream, and read
fields into the existing object from an input stream. C++ now expresses these as
virtual methods using the repository's byte-vector output and span/offset input.
A virtual destructor permits safe destruction through the common base type.

Five existing classes initially implement the interface: MiniByte, MMRProof,
HORSTSignature, FORSSignature and SPHINCSSignature. The proof/signature methods
keep their existing bodies. MiniByte reuses the shared byte reader and appends
its single byte. Other source implementers still need to join this common path;
adding the interface file alone does not complete that integration.

The new test holds each object as Streamable, writes after an existing prefix,
then reads a different encoding into the same object. It tries every truncation
boundary and examines the offset and object state after an exception. A failed
read can leave some fields changed: replacing the whole object only on success
would be different behavior.

Generic callers also receive Streamable references: MiniFile for persisted bytes
and in-place loading, Crypto for hashes, MiniData conversion, and NIOManager for
message construction. Comparing their exact Java outputs demonstrates that the
interface is used beyond a standalone example. No live network is necessary to
check a constructed network-message payload.

A synthetic implementation deliberately writes a prefix then throws IOException.
It checks virtual dispatch, preserved partial output and exception propagation.
It is a test object, not a substitute for any missing production implementation.
Vector/span operations do not yet model every possible custom Java stream,
blocking I/O, allocation failure or concurrent access. Both complete builds pass.
Each interface suite passes 909 comparisons and 3,850 assertions, with matching
transcript hashes and empty native stderr. All six affected regressions pass in
both builds. [Saved evidence](../../validation/streamable-work-in-progress/progress.json).

For students: why must readDataStream modify the same object? Why is the offset
a reference? Which bytes should remain in the output if a writer fails after
appending two bytes? Why can two Streamable classes use entirely different formats?

The [MMR/signature follow-up](streamable-mmr-signatures.md) extends this same suite
to 11 types, passing 1,299 comparisons and 6,290 assertions per build. The five-type
results above describe the initial checkpoint. Another 33 explicit source
implementers still need integration.

The [scalar follow-up](streamable-scalars.md) brings adoption to 14 of 44 explicit
implementers. Both expanded suites pass 2,095 comparisons and 12,220 assertions;
the earlier results above are historical checkpoints. Thirty implementers remain.

The [transaction-object follow-up](streamable-transaction-objects.md) brings adoption
to 20 of 44 explicit implementers. Both expanded suites pass 2,771 comparisons and
20,914 assertions; the earlier results above are historical checkpoints.

The [Witness follow-up](streamable-witness.md) brings adoption to 21 of 44 explicit implementers. Both suites pass 3,447 comparisons and 31,650 assertions, plus six affected regressions. Earlier results above remain historical checkpoints.

The [Transaction follow-up](streamable-transaction.md) brings adoption to 22 of 44 explicit implementers. Both interface suites pass 3,782 comparisons and 39,522 assertions. Both full builds, all 21 selected CTest suites and saved SPHINCS replays pass. Earlier figures remain historical checkpoints.

The [body ownership](streamable-txbody.md) and [checked ID iteration](txbody-id-iteration.md) follow-ups bring explicit interface adoption to 23 of 44 types. Both current suites pass 7,623 comparisons and 95,664 assertions; the 23 affected normal regressions also pass, with exact sanitizer scope in the linked evidence. Earlier counts are historical checkpoints.
