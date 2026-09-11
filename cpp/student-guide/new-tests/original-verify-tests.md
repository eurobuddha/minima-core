# Original VERIFY tests: comparing a coin against a contract's requirements

VERIFYIN selects a transaction input. VERIFYOUT selects an output. Both compare
its address, token ID and amount with arguments supplied by a script. A match
returns Boolean TRUE; an ordinary mismatch returns FALSE. An invalid index,
wrong argument count or missing custom-token metadata raises ExecutionException.
These are distinct outcomes, and the tests retain that distinction.

## Reused code and exact fixtures

The pinned VERIFYINTests.java and VERIFYOUTTests.java define three annotated
methods each and a newSimpleAddress helper. The mapped C++ files retain all
active cases. The helper still generates 32 random bytes and constructs a real
Address from RETURN SIGNEDBY(...). Each valid/invalid method creates six addresses,
three inputs, three outputs, a Witness with two ScriptProof objects, a custom
Token and a Contract. Random IDs remain 16 bytes. Java and C++ generate independent
random fixtures; these are not claimed to be identical-input comparisons.

Input amounts are 50, 75 and 1; output amounts are 40, 30 and 1. The second coin
uses a scale-10 token, so verification must compare 75 × 10¹⁰ or 30 × 10¹⁰,
respectively. Supplying the raw stored amount must return FALSE. The third coin
has a custom token ID but no Token metadata, so verification must throw even
when another compared field does not match.

Both VERIFYIN fixtures and VERIFYOUT's valid fixture assign absent metadata to
out3. VERIFYOUT's invalid fixture instead clears out2 after inserting it into
the transaction. This exact source assignment is preserved through existing
Coin shared handles. Its presence in the fixture does not imply that the original
invalid-argument assertions independently test alias mutation.

The shared OriginalFunctionTestSupport.hpp, existing VERIFYIN/VERIFYOUT production
classes and their Address/Coin/Token/Witness/Contract dependencies are reused.
No production algorithm changes were needed. The inspected sibling
`tools/minima-core-cpp/src/kissvm/functions/Functions.cpp` allows three arguments,
uses raw amounts, skips mismatched types and lacks the pinned keep-state option;
those functions cannot be substituted for the pinned implementation.

## Original assertion coverage

| Class | Constructors | Valid parameters | Invalid parameters | Total |
| --- | ---: | ---: | ---: | ---: |
| VERIFYINTests | 4 | 17 | 7 | 28 |
| VERIFYOUTTests | 4 | 17 | 7 | 28 |

The valid method has eight Boolean results, each checked for both its type and
truth value, followed by one missing-metadata rejection. Invalid methods retain
all seven arity/index rejection cases. Only ExecutionException satisfies the
native rejection helper; another exception fails the executable.

OriginalVerifyTests adapts the established direct Java runner. It verifies the
annotated method set, default JUnit options and absence of lifecycle methods,
then calls every unchanged source method on a fresh instance using actual JUnit
assertions. Native assertion counts are measured; Java's reported 28 per class
comes from reviewing the successful source paths. Six methods and 56 assertions
per runtime pass in both normal and sanitizer configurations.

Commented historical numeric amount-check options remain inactive. The active
VERIFYOUT source uses a Boolean fifth parameter: whether the output stores state.
Every original valid output fixture stores state and passes TRUE. These original
cases therefore do not establish the omitted or FALSE option's behavior.

## Supplemental validation

The existing KissVMOracle and minima_kissvm probes provide fixed inputs and
outputs with both Minima and custom tokens. Their scaled amounts are 7 and 25
for inputs, and 5 and 12.5 for outputs. VERIFYIN exercises matching fields and
one incorrect address, amount or token. VERIFYOUT repeats those cases with the
keep-state argument omitted, TRUE and FALSE, against outputs that store state
and outputs that do not.

There are 32 identical-input Java/C++ comparisons and 64 independent expected
Boolean checks in each build. Every comparison includes all 13 result/context/
trace fields, including the source spelling in VERIFYOUT's failure trace.
Matching exceptions cannot pass: each supplemental case must return its expected
Boolean without an error. These cases add coverage without replacing source tests.

```sh
cmake --build cpp/build --target VERIFYINTests VERIFYOUTTests minima_kissvm MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_verify_tests$' --output-on-failure
```

Use cpp/build-sanitized for the configured sanitizer build. Normal CTest passed
in 2.78 seconds; sanitizer CTest passed in 4.03 seconds. Both real node targets
build. [Evidence](../../validation/original-verify-tests-work-in-progress/progress.json)
records the source pin, results and remaining files. Full production API and
node parity remain unverified.

Exercise: explain why a coin with missing token metadata must throw rather than
silently compare its raw amount, then design a test that distinguishes a FALSE
keep-state argument from omitting that argument.
