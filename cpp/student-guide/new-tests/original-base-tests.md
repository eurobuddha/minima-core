# Original base tests: assertions, values and representations

MiniNumberTests and MiniDataTests now have mapped C++ files with all twelve active
source methods. The inventory rises from 421 to 423 counterparts; 34 files remain
absent. All full-parity labels remain `not_verified`.

## Reuse first

The translation uses existing MiniData, MiniNumber, BigInt, byte-stream and
exception-formatting code. The source's decimal observations reuse the existing
unbounded coefficient/scale representation in MMREntryNumber, also used for
chain weights. MiniData's byte magnitude can exceed MiniNumber's monetary limit,
so converting it through MiniNumber would reject valid source values.

The new adapters expose comparison, inclusive comparison, decimal views, a
BigInt constructor and a bounded hex string. A decimal view copies the current
coefficient and scale without another rounding pass. The caller must use the
carrier's decimal formatting and scale, not infer representation from numeric
equality alone. This does not implement the entire JDK BigDecimal API or its
object identity model.

## Count what actually executes

| Original class/method | Executed source assertions | Observation |
| --- | ---: | --- |
| MiniNumberTests.testValueZero | 8 | Zero, equality, ordering and arithmetic. |
| MiniNumberTests.testValues | 28 | Constructors, conversions, arithmetic and decimal equality. |
| MiniNumberTests.testWriteDataStream | 1 | Original number remains non-null after writing. |
| MiniNumberTests.testReadDataStream | 2 | Original number is non-null and numerically one. |
| MiniNumberTests.testReadStreamTooLarge | 1 | The caught error contains the expected phrase. |
| MiniNumberTests.testReadFromStream | 1 | Static read result is discarded; original number is checked. |
| MiniDataTests.testValueBytes | 27 | Non-null values, byte equality and numeric ordering. |
| MiniDataTests.testReadFromStreamData | 3 | Two read observations, then a catch with unconditional true. |
| MiniDataTests.testReadFromStreamDataTooBig | 0 | Both reads succeed on the valid leading object. |
| MiniDataTests.testWriteHashStreamData | 1 | Original object remains non-null after writing. |
| MiniDataTests.testReadHashFromStream | 1 | Hash reader returns an object. |
| MiniDataTests.testReadHashStreamDataLarge | 1 | Oversized hash length is rejected. |

Totals are 41 MiniNumber assertions and 33 MiniData assertions per runtime.
The Java runner checks the exact `@Test` method set, default options and absence
of lifecycle methods before invoking each unchanged method on a fresh instance.
It uses the real JUnit assertions, following the repository's existing direct
runner. Native counters track executed assertion calls.

The ten-million-byte append loop is retained in both runtimes. Its test name
does not guarantee an exception: the reader first sees the valid object's length,
and resetting the input returns to that same valid object. The method reaches
its second print and executes no assertion. We record that weakness explicitly.

All original printed observations are compared in order. Two observations are
inherently different across processes: a byte array's identity suffix and a
separately generated random byte. The driver first validates their expected
forms, including exactly one random byte, then normalizes only those values.

## The defect missed by sign-only checks

The extra API corpus compared MiniData values 2 and 0. Java returned `1`; C++
returned `2`. The native BigInt comparison exposed `memcmp`'s byte difference.
That still made `result > 0` true, so the existing ordering assertions passed.
Java's comparison API requires exactly `-1`, `0` or `1`.

The fix normalizes BigInt's public comparison result. Its inspected consumers
use comparison signs or expose the result through MiniNumber/MMREntryNumber;
they do not need the byte difference. The magnitude arithmetic helpers retain
their existing sign checks. Initial reports preserve all 336 direct mismatches
per build and the independent expectations that also caught them.

## Additional checks and their limits

The 1,108-case API corpus compares exact results and independently checks 18,632
expected fields per build. Cases cover:

- Equal numbers with unequal leading-zero byte sequences.
- Empty data, 32-byte hashes and 128-byte magnitudes beyond monetary limits.
- Warmed numeric caches retained across in-place byte replacement.
- Retained decimal snapshots, coefficient/scale/text and zero at scale 127.
- Constructor range failures and wire-read values beyond constructor limits.
- Bounded hex output, which always appends two dots, and exact negative-bound errors.
- Positive, negative and zero comparison results through both decimal types.

Both builds pass this corpus with transcript SHA-256
`8add3a91186f26bf3f03a71c987f8420a134434a2d46c566e16fd27ce2717a14`,
all twelve original methods/74 assertions, and the existing 3,802-case primitive
regression. Both MinimaCoreCpp targets build the `MinimaCoreC++` executable.
The full CTest suite was not repeated; these results do not certify complete
base APIs, arbitrary allocation/concurrency behavior or a finished node.

[Checkpoint evidence](../../validation/original-base-tests-work-in-progress/progress.json)
contains results, initial failures, build logs and the remaining-file list.
Run `python3 cpp/tools/original_base_tests.py --build cpp/build` and
`python3 cpp/tools/base_value_api_differential.py --build cpp/build` from the
repository root. Substitute `cpp/build-sanitized` for the sanitizer build.

Exercise: explain why byte sequences `0x01` and `0x0001` compare numerically equal
but fail byte equality. Then predict the numeric and byte observations after
warming `0xFF`'s cache and reading `0x02` into the same object.
