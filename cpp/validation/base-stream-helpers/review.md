# Code review

Self-review using the code-review skill; no independent reviewer is claimed.

The source-named helpers delegate to the existing readers/writers rather than
introducing alternative serialization. Expected-length and hash-bound checks
remain in the reused readers. MiniData version conversion routes a Streamable
through its virtual writer and the existing IO-only catch/log/null helper. Null
arguments throw and runtime exceptions propagate. Decimal access preserves the
unbounded carrier by value, following the existing immutable-snapshot adaptation.

The MiniByte correction preserves each global object's address while permitting
in-place reads; isFalse consults current FALSE as Java does. Constructor/writer
behavior is unchanged. The fixture varies the shared value and restores it before
subsequent cases. Its malformed-read cases check both exception class and consumed
offset; exact Java/native reply comparisons remain in addition to independent
expected fields. Logger observation intentionally checks the expected IO error,
not timestamp or stack-frame formatting.

No blocking finding in this change. Full virtual override dispatch, arbitrary
stream implementations, nondefault MINIMA_CHARSET and concurrent mutation remain
explicit API audit work. A successful finite corpus is not whole-class proof.
Verdict: approve with those existing scope limits retained.
