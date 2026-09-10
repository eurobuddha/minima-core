# NumberValueTests — The VM wrapper around exact decimal numbers

Java source: [test/org/minima/tests/kissvm/values/NumberValueTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/NumberValueTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Construct values from integer, long, and larger numeric inputs. Check arithmetic, comparisons, and string formatting, including numeric boundaries. A textual comparison can detect a precision loss that a rounded display would hide.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/NumberValueTests.java#L17) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 7 |
| [`testArithmentic`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/NumberValueTests.java#L48) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 38 |
| [`testComaprisons`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/NumberValueTests.java#L227) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 24 |
| [`testToString`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/NumberValueTests.java#L374) | Compare the displayed representation with explicit expected strings; display equality is distinct from execution correctness. | 6 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 33](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/NumberValueTests.java#L33):

```java
assertEquals("should be equal ", nv1.getNumber().getAsInt(), nv1a.getNumber().getAsInt());
```

[Source line 34](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/NumberValueTests.java#L34):

```java
assertEquals("should be equal ", nv2.getNumber().getAsLong(), nv2a.getNumber().getAsLong());
```

[Source line 162](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/NumberValueTests.java#L162):

```java
assertThrows(ArithmeticException.class, () -> { NumberValue nv1l = new NumberValue(0); NumberValue nv2l = new NumberValue(0); NumberValue resl = new NumberValue(0); nv1l.div(nv2l); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. The mapped native test retains these source expectations. Its two exception assertions require the dedicated native JavaArithmeticException type; a different exception does not satisfy them.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.values.NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)

## Teacher's warning: written branches are not always executed

The comparison method increments a counter and uses a switch to select a
construction route. Its case 3 is commented out. When the counter reaches 3,
execution therefore enters `default`, resets the counter to zero, and leaves the
loop. The active-looking cases 4, 5 and 6 below that point are never reached in
these loops. Each comparison loop checks only routes 1 and 2 in the original
source. A translator must not replace this with a loop over every written case
and then claim those additional cases came from the pinned test.

This illustrates three different counts: assertion call sites in the source,
assertions actually executed by loops, and possible branches an engineer might
wish to test. They are not interchangeable. More tests could be added deliberately,
but the 1:1 mapped test should retain the source's behavior and explain the gap.

The arithmetic method also distinguishes machine integers from Minima numbers.
Adding one to a NumberValue made from `Long.MAX_VALUE` constructs its expected
result through MiniNumber arithmetic, not by overflowing a native signed long.
Multiplication includes results larger than a Java int; subtraction checks the
opposite operand order where appropriate. Commented division/formatting cases
remain inactive, even when their comments describe interesting precision issues.

## The correction prompted by the next test

The source delegates division through NumberValue to MiniNumber and then Java's BigDecimal. The previous native `divideDecimal` helper threw a generic runtime error for every zero denominator. The new mapped test requires the native JavaArithmeticException type, which reuses the project's existing typed exception and stack-capture machinery.

The helper now distinguishes the dividend as well: `0/0` throws `ArithmeticException: Division undefined`, while a nonzero value divided by zero throws `ArithmeticException: Division by zero`. These messages come from the installed JDK's `BigDecimal.divide(BigDecimal, MathContext)` body, which was inspected alongside the pinned MiniNumber caller. The positive-precision context used by MiniNumber takes that path. Successful division arithmetic is unchanged.

## C++ translation and validation

The [mapped C++ test](../../../../../../../../test/org/minima/tests/kissvm/values/NumberValueTests.cpp) uses existing NumberValue and MiniNumber operations. It preserves **99 executed source assertions**: 7 constructor assertions, 38 arithmetic assertions, 48 comparison assertions and 6 formatting assertions. An explicit native assertion counter guards that count. The comparison method contains 24 assertion call sites, but each group runs twice before the missing case 3 terminates its loop.

The C++ int64_t constructor covers both source int and long routes. Expected integer calculations that exceed 32 bits are performed with 64-bit values; the expected result above Long.MAX_VALUE is constructed with MiniNumber arithmetic, just as in Java. This avoids introducing signed C++ overflow while translating a test of exact decimal arithmetic.

[NumberValueOriginalTests.java](../../../../../../../../support/NumberValueOriginalTests.java) invokes all four unchanged pinned JUnit methods, including their original misspelled names. The [shared original-test driver](../../../../../../../../tools/script_value_original_tests.py) compiles those Java sources with the repository's bundled JUnit dependency, runs the native test, and records the result separately from ScriptValue and HexValue.

Seven **additional** observations follow the mapped assertions. Java and C++ each print the exception class and message for dividends 0, 1 and -1 divided by denominators `0` and `0.000`, plus `MiniNumber(0).pow(-1)`, whose reciprocal reaches the same helper. The driver compares the full output byte for byte. These checks are supplemental regression evidence, not additional assertions present in the original NumberValueTests file.

Full normal and ASAN/UBSAN builds passed. All 16 selected regression suites passed in 193.70 seconds and 528.87 seconds respectively. Both original-test reports confirm four unchanged Java methods, 99 mapped assertions per runtime and seven matching supplemental error observations. Primitive (3,802), value (1,451), VM (4,471), object (9,244), checker (1,343), logger (2,016) and consensus (2,890) comparison reports have zero failures in both builds. These are suite totals, not additional NumberValue-only assertions.

After a successful build, run the original/mapped comparison with:

```sh
ctest --test-dir cpp/build -R '^(NumberValueTests|number_value_original_tests)$' --output-on-failure
```

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `0f9417b49015215d881e5b9ebd6534eaa23e44490ca9a6fba0388516ed689fb0`.

## Additional exponent-boundary evidence

The custom-address sending batch adds four separately labelled probes to the seven existing arithmetic error observations. Exponents -1000000000, -2147483648, 1000000000 and 2147483647 must raise ArithmeticException with Invalid operation. Normal and ASAN/UBSAN comparisons pass all eleven exact error observations plus the unchanged 99 original assertions. These four additions are not new assertions in the pinned Java test.
