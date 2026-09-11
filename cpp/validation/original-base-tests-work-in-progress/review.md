## Code Review

### Summary

Scoped review of both mapped base test files, shared formatting helper, Java/native API fixtures, Python drivers, CMake changes, MiniData/MiniNumber API additions and BigInt comparison correction, relevant comparison consumers, changed lesson sections and source-map/evidence inventory. All original active calls and assertion paths are retained, including full append loops and discarded static-reader results. Public decimal methods reside in their respective mapped class .cpp files and reuse the existing unbounded carrier.

### Findings

#### 🟠 MAJOR — Public comparison leaked byte difference (fixed)
**File:** `cpp/src/org/minima/objects/base/MiniNumber.cpp`, BigInt::compareTo
**Problem:** Equal-length magnitude comparison returned memcmp's raw byte difference, so comparing 2 to 0 yielded 2 instead of Java's exact 1. Existing sign-only assertions did not detect this. The initial extra corpus found 336 direct mismatches per build and 336 corresponding native expected-field failures.
**Fix:** Normalize the public comparison result to -1/0/1 before sign reversal. Keep internal magnitude arithmetic sign tests intact. Inspected callers use signs or expose comparison results; downstream MiniData, MiniNumber and MMREntryNumber exact results now match Java. Both API corpora and existing primitive regressions pass.

No unresolved blocker found in this bounded scope. Decimal views copy unscaled value/scale without imposing monetary limits or rerounding. Byte comparisons use the existing deliberately retained numeric cache, while byte equality uses actual bytes. Bounded hex appends two dots even without truncation and reports negative substring bounds accurately. Original printed labels are retained, with only validated identity/random observations normalized. Source TooBig executes zero assertions and remains explicitly documented as such. Native non-null smoke checks materialize value results as owned objects; they are not relabeled as content equality checks. The original-test runner validates annotation membership/defaults and uses real JUnit assertions. Full heap-failure, mutable Java array/decimal identity, JDK BigDecimal and complete node/API behavior remain unverified.

### Verdict

✅ Approve for an incremental local checkpoint with recorded focused evidence. This is not a full MinimaCoreC++ parity or release certification.
