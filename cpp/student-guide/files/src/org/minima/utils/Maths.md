# Maths — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/Maths.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/Maths.cpp) · [C++ header](../../../../../../src/org/minima/utils/Maths.hpp)

## Purpose

These helpers compare dotted versions and calculate proof-of-work cascade levels. The logarithm code deliberately reduces a large integer to a 53-bit mantissa. Tiny rounding changes can cross an integer boundary, so the native port must reproduce more than an approximate displayed answer.

## Follow the algorithm

1. compareVersions splits each string at dots, parses corresponding components as Java integers, supplies zero for missing components, and returns at the first unequal pair. Java splitting discards trailing empty components; interior empty components still fail integer parsing.
2. log2 computes log10(value)/log10(2). Its zero, negative, infinity, and NaN behavior comes from the underlying floating-point logarithm.
3. log2BI obtains Java BigInteger.bitLength, copies at most 53 high bits into a mantissa, and rounds upward when the next bit is set. It divides the mantissa by 2^52, then adds the integer exponent to log(fraction) multiplied by a fixed conversion constant.
4. getSuperLevel first divides difficulty by actual hash value using integer division. It computes log2BI, converts the result to an integer using Java conversion rules, clamps negative levels to zero, and then clamps to MINIMA_CASCADE_LEVELS−1. Actual zero therefore fails at division before logarithm processing.

## How the C++ translation works

The native implementation uses the existing BigInt byte representation to reproduce signed bitLength/testBit behavior. It calls the reused fdlibm routines rather than arbitrary platform logarithms and forces multiplication and addition to remain separate. It handles Java floating-to-int saturation and NaN conversion explicitly. Version components use javaParseInt, preserving Unicode digit handling and overflow errors. Constants are BigInt and MiniNumber counterparts.

## Understanding the tests

The small-utilities driver compares logarithm results as unsigned 64-bit encodings of IEEE 754 doubles, not rounded decimal strings; NaNs are canonicalized like Double.doubleToLongBits. It includes powers of two plus/minus nearby offsets through 2048-bit values and random bit patterns for doubles. Version inputs include trailing/interior dots, signs, integer overflow, Unicode digits, and whitespace. Cascade cases vary zero/empty hashes and extreme level limits. The entire observed result is compared between freshly compiled Java and C++.

- [Java fixture](../../../../../../support/SmallUtilitiesOracle.java)
- [Native fixture](../../../../../../support/small_utilities.cpp)
- [Differential driver](../../../../../../tools/small_utilities_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Most non-number-format failures are collapsed to ERROR in the fixture. Matching finite samples does not prove every floating-point input or every BigInteger is equivalent. A useful hand-derived boundary is a quotient just below and exactly at a power of two, checking whether the level changes where expected.

## Check your understanding

Why should 1.2 and 1.2.0 compare equally, while 1..2 can fail? Why is raw floating-point bit comparison stronger than checking six printed decimal places?

Pinned Java SHA-256: `435e4c4154bf7b92826c794f07f77bf8c36b897c82a78cf8b04921a139b6bfb6`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
