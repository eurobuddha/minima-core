# Original LET tests: storing evaluated values

LET has two constructor forms. A variable assignment stores the evaluated value
under a name. An array assignment evaluates numeric positions, joins their
canonical number strings with commas and stores the value under that key. GET
uses the same key convention. The display string describes expressions without
running them; that distinction explains the original tests' coverage.

## Reuse and exact translation

The pinned LETstatementTests.java and LETstatement.java were read, along with
native LET, Contract variable storage, Boolean/Operator/Function/Global/Variable
expressions and GET. Existing statement command tests provide the direct execution
pattern, counted assertions and annotation-checked Java runner. Java references
become shared_ptr objects; expression lists become vector<ExpressionPtr> values.
The source does not mutate these lists after constructing LET. No production
algorithm changed. The sibling Interpreter.cpp's LET supports a named AST
assignment and does not provide the pinned array constructor/display API.

Twenty constructor assertions cover Boolean expressions, number constants,
unevaluated GET/DEC/CHECKSIG calls, global references, arithmetic expressions,
variable references and two heterogeneous expression lists. Exact display strings
are retained, including punctuation inside function descriptions. Constructing a
zero-parameter CHECKSIG for display does not run signature verification. Similarly,
@BLKNUM is displayed without claiming it is a populated runtime global.

The execution method has nine fresh Contract fixtures: three Boolean expressions,
three numeric constants and three arithmetic expressions. Each executes LET and
casts the retrieved value to the expected runtime type before checking its string.
The minus-one fixture also repeats its constructor display assertion, yielding ten
execution assertions and thirty overall. Native dynamic_cast preserves the source
type requirement instead of accepting an unrelated value with the same spelling.

The original function, global, variable and array execution examples are commented
out. They stay inactive. In particular, a displayed Boolean array index is not
proof that execution accepts Boolean positions: the production code requires numbers.

## Supplemental array checks

Four fixed scripts run through the existing KissVMOracle and minima_kissvm probes:

- Store and retrieve a two-dimensional key (1, 2).
- Store and retrieve a negative and fractional key (-1, 0.5).
- Overwrite an existing key and retrieve its new value.
- Compute a key from a variable expression, then retrieve it by the resulting number.

Every script returns the independently expected TRUE result without exceptions.
Both runtimes must also match all eleven record fields: parse/outcome flags,
exceptions, instructions, stack depth, variables and trace. Four comparisons and
eight independent expected-outcome checks pass in each build. They supplement
all original assertions; they do not replace the inactive examples with weaker
claims or certify every malformed key and alias-mutation case.

## Results and reproduction

The unchanged Java tests and native counterpart pass both annotated methods and
all thirty source assertions in normal and sanitizer builds. Java's assertion
count is derived from reviewed successful source paths; native checks are counted.
The runner verifies the exact method annotations/defaults and absence of lifecycle
methods before invoking real JUnit assertions on fresh instances. Normal CTest
passed in 2.69 seconds; sanitizer CTest passed in 2.71 seconds. Both node targets
build. Full LET API and node parity remain unverified.

```sh
cmake --build cpp/build --target LETstatementTests minima_kissvm MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_let_tests$' --output-on-failure
```

Use cpp/build-sanitized for the configured sanitizer build.
[Evidence](../../validation/original-let-tests-work-in-progress/progress.json)
contains results and remaining files.

The unrelated saved Magic sanitizer full demo ended with a 7200-second timeout
and empty stderr; its complete-run validation failed. The normal full replay
had passed earlier. [Timeout evidence](../../validation/original-let-tests-work-in-progress/magic-sanitizer-timeout/magic-demo-replay-results.json)
is preserved separately and does not alter the passing LET results. Partial
buffered output cannot establish completion or an output mismatch after a full run.

Exercise: explain why LET (1 2)=42 and GET(1 2) agree on the key, then distinguish
a constructor display test from an execution test for LET (TRUE 1)=42.
