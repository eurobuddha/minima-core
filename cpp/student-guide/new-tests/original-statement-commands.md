# Original statement commands: control flow and contract outcomes

This checkpoint maps RETURNstatementTests, WHILEstatementTests, IFstatementTests
and MASTstatementTests. Every active source fixture and assertion remains. The
native statement classes already implement the exercised behavior, so this batch
changes tests and teaching material, with no production algorithm correction.

## Reuse and translation

The pinned Java tests and production command files were read first. Existing
StatementBlockTests.cpp supplies the direct statement execution and exact
ExecutionException pattern. OriginalFunctionTestSupport.hpp supplies counted
assertions. The mapped commands reuse Contract, ConstantExpression,
VariableExpression, BooleanExpression, OperatorExpression, StatementBlock,
LETstatement, ScriptProof, Witness and the existing tokenizer/parser.

Java object references become shared_ptr objects. ArrayList<Statement> becomes
a vector of StatementPtr values; all lists in these fixtures are populated before
StatementBlock construction. No case mutates a list afterward, so these tests do
not certify source list-alias behavior. Unexpected exceptions fail the executable;
only ExecutionException satisfies an expected rejection. Source try/fail guards
become ordinary calls whose exceptions propagate as test failures.

The inspected sibling `tools/minima-core-cpp/src/kissvm/Interpreter.cpp` returns
arbitrary value types, uses an extra 512-iteration limit and treats MAST as EXEC.
Its instruction accounting also differs. It cannot replace the pinned command
classes; the current native command implementations are reused unchanged.

## RETURN: a Boolean result is required

Eleven constructor assertions retain exact display strings, including trailing
spaces for empty HexValue and StringValue. Execution checks TRUE and FALSE,
then rejects four hex values, three numbers and two strings. Each rejection
also confirms that success remains unset and false. That distinction matters:
a directly called invalid RETURN throws before setting the contract outcome.
The top-level Contract.run wrapper handles exceptions separately.

RETURN has 42 source assertions: 11 constructor checks and 31 execution checks.
Numbers are not interpreted as truth values. The source comments questioning
empty display strings do not authorize changing those expected strings.

## IF: evaluate conditions in order

The constructor fixture calls its false constant CheckTrue and builds one block
from the other block's list. Both source choices are retained. The expected
string is IF FALSE, ELSEIF FALSE, ELSEIF FALSE.

Seven execution cases include an empty IF and branches with the first TRUE in
different positions. Empty IF does nothing and consumes zero instructions when
called directly. A selected RETURN TRUE branch sets success. The source checks
instruction counts of 3, 4 or 5 as more conditions are evaluated. There are 22
assertions overall. The commented expectation that empty IF should throw stays
inactive; the active source explicitly expects normal completion.

## WHILE: retain the test and expose its coverage limit

The constructor builds a condition a LT 5 and body a=a+1, then checks its display
string. Execution constructs a real block that starts a at zero and loops to
five. A second block returns TRUE after the first increment. Both source cases
only fail on an exception; neither asserts the final value or contract outcome.
Thus WHILE contributes one source assertion, plus two active execution fixtures.
Reporting more original assertions would overstate the source's coverage.

Eight supplemental fixed scripts reuse KissVMOracle and minima_kissvm. They
independently require a=5 after normal termination, a=1 after early TRUE/FALSE
returns, and a=0 when the initial condition is false. Other scripts check ELSE,
ELSEIF and that statements after RETURN do not override the outcome. Both
runtimes must match all 11 run-record fields, including instructions, stack depth,
variables and trace. Sixteen expected-outcome checks prevent matching generic
errors from passing. These checks supplement rather than replace source cases.

## MAST: select code from witness by its address

Two constructor cases check display strings. Five execution fixtures cover an
empty script, RETURN TRUE, RETURN FALSE, malformed Hello World, and an address
whose proof is absent from the witness. Every fixture constructs a real
ScriptProof and uses its derived address. The missing-proof fixture deliberately
does not add it to Witness. Success flags and instruction counts are retained;
malformed and missing scripts require ExecutionException. There are 19 assertions.

## Validation and limits

| Class | Original methods | Original assertions |
| --- | ---: | ---: |
| RETURNstatementTests | 2 | 42 |
| WHILEstatementTests | 2 | 1 |
| IFstatementTests | 2 | 22 |
| MASTstatementTests | 2 | 19 |

All eight annotated methods and 84 source assertions per runtime pass in normal
and sanitizer builds. The direct Java runner checks the exact annotation set,
default options and absence of lifecycle methods before invoking unchanged Java
methods on fresh instances with real JUnit assertions. Java assertion totals
come from reviewed successful source paths; native checks are counted at runtime.
The eight supplemental comparisons / sixteen independent checks also pass.
Normal CTest took 3.61 seconds and sanitizer CTest 4.58 seconds. Both node targets
build. The full 277-test suite was not repeated for unchanged production code.

```sh
cmake --build cpp/build --target RETURNstatementTests WHILEstatementTests IFstatementTests MASTstatementTests minima_kissvm MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_statement_commands_tests$' --output-on-failure
```

Use cpp/build-sanitized for the configured sanitizer build.
[Evidence](../../validation/original-statement-commands-work-in-progress/progress.json)
includes results and remaining files. Full command API, alias mutation, resource
limits and complete node parity remain unverified.

Exercise: derive IF's instruction count when two FALSE conditions precede a
TRUE condition whose action is RETURN TRUE. Then explain why successful WHILE
execution alone cannot prove that the loop ran the expected number of times.
