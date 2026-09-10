# Lab: the four original casting test classes

Written before running the new mapped tests. The pinned files are
`test/org/minima/tests/kissvm/functions/cast/{BOOLTests,HEXTests,NUMBERTests,SCRIPTTests}.java`.
Read all four classes, the four cast implementations, their Value classes,
ConstantExpression, and the function registry and parameter-count checks.
Reuse the existing LENTests native harness pattern and unchanged-JUnit driver.
The sibling script-level casting examples do not replace these class assertions.
No new cast implementation is needed for the translation of these tests.

Each file calls a fresh prototype’s getNewFunction and registry getFunction,
checking the name and zero supplied parameters. A Contract with empty script,
witness and transaction supplies the execution context. Each input becomes a
ConstantExpression, and each function execution uses a fresh parameter list.
Keep result-type checks separately from result-text checks: matching text alone
would allow a value of the wrong type to pass.

- BOOLTests has 21 executed assertions across three methods. Booleans keep their
  truth value, zero hex/numbers are false, and StringValue is false only for the
  exact text FALSE among these examples. Zero arguments raise ExecutionException.
- HEXTests has 24 assertions. It checks Boolean bytes, unchanged hex, 0/1/65535
  as unsigned hex, and the UTF-8 bytes of two case-sensitive strings. It checks
  both zero and two arguments. Historical comments saying a test fails are not
  disabled assertions: run the unchanged expectations and report what happens.
- NUMBERTests has 19 assertions. It checks Boolean numbers, unsigned hex including
  2^64-1, and signed/positive numeric values. Two ordinary nonnumeric strings must
  throw NumberFormatException specifically, not any arbitrary exception. Zero
  arguments instead raise ExecutionException.
- SCRIPTTests has 23 assertions but actually tests STRING, including that registry
  name. It converts values to their displayed text; hex remains text beginning
  with 0x rather than being decoded into letters. Commented lowercase assertions
  are excluded; active mixed-case expectations are retained.

The native files stay at the same mapped package paths, including SCRIPTTests.cpp.
A small Java launcher calls all three unchanged @Test methods for each file.
The existing comparison driver compiles the pinned tests and executes both
runtimes. These 87 assertions teach the original examples; they are not exhaustive
cast coverage, integer-range proofs, alias tests or a full VM parity certificate.

Final evidence: both full builds and all eight selected CTest entries pass (7.70s normal, 7.68s sanitizer). Both builds’ four original-test JSON reports confirm 3 methods per class and 21/24/19/23 assertions per runtime, respectively. No cast implementation changed. Source comments claiming failure are stale for the executed examples; no active assertions were removed.

Review refinement: retain the source’s explicit result-class casts when reading text, using native dynamic_cast after the type assertion. A forged type tag alone must not satisfy an assertion that Java follows with a class cast. The four mapped tests are rerun after this refinement.

After retaining explicit result casts, all ten selected checks pass (11.35s normal, 11.12s sanitizer): the eight new casting checks plus existing LEN and MinimaFunction original-test comparisons. Both builds’ six final JSON reports were inspected. No compiler warnings were observed.
