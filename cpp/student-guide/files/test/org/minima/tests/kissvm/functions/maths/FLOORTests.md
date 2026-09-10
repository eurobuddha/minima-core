# FLOORTests — understanding directed decimal rounding

**Teaching status:** Authored source, translation and executed-test explanation.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ test exists. These labels do not mean complete production API equivalence.

Source: [test/org/minima/tests/kissvm/functions/maths/FLOORTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/FLOORTests.java). Translation: [FLOORTests.cpp](../../../../../../../../../test/org/minima/tests/kissvm/functions/maths/FLOORTests.cpp). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

## Purpose and mathematical logic

FLOOR returns the greatest integer less than or equal to its input. For example, FLOOR(-0.49999) is -1. This direction matters: truncating a negative fraction toward zero would give zero, whereas flooring it gives minus one. Ceiling and floor both leave an integer unchanged.

The production [FLOOR lesson](../../../../../../../src/org/minima/kissvm/functions/number/FLOOR.md) checks the parameter count, asks Contract to evaluate a NumberValue argument, calls MiniNumber's decimal operation, and returns a NumberValue. The port reuses this native implementation unchanged; it does not introduce floating-point rounding into the test.

## Walk through the three original methods

| Method | Experiment | Assertions executed on a successful run |
| --- | --- | --- |
| testConstructors | Get a fresh function directly and through MinimaFunction's registry; both must have the correct name and zero stored parameters. | 4 |
| testValidParams | Evaluate 11 source inputs; each result must have VALUE_NUMBER type and the expected decimal text. | 22 |
| testInvalidParams | Supply no parameter, then Boolean, Hex and String parameters; each call must throw ExecutionException. | 4 |
| Total | All three unchanged source methods | 30 |

The valid inputs retain the source order. Whole numbers −1, 0 and 1 use numeric constructors. Fractions use exact decimal strings so the test never first rounds them through a binary floating-point number. A fresh function is used per case, while the valid cases share one Contract as in Java. A second Contract belongs to the invalid-parameter method.

The near-boundary values include 0.99999999999999999 and 0.49999999999999999. They are useful because a careless conversion to a machine float could change a value before the function sees it. The returned text is checked as well as the type: returning a numerically similar StringValue must not pass.

## How the C++ test preserves the Java experiment

The native file reuses the ABSTests require/typed-rejection pattern, actual ConstantExpression objects and fresh native function factories. A counted require helper fails if a condition is false. The rejection helper catches only ExecutionException; an unrelated runtime error escapes and fails the executable. A final counter guard requires exactly 30 active assertions.

The original source also contains fail() calls inside catches for unexpected errors. Those are failure paths, not extra assertions executed on a successful run. Counting text occurrences would overstate the test's successful observations.

[FLOOROriginalTests.java](../../../../../../../../../support/FLOOROriginalTests.java) calls the original three Java test methods without rewriting them. The [shared original-test runner](../../../../../../../../../tools/script_value_original_tests.py) compiles them against the pinned source and bundled JUnit dependency, then runs the native test. This tests the original expectations directly rather than manufacturing expected results from C++ output.

## What passed and what remains untested

All **30 assertions per runtime** pass in normal and ASAN/UBSAN builds. All ten selected suites pass in each build, including the existing original value/function tests used by the shared runner. The complete selections took 13.21 seconds normal and 13.07 seconds with sanitizers. No production arithmetic change was required.

The source's invalid-domain block is empty. Its invalid-count method tests zero arguments but does not test two arguments, although the production function requires exactly one. These are coverage gaps, not implied passes. The original tests also do not inspect serialized numeric bytes, unusual scales, null receivers, side-effecting expressions, or all decimal limits. The existing VM differential suite has additional cases, but it does not make this finite file exhaustive.

After building successfully, run the native and original-source pair from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^(FLOORTests|floor_original_tests)$'
```

The report is cpp/build/floor-original-results.json; the sanitizer build has its own report.

## Check your understanding

1. Why construct decimal fractions from text? **Answer:** the input must retain the exact decimal digits specified by the original fixture.
2. Why test both value type and displayed text? **Answer:** identical text can come from the wrong VM type, which changes later operations.
3. Does the empty invalid-domain block test anything? **Answer:** no; it is a source placeholder, not evidence.
4. What extra test would check argument count more thoroughly? **Answer:** pass two valid numeric expressions and require the source-defined ExecutionException; label it as an additional case rather than an original assertion.

## Traceability

Pinned Java SHA-256: `2267c7b790bddf0c06b09a36bf103bf94e7d5eafb362931c67c84869c4d52aa5`. [Source map](../../../../../../../../../source-map.json). [Review and limitations](../../../../../../../../../REVIEW.md).
