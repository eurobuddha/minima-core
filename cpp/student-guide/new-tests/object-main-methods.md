# Lab: entry points with random input

This lab follows three source demonstrations: `Address.main`, `MiniData.main`, and
`TxPoWGenerator.main`. The production classes already existed in C++; their Java
entry points were missing. Adding these methods is API progress, not proof that
all behavior in any of the three classes has been ported.

## Logic: read the whole expression before translating it

Address generates 16 random bytes and encodes their unsigned integer value in
Minima Base32. It takes characters at `[2,6)`, `[7,11)`, `[12,16)`, `[17,21)`, and
`[22,26)`, inserting hyphens between those five groups. The end index is excluded.
Notice that the source skips characters between groups. Copying consecutive groups
would look reasonable but produce a different password.

Base32 integer conversion also removes leading zeroes. An unusually small number
can make the text too short for one of the five substrings. Java then throws before
printing any of the three output lines. C++ `substr` silently shortens some requests,
so the native body checks the end index and throws the matching Java exception.
This preserves the demonstration as written; it is not a password-format redesign.

MiniData generates 10 bytes and prints their byte length, hexadecimal spelling, and
unsigned integer value. It then pads on the left to 16 bytes and prints again.
The extra six zero bytes change length and spelling, while preserving the integer.
The existing `withMinLength` and `getDataValue` implementations do that work.

TxPoWGenerator creates the list zero, one, two and sorts with the reversed comparator
`second.compareTo(first)`. Native `stable_sort` asks a boolean question, so its
predicate is `second.compareTo(first) < 0`. The actual resulting list prints as
`[2, 1, 0]`; the demonstration does not generate a block or access a chain database.

## Test: control inputs without replacing the tested calculation

The [Java fixture](../../support/ObjectMainsOracle.java) installs a temporary security
provider in its own process for fixed-input cases. The unchanged source main calls
`new SecureRandom()` normally. The provider supplies the chosen bytes and verifies
that the request has the original size and happens exactly once.

The [native fixture](../../support/object_mains.cpp) has friend access to a private
helper holding the production code after the random draw. The ordinary native main
calls that same helper after obtaining bytes from the existing OS random source.
The fixed native comparison therefore tests the production calculation and output,
but bypasses its random draw. This distinction is part of the test's limits.

The [driver](../../tools/object_mains_differential.py) compares complete fixed-input
stdout with Java, including exceptions and the absence of earlier output on failure.
It also checks expected results independently using Python integers and a Base32
alphabet. Values cover every first-failing substring, the final success boundary,
zeroes, leading zeroes, high-bit values, and all ones.

For ordinary runs, both real main methods use their normal random sources. The
resulting bytes should not be equal across languages. Instead, the test parses the
printed bytes and verifies every other field against those bytes. It runs null,
empty, and nonempty argument arrays because these source methods ignore arguments.
The sorting main is compared exactly under all three argument forms.

Finally, `getRandomData` is tested with negative, zero, and positive sizes. Java's
negative-size allocation throws `NegativeArraySizeException`, including the number
in its message. The C++ method previously threw `IllegalArgumentException`; it now
uses the existing native carrier for the correct Java exception.

## Reproduce and interpret

Build `object_mains`, then run:

```sh
python3 cpp/tools/object_mains_differential.py --build cpp/build
python3 cpp/tools/object_mains_differential.py --build cpp/build-sanitized
```

Each build writes `object-mains-results.json`, including every raw Java/native
observation. The checkpoint also retains these reports under
[validation](../../validation/object-mains-work-in-progress/).
A pass proves the listed comparisons. It does not certify random-number distribution,
all stdout failures, object aliasing, subclass dispatch, or whole-file parity.

Exercise: explain why left-padding changes byte equality but preserves numerical
comparison. Then find the smallest positive integer whose Base32 text is long enough
for the last password group, remembering the two-character `Mx` prefix.

## Checkpoint result

Normal and ASAN/UBSAN node builds passed. In each build, this lab passed 31 exact
comparisons and 12 actual random main runs, with 85 driver assertions. The existing
original base/object regressions also passed: 20 source test methods and 94
assertions per runtime. Those regression counts are separate from this lab.
