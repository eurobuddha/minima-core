# Reading a SPHINCS signature without losing its partial state

The pinned SPHINCSSignature is an envelope, not the signing algorithm. Its wire
format is simply `Signature || MMRData || FORSSignature`. It adds no tag, version,
or outer length. Its constructor stores the three supplied object references.

There are three distinct read boundaries. The WOTS field changes only after its
whole Signature has been read. The root changes only after its whole MMRData has
been read. The FORS field, however, is replaced **before** its nested read starts.
A short input can leave old fields, new complete fields, and a new partial field
in the same object. A transactional all-or-nothing parser would change Java's
observable behavior.

## Checks planned before execution

1. Compare constructor/getter identity, all eight null-component combinations,
   and mutations through the original component references.
2. Build small wire examples independently in Python. Check every truncated prefix
   and the consumed offset, field identity, partial JSON/write state, and errors.
3. Wrap an actual full-size FORS signature saved by the Java/native FORS comparison.
   Sample throughout the larger envelope without shrinking its forest or proofs.
4. Check trailing bytes, negative and wrapped counts, oversized hash lengths, and
   malformed BigIntegers. The converter logs/catches only I/O errors and returns
   the partial envelope; runtime errors escape.
5. Fail a WOTS proof write after its key and signature bytes. The new incremental
   Signature/SignatureProof writers must keep that output prefix, using the same
   codecs and shared MMR writer as successful serialization.
6. Regress the existing object tests in both complete builds. Container tests do
   not claim that a synthetic WOTS signature authenticates the FORS root; full
   SPHINCS signing/verification integration remains separate unfinished work.

## Completed execution

Both complete builds passed. Each runtime comparison passed **1,449 cases and
158 independent assertions**, with identical normalized transcript hashes. Normal
and sanitized driver times were 6.930s and 23.303s. The 9,464-case object regression
also passed in both builds; the full 241-entry CTest suite was not run.

[Saved evidence](../../validation/sphincs-signature-work-in-progress/progress.json)
contains reports and build/test logs. The actual FORS input is retained in the
[normal FORS vectors](../../validation/fors-work-in-progress/normal/fors-known-vectors.json),
so these tests need no expensive forest reconstruction merely to check the envelope.

Current key classes'
null SignatureProof entries and raw Java MiniData byte-array aliases, arbitrary
subclasses, concurrent mutation and arbitrary output-stream failures remain outside
this validation. Full file and node parity are not certified.
