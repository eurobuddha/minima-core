# Original expression and token tests: preserve types, values and spelling

This checkpoint maps BooleanExpressionTests, OperatorExpressionTests and
kissvm/tokens/TokenTests. The last file is distinct from the already mapped
objects/TokenTests. Its executable is KissTokenTests to avoid a CMake name collision;
its package-relative C++ path and reported Java class remain TokenTests.

## Proven code reused

The pinned test files and their production BooleanExpression, OperatorExpression,
ScriptToken and ScriptTokenizer classes were read before translation. Native
expression classes, Value type checks, MiniNumber arithmetic, the immutable
MMREntryNumber decimal carrier, token classes and MinimaFunction registry are
reused. Existing direct JUnit runners, OriginalFunctionTestSupport and KISS/base
probes provide validation. No expression or tokenizer algorithm was changed.

The inspected sibling `tools/minima-core-cpp/src/kissvm/Interpreter.cpp` uses
short-circuit Boolean evaluation and different type/operation behavior. Its
Tokenizer.cpp changes hex spelling, handles nested strings differently and uses
a different identifier/operator vocabulary. These cannot replace the pinned APIs.

## BooleanExpressionTests: evaluate both operands and preserve types

The source tests all Boolean truth-table combinations for AND, NAND, OR, NOR,
XOR and NXOR, then both unary NOT results. Even unary construction retains the
source's default FALSE right expression. Ordinary Boolean evaluation eagerly
gets both operand values; C++ short-circuit operators inside the final Boolean
calculation do not skip expression evaluation.

The tests require IllegalArgumentException for inappropriate Boolean or ordering
operand types, represented by the established native std::invalid_argument.
Unknown operation IDs require the distinct ExecutionException. Equality and
inequality operate on matching Boolean, hex, script or number values; ordering
requires numbers. Fourteen display assertions include unknown-operation ERROR
spelling. Both methods and all 116 original assertions remain, including shared
Contract instances within source fixture blocks and their accumulated instructions.

## OperatorExpressionTests: arithmetic is not machine-integer arithmetic

The complete constructor/execution method has 166 assertions, followed by twelve
display assertions. All 178 remain. The fixtures include signed int/long extrema,
the Minima numeric limits, fractional numbers, empty strings and differently sized
hex values. Even unused fixture objects and the source ce_5 initialized to +5
are preserved. Typed casts remain actual native dynamic_cast checks.

Results beyond signed 64-bit limits are compared as BigInt decimal strings.
Java Long.valueOf arithmetic is explicitly widened in C++ to avoid signed-int
overflow. Modulo retains truncation-toward-zero remainder semantics, including
negative fractional results. Negative shift distances reverse direction; they
are active source expectations, despite comments questioning that behavior.
Bitwise operations align the least-significant ends and strip leading zero bytes,
while preserving a single zero byte for an all-zero result. NOT complements only
the bytes present in the input before trimming.

The source's inactive extreme modulo/range examples stay inactive. Wrong-type
rejections preserve the difference between IllegalArgumentException and
ExecutionException; divide-by-zero and unknown operations require the latter.
Display strings retain the source's spacing and unary forms.

Eight supplemental fixed KISS expressions independently check results beyond
signed-long limits, fractional remainder, reversed shifts, hex NOT, Boolean NOT
and NXOR. Both runtimes must match all thirteen result/context/trace fields and
return the known type/value without an exception: sixteen independent checks.

## MiniNumber API additions exposed by these tests

The original fixtures use MAX_MININUMBER and MIN_MININUMBER as BigDecimal values.
Native MiniNumber now exposes those limits through the existing immutable decimal
carrier, plus MAXIMUM as a MiniNumber. The values are ±(2⁶⁴−1), not signed-long
limits. A decimal-carrier constructor delegates to the existing coefficient/scale
constructor, preserving plain-string conversion, 64 significant digits rounded
toward zero, the 44-decimal-place limit and range checks. This adds missing API
surface without replacing the tested arithmetic implementation.

The existing base-value differential corpus retains all 1,108 previous cases.
Fifty-six additional cases inspect all three constants and exercise decimal
construction over signs, zero, positive/negative scales, trailing zeros, numeric
limits, excessive precision and out-of-range values. Python Decimal provides
independent expected coefficient, scale and wire observations; Java/native
records and error diagnostics must also match. Both builds pass 1,164 comparisons
and 19,160 independent field assertions. This is bounded API evidence, not full
BigDecimal or MiniNumber API parity.

## TokenTests: classify and preserve the original text

The constructor loops cover all thirteen commands, fifty-three functions, ten
number operators and fourteen Boolean operators. Their runtime registry/list
loops remain intact. Ten explicit values/types add thirty checks. Constructor
coverage totals 300 assertions. An unknown type 99 retains three assertions,
including the literal type name null. Seventy-four checker assertions preserve
the source's lowercase-variable and numeric syntax expectations.

Tokenization contributes 322 assertions over commands, functions, operators,
nested bracket strings, hex values, parentheses, Booleans, globals and variable
names. All five random hex sizes remain 4, 8, 16, 32 and 64 bytes, generated
independently in each runtime. Tokenization preserves the source spelling rather
than normalizing it to a MiniData display. Constructor labels such as :123456 do
not imply that text is accepted by the tokenizer.

The checker accepts a 26-character lowercase variable in the active source,
despite an outdated comment saying sixteen characters. Tokenization enforces the
production limit of thirty-two. Four supplemental fixed cases check nested
strings, exact mixed-case/zero-padded hex spelling, brackets/Booleans and a
32-character variable, comparing token type, source spelling and display metadata.
All four original methods and 699 assertions pass, plus four comparisons/eight
independent supplemental checks. Registry mutation and every malformed token
remain outside this checkpoint's certification.

## Validation and student exercises

| Original file | Methods | Assertions per runtime |
| --- | ---: | ---: |
| BooleanExpressionTests | 2 | 116 |
| OperatorExpressionTests | 2 | 178 |
| tokens/TokenTests | 4 | 699 |

All eight methods / 993 original assertions pass in normal and sanitizer builds.
Java runs unchanged methods with real JUnit assertions after checking exact
annotations, options and absence of lifecycle methods. Native assertions are
counted at runtime; Java totals come from reviewed successful source paths and
full loop cardinalities. Matching generic errors cannot satisfy supplemental cases.
The original base suite (12 methods / 74 assertions) and all 3,802 primitive
comparisons also pass in both builds. Both actual node targets build. The full
280-test suite is not repeated for these bounded API additions.

```sh
cmake --build cpp/build --target BooleanExpressionTests OperatorExpressionTests KissTokenTests minima_kissvm minima_kiss_values base_value_api MiniNumberTests MiniDataTests minima_primitives MinimaCoreCpp
ctest --test-dir cpp/build -R '^(original_expressions_tests|original_token_tests|original_base_tests|base_value_api_differential|java_differential)$' --output-on-failure
```

Use cpp/build-sanitized for the configured sanitizer build.
[Evidence and remaining files](../../validation/original-expression-token-tests-work-in-progress/progress.json)
record scoped outcomes. Full working-node parity remains incomplete.

Exercise: explain why −3.14 remainder 0.5 is −0.14, why NOT 0x00FFFF produces
0xFF0000, and why a token's text must retain 0xab even though a MiniData value
would display uppercase hex. Then design a test that detects accidental
short-circuit expression evaluation rather than merely a wrong truth table.
