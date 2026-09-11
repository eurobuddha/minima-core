# Code Review — Witness Streamable and list ownership

## Summary

The change follows the pinned Witness read order and reuses native Signature’s
shared-record/list pattern. It preserves child codecs, duplicate checks and wire
ordering. Production and fixture callers have been inspected at the changed access
sites; list handles are retained across iteration and contract verification.
The independent fixture computes partial list contents and validates real TxPoW,
file, hash and network-message callers. This is an incremental integration review,
not approval of complete Witness or node parity.

## Findings resolved before checkpoint

- Two TxBlock accesses still used vector syntax after its list getter changed.
  The initial full build rejected them; empty/front now use the owned handle.
- A mechanical range-loop adaptation initially targeted an outer wallet loop.
  Review corrected it before the successful build. All changed loops were inspected
  again; their initialization now owns the actual evidence list.
- The Python partial-list checker reused the top-level case counter name. Assertions
  completed but the report showed 1,037 executed versus 3,447 planned comparisons.
  The child counter now has a distinct name, and mismatched totals fail the suite.
  The initial raw report is retained, explicitly superseded by final validation.

## Remaining scope

Witness list handles own their allocation, but contained null elements and complete
Java collection operations are not represented. getScript still returns a pointer
that vector growth or removal can invalidate. ScriptProof still has value-backed
fields. Structural mutation during iteration does not reproduce Java fail-fast
behavior. Subclass dispatch, custom streams, allocation-failure timing and concurrent
mutation remain unverified. These existing gaps remain required future port work.

## Verdict

Approve the incremental ownership/interface implementation. Final normal and
sanitizer builds and all seven selected suites pass; evidence is in progress.json. Do not mark the
class or project parity complete.
