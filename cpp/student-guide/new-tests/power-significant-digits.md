# Powers and significant digits: reading what a source test proves

A faithful test port preserves both the positive examples and the rejection checks. These two files call the actual function objects through ConstantExpression and Contract, rather than substituting arithmetic inside a test fixture.

Read [POWTests.java](../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/POWTests.java) and [SIGDIGTests.java](../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.java).

## POW: exponent comes first

POW takes exponent, then base. The production function requires exactly two numeric arguments, rejects fractional exponents and rejects absolute exponents of 1024 or more. It delegates accepted arithmetic to MiniNumber.pow. The original positive test is only POW(0, 0), whose result is 1. That single example cannot demonstrate general exponentiation accuracy or the 1024 boundary.

The mapped test preserves four constructor/registry assertions, two result assertions, three invalid argument counts, one fractional-exponent rejection, and all fifteen pairs of argument types except Number/Number. Total: 25 active assertions per runtime. It catches ExecutionException specifically.

## SIGDIG: precision comes first

SIGDIG takes precision, then value. It rejects fractional and negative precision before calling MiniNumber.setSignificantDigits. Precision zero means unlimited precision in the underlying decimal context. It does not mean round the value to zero.

The source loop checks precisions 0 through 32 on one decimal value. For each precision it checks the result type and compares the function result with a direct call to the same MiniNumber method. This verifies delegation and numeric equality; it is not an independent oracle for the rounding algorithm. Other Java/C++ differential suites provide separate integration evidence.

The mapped test preserves four constructor/registry assertions, 66 loop assertions, two invalid argument counts, two precision-domain rejections, and fifteen invalid type pairs. Total: 89 active assertions per runtime. The source comment saying the negative case does not throw is stale: execute the assertion and inspect the current function body before accepting that comment.

## Reuse and validation

Reuse the existing INCTests native assertion/context pattern, the existing script_value_original_tests.py runner and unchanged pinned Java test methods. No production arithmetic change is planned. The sibling SIGDIG implementation uses reversed arguments and double rounding, so it is incompatible with these tests.

Both full libraries and the two new test targets compile. All four selected CTest entries pass in each build: normal 5.60 seconds, AddressSanitizer/UndefinedBehaviorSanitizer 6.11 seconds. The original Java methods and native tests execute 25 POW assertions and 89 SIGDIG assertions per runtime. These small source tests do not establish complete API parity.
