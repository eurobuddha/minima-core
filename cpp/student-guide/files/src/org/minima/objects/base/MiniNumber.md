# MiniNumber — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/base/MiniNumber.java](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java) | [src/org/minima/objects/base/MiniNumber.cpp](../../../../../../../src/org/minima/objects/base/MiniNumber.cpp) |

## What this file does

A decimal number with bounded constructor range, controlled precision and a scale-sensitive wire representation. It is used for amounts, counters and KISS arithmetic.

## Follow the logic

1. Think of the value as unscaled_integer × 10^(-scale). Java BigDecimal supplies that pair and rounds to 64 significant digits with RoundingMode.DOWN, meaning toward zero rather than toward negative infinity.
2. Constructors call checkLimits: first truncate scales above 44 decimal places, then reject values outside ±(2^64 - 1). isValidMinimaValue adds a different rule: greater than zero and at most one billion.
3. Arithmetic uses the same math context and builds a new MiniNumber. Addition/subtraction align decimal scales; multiplication combines them; division must keep sufficient significant digits before truncation. Remainder follows Java remainder semantics, not always a nonnegative mathematical modulo.
4. floor and ceil deliberately use FLOOR and CEILING, unlike general DOWN rounding. setSignificantDigits rejects values below 0 or above 64; zero is accepted because MathContext(0) means unlimited precision before the result returns through normal construction.
5. Numeric comparisons ignore scale. toString removes trailing zeros and prints plain decimal notation, while decimalPlaces returns the stored scale. Thus 1 and 1.00 can print identically but retain distinct binary encodings.
6. Wire output stores signed one-byte scale, one-byte byte-array length, then the signed two’s-complement BigInteger bytes. Positive 128 needs a leading 00 sign byte; negative values use signed encoding.
7. The reader accepts signed scale, rejects lengths below 0 or above 32, reads the BigInteger and applies the 64-digit math context. It does not call checkLimits. A value arriving from bytes can therefore violate constructor range or decimal-place restrictions until another operation reconstructs it.
8. Integer conversions truncate fractions and use Java narrowing semantics for overflow. They are not checked range conversions; this matters when a decimal controls an index or loop.

## How the translation is approached

Native MiniNumber explicitly stores a BigInt and scale. Its BigInt implements signed byte normalization, magnitude addition/subtraction, schoolbook multiplication and long division whose next base-256 quotient digit is found by binary search. Decimal addition aligns scales, exponentiation uses repeated squaring with guard digits, and sqrt uses an integer Newton iteration on a scaled radicand. Static deserialization intentionally bypasses constructor limits. This is substantially more than replacing BigDecimal with double.

## What the tests tell us

Selected constants include range endpoints, excessive precision, negative values and differently scaled zeros. The driver adds 100 seeded groups of add/subtract/multiply inputs, division including zero, floor/ceil, significant-digit zero, integer narrowing, square roots and positive/negative powers. Each numeric result compares both text and wire bytes. Although the harness supports mod, this driver does not generate a mod case, so its branch is not direct modulo coverage.

The [primitive differential driver](../../../../../../../tools/differential.py) compiles the pinned Java classes and runs [PrimitiveOracle](../../../../../../../support/PrimitiveOracle.java) against [minima_primitives](../../../../../../../support/primitives.cpp). Both report observable text and wire bytes; both flatten exceptions to `ERROR`. The inspected [existing report](../../../../../../../validation/checkpoint-362/normal/differential-results.json) records 3,802/3,802 comparisons with zero failures for the complete primitive corpus. This does not prove exact error classes, all methods, or full-node parity.

## Dependencies and reading map

