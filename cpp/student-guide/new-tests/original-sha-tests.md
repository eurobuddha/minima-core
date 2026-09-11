# Original SHA tests: preserve what the test actually proves

This batch maps SHA2Tests, SHA3Tests and CHAINSHATests to native C++ files.
The source remains pinned to `5564b9d78978efcaf23679190d05253961fc6e69`.
A mapped test is evidence about its assertions, not certification of the full node.

## Reuse and translation

The native functions already exist under `src/org/minima/kissvm/functions/sha`.
The tests call those production classes directly. The fixture follows the existing
LENTests: a real Contract, fresh function instance, and ConstantExpression arguments.
A small shared test helper retains the exact ExecutionException check and counts
executed assertions. A different exception fails the test instead of passing it.

The Java runner adapts OriginalBaseTests. It checks the exact annotated method set,
default annotation options and absence of lifecycle methods, then invokes each
unchanged source method on a fresh instance using real JUnit assertions. It does
not need the missing Hamcrest dependency required by JUnitCore.

The sibling `tools/minima-core-cpp/src/kissvm/functions/Functions.cpp` is unsuitable
for this proof test: its PROOF accepts three arguments and uses different proof
objects, while the pinned production function requires five arguments. The current
native function is already aligned with the pinned five-argument entry point.

## Count executed assertions

| Class | Constructors | Valid method | Invalid method | Total |
| --- | ---: | ---: | ---: | ---: |
| SHA2Tests | 4 | 204 | 4 | 212 |
| SHA3Tests | 4 | 0 | 23 | 27 |
| CHAINSHATests | 4 | 0 | 20 | 24 |

All nine original methods run. Their successful paths execute 263 assertions in
each runtime. SHA2 retains all 100 random 64-byte trials, plus empty hex and empty
string cases. The expected random digest comes from Crypto, just as in Java.
Each runtime generates its own random inputs; those trials are not a shared
Java/C++ differential corpus. The fixed empty-string digest is an independent
known answer. The supplemental fixed-input corpus provides another independent
check rather than relying only on two paths through the same native hash routine.

SHA3's valid method constructs its fixture, but its remaining code is commented
out. Its single NumberValue(160) argument reaches type rejection. Its other 22
invalid fixtures have the wrong number of arguments, so they do not prove the
bit-length and type rules suggested by old comments.

CHAINSHATests is a historical name: the active class under test is PROOF.
Its valid method is entirely commented out. Every invalid fixture supplies zero
to three parameters; all twenty reject at the five-argument count check. No proof
bytes are decoded by those assertions. Leaving that empty source method intact
is faithful translation; presenting it as successful proof validation would be
misleading. The rest of this counterpart contains the real constructor and
rejection assertions, not a placeholder implementation.

## Supplementary evidence

The existing KissVMOracle and native minima_kissvm probe evaluate the same fixed
expressions. Fifteen byte lengths per hash include empty data, SHA-256 and SHA3
block boundaries, and the 65,536-byte HexValue limit. Six strings per hash include
empty text, repeated spaces and UTF-8 characters. Python hashlib independently
computes the expected 42 digests from the original bytes. Hashing the hexadecimal
display string would fail these checks.

Four existing Java-generated MMR fixtures check hex and script leaves, each with
a correct and incorrect sum. They independently expect TRUE, FALSE, TRUE, FALSE.
These are supplemental tests, not revived commented-out source tests. Together,
46 expressions are compared, including values, instruction counts and traces;
92 independent expected-value checks cover both runtimes. Successful evaluations
must have empty exception fields, so generic error normalization cannot make
these cases pass. This does not establish arbitrary malformed-proof exception
parity or complete MMR behavior.

Run from the repository root:

```sh
cmake --build cpp/build --target SHA2Tests SHA3Tests CHAINSHATests minima_kissvm MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_sha_tests$' --output-on-failure
```

Repeat with `cpp/build-sanitized` for the configured sanitizer build. Results and
scope are saved in the [checkpoint evidence](../../validation/original-sha-tests-work-in-progress/progress.json).

## A build lesson

On this macOS filesystem, adding the project root to include paths caused the
standard `<version>` include to resolve to the project's `VERSION` text file.
The fix follows existing test files: use a relative include for the test helper
and give that helper its explicit MinimaFunction dependency. No production
algorithm needed a change for these tests.

Exercise: explain why an invalid five-argument PROOF call would test a different
path from every original CHAINSHATests rejection. Then identify which observation
in the supplemental corpus would catch hashing UTF-8 display text instead of
raw hex bytes.
