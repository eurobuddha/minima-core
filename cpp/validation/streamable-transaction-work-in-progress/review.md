# Incremental Transaction review

Reviewed pinned Transaction, Crypto.hashObject, TreeKey.verify and Winternitz.verify,
current native implementations, shared-record predecessors, source/current tests,
sibling Transaction and changed production caller paths. Reused established codecs,
validation, logging and proof algorithms. No dependency replacement.

The relevant distinction is object ownership, not merely identical bytes. The Data
record keeps flags, current ID and replacement lists together. Retained getters own
old lists. Child reads append only after successful factories. The common writer
calls children directly. Missing state removal keeps identity/order unchanged.

Review identified nullable verification as an additional required adaptation:
empty/mismatching/overlong proofs return before null data is read; a matching final
proof reaches the error. The actual checker accepts an empty witness without reading
the null ID. Seven final differential cases cover those paths. Native Error/bad_alloc
escape hashing; Exception logging precedes the cached-null assignment failure.

All selected normal and sanitizer regressions pass; see progress.json and saved
reports. Existing TransactionTests assertions are preserved with owned-getter syntax.
SPHINCS uses existing full signature replay, not newly generated signatures.

Approve this incremental change for a local checkpoint. This is not full-parity
approval. Nullable list elements, child/StateVariable pointer identity, full collection
APIs/fail-fast behavior, shared constant identity, subclasses/custom streams,
reentrancy/concurrency and allocation paths remain open. Value adapters reject null
but all downstream timing/lifetime paths still need complete audits. State-size
exception logging is source-following code without a representable failing-child
fixture. H2 AOT is not sanitizer-instrumented. Full 261-suite CTest was not run.
