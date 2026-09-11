# Original parser tests: constructing code is different from executing it

ExpressionParser turns tokens into expressions. StatementParser groups expressions
into assignments, branches, loops and other commands. These original tests chiefly
check construction. A parser can accept TRUE + 543 or a non-Boolean condition even
though execution later rejects its value types. A faithful port preserves that
separation rather than making the grammar enforce new runtime restrictions.

## Reuse and source preservation

Both complete pinned parser test files and production parser bodies were read.
The current native ExpressionParser, StatementParser, ScriptToken,
ScriptTokenizer, LexicalTokenizer, Contract.cleanScript and expression/statement
classes are reused unchanged. Existing direct JUnit runners and counted test
helpers supply validation. The inspected sibling
`tools/minima-core-cpp/src/kissvm/Parser.cpp` has different Boolean/modulo precedence,
LET-array support, function-call syntax and boundary/error behavior; it cannot
replace the pinned parser APIs.

Java lists become vectors of ScriptToken values. The original tests call
cleanScript before tokenization wherever written, and retain handcrafted tokens
where written. No cases are deduplicated. Expected MinimaParseException checks
catch only that native exception; another exception fails the executable. Original
try/fail success blocks become ordinary calls whose exceptions fail the test.

## ExpressionParserTests

The first method loops over all twelve Boolean operators. Each operator retains
26 successful expressions, checking both the exact expression class and display
string, followed by two required rejections. The arithmetic method retains the
same structure over all ten operators. Operands cover Booleans, hex, integers,
positive/negative decimals, variables, scripts, functions, globals and redundant
parentheses. Hex values become canonical value displays, while variable/global
and function descriptions retain their distinct forms.

NOT and NEG each retain fourteen successful cases and four rejection cases,
including the duplicated malformed inputs. The final method appends an invalid
token and constructs an invalid token inside parentheses. The commented examples
remain inactive. Exact native type identity uses typeid, not a permissive base
class cast; the helper also fails cleanly if an expression is absent.

| Method | Parser calls | Assertions |
| --- | ---: | ---: |
| Boolean operators | 336 | 648 |
| Boolean unary | 18 | 32 |
| Arithmetic operators | 280 | 540 |
| Arithmetic unary | 18 | 32 |
| Explicit invalid tokens | 2 | 2 |
| Total | 654 | 1,254 |

These are 600 successful constructions with two assertions each, plus 54 required
parse exceptions. Runtime validity of every constructed expression is not asserted.
For example, constructing TRUE LT TRUE is separate from evaluating its numeric
ordering requirement.

## StatementParserTests

Ten annotated methods retain 93 direct parser calls: 78 accepted constructions
and fifteen required parse exceptions. The accepted cases are smoke checks: the
source does not inspect block contents or run them. The mapped test preserves
those calls without pretending they contain successful-outcome assertions.

The fixture set covers empty input, LET variables/arrays, EXEC, MAST, IF, WHILE,
ASSERT, RETURN and an unknown command. IF preserves forty accepted forms across
ten conditions; WHILE preserves twenty forms including nested loops. All
non-Boolean conditions remain accepted at parse time. LET's handcrafted tokens
retain the unusual accepted VARIABLE token whose text is '=': source validation
checks that text in this branch. Missing/malformed array positions, missing
expressions and unknown command tokens retain their precise rejection category.

The mixed array example containing SCRIPT(...) remains exactly as written and
passes through cleanScript. Passing this construction does not establish that
SCRIPT is a registered runtime function or that heterogeneous array positions
will execute successfully.

## Supplemental execution and precedence

Eight fixed inputs run through the existing KissVMOracle and minima_kissvm probes.
They require independently expected results for left-associated Boolean operators,
modulo versus multiplication, addition versus multiplication, chained division,
shift operands, unary NOT versus equality, array assignment/retrieval and a WHILE
containing IF. Examples that distinguish the pinned precedence include:

- TRUE OR FALSE AND FALSE evaluates to FALSE because these Boolean operators
  share a left-associated level.
- 20 % 3 * 2 evaluates to 2 because multiplication binds more tightly than modulo.

All expression records include thirteen fields; statement-run records include
eleven. Complete Java/native records must match, including instructions, stack,
variables and traces. Each runtime must return the independently expected value
without errors, producing sixteen independent checks. These supplement every
source case rather than replacing construction coverage with a few executions.

## Validation and limits

Both builds pass fifteen original methods, 1,269 source assertions and 747 direct
parser calls. Native counters measure executed assertions/calls. Java totals come
from reviewed successful source paths and loop cardinalities; unchanged methods
run with real JUnit assertions after exact annotation/options/lifecycle checks.
Both builds also pass all supplemental comparisons and the actual MinimaCoreCpp
node build. Normal CTest took 2.88 seconds; sanitizer CTest took 3.22 seconds.
Final native builds are warning-free for these targets. The full 281-test suite
was not repeated because production parser implementations did not change.

```sh
cmake --build cpp/build --target ExpressionParserTests StatementParserTests minima_kissvm MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_parser_tests$' --output-on-failure
```

Use cpp/build-sanitized for the configured sanitizer build.
[Evidence and remaining files](../../validation/original-parser-tests-work-in-progress/progress.json)
record the exact scope. Complete parser API, arbitrary malformed input/resource
limits, alias behavior and full node parity remain unverified.

Exercise: draw the parse tree for 20 % 3 * 2. Then explain why accepting IF 1 THEN
RETURN TRUE ENDIF is consistent with execution requiring a Boolean condition.
