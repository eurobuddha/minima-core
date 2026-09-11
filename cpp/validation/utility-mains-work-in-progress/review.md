## Code Review

### Summary
Reviewed the five added main methods, header declarations, formatter changes,
fixtures, driver and CMake registration. Each main reuses the actual native helper.
No POW iteration or source sample data was removed. BaseConverter's demonstrated
failure is retained, not converted into a successful alternate decode path.

### Findings
Source review identified a formatter corner case: Java substring(0,0) succeeds after
an unmatched initial quote and leaves currentpos zero. The native formatter formerly
returned through its fallback. It now preserves the source loop, with five isolated
child observations following an entry marker and no returned output. No child is
left running. Finite malformed cases continue to verify fallback log and notification
behavior. Java Error and native allocation failures are not swallowed as Exception.

Resolved fixture-only issues: missing direct Unicode include; a post-observation
Main.ClearMainInstance call mutated the shared captured log array. Removing that
unrelated cleanup call leaves actual formatter output fully compared. Saved initial
failure logs record the mismatch. No production behavior was weakened to pass tests.

### Verdict
Approve the scoped change. Both executable builds pass, five complete main programs
and all 36 formatter matrix cases match the stated scope, and both selected existing
regressions pass. Complete 457-file behavior/API parity remains unverified.
