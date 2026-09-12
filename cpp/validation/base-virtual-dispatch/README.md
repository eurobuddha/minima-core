# Base value virtual dispatch

Corrects two existing counterparts; no newly mapped Java/C++ file. Source pin:
5564b9d78978efcaf23679190d05253961fc6e69. Development baseline: 9fa16e1.

MiniByte now declares all six source value/query methods virtual and equality
calls getters in Java order. MiniString declares its three source value/query
methods virtual; equality and its explicit copy constructor call toString.
The checked Streamable implementations are reused. MiniByte's writer retains
field access; MiniString's writer retains its getData call. Native byte helpers
and assignment remain extra APIs, outside Java source mapping.

The actual pinned Java and native classes run through derived fixtures. All 1,853
prior base API cases are retained and 2,328 subclass cases added. Independent
expected fields cover values, raw/prefixed wire bytes, ordered override calls and
exact propagated exceptions. The baseline built successfully but differed on
1,610 cases. No Java independent expectation failed; baseline-results.json retains
all mismatches. In the final fixture, C++ override specifiers additionally check
that the intended methods really override production declarations.

Final build and test reports are named by configuration. No installed-preview
replacement, full-suite pass or complete class parity is implied. In particular,
nullable string/reference values, MiniString.MINIMA_CHARSET and broader subclass
semantics in MiniData, MiniNumber and MMREntryNumber remain open.

See [the student lesson](../../student-guide/new-tests/base-virtual-dispatch.md)
and [the next API audit leads](next-api-audit.md).

## Verified results

| Check | Normal | Optimized | ASan/UBSan |
| --- | --- | --- | --- |
| Actual MinimaCoreCpp node build | PASS | PASS | PASS |
| Expanded base API | 4,181 PASS | 4,181 PASS | 4,181 PASS |
| Independent API assertions | 40,534 | 40,534 | 40,534 |
| Primitive differential | 3,802 PASS | 3,802 PASS | 3,802 PASS |
| Streamable differential | 7,705 PASS | 7,705 PASS | 7,705 PASS |
| Native MiniByteTests | PASS | PASS | PASS |
| Original MiniString tests + separate helper | PASS | PASS | PASS |

Each Streamable run checks 96,172 independent assertions. Each original string
run executes two annotated methods with five successful-path assertions, plus
its unannotated helper separately (one assertion). All build/test sessions exited
zero. API transcript SHA-256:
`d4f725776a526ab33407b7d6bf11471bc3d4f0b5936e4948e3c1854c65e519bf`.
See results-summary.json and the complete reports. The existing OpenSSL padding
helper deprecation warning persists; no new warning was observed.
