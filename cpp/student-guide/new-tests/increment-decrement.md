# Increment and decrement — preserve the small difference

INC adds one to a number; DEC subtracts one. Their implementations already exist. This step maps the original Java test classes to individual C++ test files, using the same active inputs, expected strings and exception types.

Read [INCTests.java](../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/INCTests.java), [DECTests.java](../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/DECTests.java), [INCTests.cpp](../../test/org/minima/tests/kissvm/functions/maths/INCTests.cpp) and [DECTests.cpp](../../test/org/minima/tests/kissvm/functions/maths/DECTests.cpp).

## What the examples teach

INC(-0.99999999999999999) is 0.00000000000000001. Replacing a decimal implementation with ordinary binary floating point can erase that distinction. DEC(0.49999999999999999) similarly produces -0.50000000000000001. A test checks the returned value type as well as its exact decimal string, because a string containing the right digits is not a NumberValue.

Each source class has three annotated methods. Constructor tests check the function name and zero initial parameters through both its factory and the global registry: four assertions. Eleven value examples check type and text: twenty-two assertions. Four rejection checks require ExecutionException for a missing argument, BooleanValue, HexValue and StringValue. Total: thirty active assertions per runtime, per class.

The source’s empty invalid-domain block asserts nothing. Its comments about alternate arithmetic results are not executable assertions. Neither class checks extra arguments or every numeric limit. We preserve that scope rather than count comments as tests.

## How the port reuses tested pieces

The native files reuse ABSTests.cpp’s actual Contract, ConstantExpression, function factory, registry and typed-exception test pattern. Only the function class, source value table and source rejection cases change. A variant retains integer construction for -1, 0 and 1 and string construction for the fractional inputs, matching the source overloads. The [comparison runner](../../tools/script_value_original_tests.py) compiles the unchanged pinned Java tests against JUnit and calls their three methods, then runs the mapped native executable. No substitute implementation of arithmetic is introduced.

Both normal and AddressSanitizer/UndefinedBehaviorSanitizer builds passed the two native tests and two Java/native comparison tests: four CTest entries per build. All four comparison reports record three original methods and thirty successful-path assertions per runtime, per class. These finite examples establish the original assertions, not complete INC/DEC or MiniNumber parity.

## Exercises

1. Why construct decimal inputs from strings? A conversion through a binary floating-point value may already have lost digits before INC runs.
2. How many successful-path assertions does one value example execute? Two: type and exact text.
3. Would catching any exception be enough for the rejection cases? No. These source tests specifically require ExecutionException.