Direct Minima imports: [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L100), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L104), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L109), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L114), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L119), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L124), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L129), [getNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L134), [checkLimits](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L141), [isValidMinimaValue](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L159), [getAsBigDecimal](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L166), [getAsBigInteger](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L170), [getAsLong](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L174), [getAsInt](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L178), [getAsDouble](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L182), [add](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L189), [sub](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L193), [div](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L197), [mult](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L201), [pow](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L205), [sqrt](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L209), [modulo](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L213), [floor](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L217), [ceil](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L221), [setSignificantDigits](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L225), [abs](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L237), [increment](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L241), [decrement](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L245), [decimalPlaces](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L249), [compareTo](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L254), [isEqual](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L258), [isLess](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L262), [isLessEqual](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L266), [isMore](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L270), [isMoreEqual](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L274), [toString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L279), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L287), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L302), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L321), [WriteToStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java#L327).

## Check your understanding

Show why floor(-1.2) is -2 but truncation toward zero is -1. Then encode the conceptual pairs for 1 and 1.00 and explain why equal comparison does not force equal wire bytes.

## Limits and traceability

Division now uses JavaArithmeticException: 0/0 reports Division undefined, while nonzero/0 reports Division by zero. Seven supplemental observations in NumberValueOriginalTests compare exact class/message text for 0, 1 and -1 over both 0 and 0.000, plus zero to power -1; both inspected normal and sanitized reports pass alongside 99 original NumberValue assertions. Negative square root and invalid significant digits still use generic standard exceptions in the inspected bodies. Generic primitive ERROR comparisons cannot establish their Java error parity. The native header now exposes MAXIMUM, both decimal limit constants, a decimal view and a decimal-carrier constructor; exhaustive API parity remains open. The source-map status remains partial and parity not_verified.

Pinned Java SHA-256: `62a458d5269f9adf6bcd0aaa051f57f6cd742afaebc46842854b4310c6b22bc0`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## Syntax failures exposed by transaction input tests

The [transaction-input lab](../../../../../../new-tests/transaction-inputs.md) found that the older native parser collapsed invalid decimal syntax into one generic error. The shared [decimal syntax helper](../../../../../../../src/support/JavaBigDecimalSyntax.hpp) now preserves the installed OpenJDK parser's character, repeated-decimal-point, missing-digit and malformed-exponent diagnostics. It retains the existing native arithmetic and rounding path. This scoped repair does not establish all exponent-overflow or precision-limit behavior.

## Exponent bounds and the smallest unit

The [custom-address sending lab](../../../../../../new-tests/custom-address-send-commands.md) exposed an incorrect generic exception for exponents outside -999999999 through 999999999. MiniNumber.pow now uses the existing JavaArithmeticException carrier with the exact message Invalid operation. Four added direct probes cover both signs and signed-int extremes; both builds pass all eleven supplemental error observations alongside the 99 original NumberValue assertions. The native MINI_UNIT constant now matches the source expression 10^-44. Token-creation tests compare the resulting stored coin and token bytes, including truncation to zero for negative decimal counts. Full API parity remains open.

## Balance scaling and constructor error text

The [balance lab](../../../../../../new-tests/wallet-balance.md) exposed a range-error
formatting mismatch in arithmetic results. Java converts a BigDecimal constructor
argument through toPlainString before applying its precision and range checks.
The native coefficient/scale constructor previously checked too soon, reporting
scientific notation where Java reported a plain integer. It now appends only the
zeros retained by the existing 64-digit precision before checking range. This
also avoids expanding a huge negative scale into an exponent-sized allocation.
The String constructor retains its distinct representation. Twenty-four direct
probes compare the two routes, signs, fractional trailing zeros and precision
boundaries. The original balance failure is preserved as a baseline report.

## Scalar Streamable integration

The native class now implements Streamable. Its append writer uses the existing
coefficient/scale encoding; its in-place reader calls the existing complete decoder
before replacing the stored pair. An invalid or truncated encoding therefore
leaves the previous number intact. Source wire reads apply the 64-digit math
context without constructor range checks. The new fixtures include extreme signed
scales, 32-byte coefficients and empty/invalid lengths through base references.

See the [scalar interface lab](../../../../../../new-tests/streamable-scalars.md) for the test logic. Both expanded interface suites pass 2,095 comparisons and 12,220 assertions across 14 types with matching transcripts and empty native stderr. All nine affected regressions pass in both builds. Full class parity remains unverified.

The scalar suite exposed redundant sign bytes in BigInt byte construction. Both
byte factories now call the existing normalise method, so number serialization
uses the same minimal signed coefficient as Java. The same fix applies to
MMREntryNumber; explicit sign-padding tests cover both callers.


## TxHeader reference and stream update

The [TxHeader lab](../../../../../../new-tests/txheader.md) explains the shared default objects, retained field and
array identities, run-length partial writes, and interrupted child factories.
Its evidence is scoped to these behaviors; full class and node parity remain open.

## Original base-test checkpoint

The original tests add the BigInt constructor overload and a decimal view that preserves the existing coefficient and scale without another rounding pass. The native view reuses MMREntryNumber, already used as an unbounded decimal carrier. BigDecimal equality in the original test checks coefficient AND scale; equal numeric values alone are insufficient.

The shared BigInt compareTo previously returned memcmp's byte difference for equal-length magnitudes. The new API corpus caught 2 compared with 0 returning 2 instead of Java's 1. Normalizing only the public comparison result fixes MiniData, MiniNumber and MMREntryNumber results while preserving ordering predicates. Both builds pass the 1,108-case API corpus, 3,802-case primitive regression and all original base assertions. This is a bounded result, not full MiniNumber/JDK parity.

See the [base-test lab](../../../../../../new-tests/original-base-tests.md) for the retained initial failure and complete scope.

## Decimal constructor and numeric limits

The original OperatorExpression tests now exercise both signed Minima limit constants and their decimal-carrier construction. MAXIMUM and the immutable MAX_MININUMBER/MIN_MININUMBER decimal values expose ±(2^64−1). The new constructor delegates to the existing coefficient/scale path, so rounding and range checks remain shared. Fifty-six new cases supplement the complete earlier base corpus: both builds pass 1,164 comparisons / 19,160 independent checks, all original base assertions and 3,802 primitive comparisons. Full class parity remains unverified.

See the [expression/token lab](../../../../../../new-tests/original-expression-token-tests.md) and [checkpoint evidence](../../../../../../../validation/original-expression-token-tests-work-in-progress/progress.json).
