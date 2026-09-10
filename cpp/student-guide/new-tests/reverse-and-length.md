# Lab: reversing bytes and measuring length

This lab was written before the first comparison run for the original REV and
LEN test mappings. Both production functions already exist; we reuse their
actual registry factories, ConstantExpression, Value types and Contract.
The assertion and typed-exception adapter comes from the existing BITGET test.

## What is being reversed?

The source REV example turns `0x01234567` into `0x67452301`. Write the input as
four bytes: `01 23 45 67`. Reversing those bytes gives `67 45 23 01`. Reversing
individual hexadecimal characters would produce a different answer. The result
must also remain a HexValue. The implementation copies the byte array before
reversing it, although the original test does not assert input preservation.

The test labelled “invalid domain” actually supplies two empty hex arguments.
The function requires exactly one argument, so this checks arity; it does not
prove that one empty hex argument is invalid. Good teachers read the assertion
and trigger, rather than treating the comment as the specification.

## What does length count?

LEN returns four for the same four-byte hex value and five for the script string
`HEllo`. It rejects BooleanValue and NumberValue. The existing native function
counts UTF-16 units for strings, following Java String.length. The sibling
implementation counts std::string bytes; its ASCII/hex examples cannot prove
Unicode compatibility. We retain the current native implementation.

The original LEN test uses ASCII only. Supplementary characters and malformed
Unicode are covered separately by parts of the existing KISS differential suite;
these twelve assertions must not be described as exhaustive Unicode coverage.

## Count the assertions

| Mapping | Factory/registry | Valid type/value | Rejections | Total |
|---|---:|---:|---:|---:|
| REVTests | 4 | 2 | 6 | 12 |
| LENTests | 4 | 4 | 4 | 12 |

Each has three original methods. The Java wrapper executes their unchanged
bodies. The mapped native file repeats the same inputs and observations, using
ExecutionException-specific catches. An unrelated runtime exception is a failure.

## Evidence

All four registered tests pass in both builds. The two Java/native reports per
build confirm three unchanged methods and twelve assertions per runtime for each
file. No production function changed. Full API parity remains unverified.
