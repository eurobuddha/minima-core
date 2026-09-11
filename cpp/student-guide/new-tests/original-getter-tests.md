# Original transaction getter tests: selecting the right coin data

This batch maps seven files: GETINADDRTests, GETINAMTTests, GETINIDTests,
GETINTOKTests, GETOUTADDRTests, GETOUTAMTTests and GETOUTTOKTests. It reuses the
native transaction getter functions and existing Coin, Token, Address,
ScriptProof, Witness and Contract implementations. No production algorithm
changes are required by these tests.

## Reuse and fixture construction

Each source file defines newSimpleAddress. It creates 32 random bytes, inserts
their hexadecimal spelling into `RETURN SIGNEDBY(...)`, and constructs a real
Address. Each mapped file preserves this helper. Tests create fresh address
sets separately for their valid and invalid methods; neither random sizes nor
fixture operations are reduced.

Most fixtures contain two inputs with amounts 25 and 75 and two outputs with
amounts 40 and 60. They attach the first two scripts to a real Witness. Address
getters select the appropriate input/output address; token getters return the
stored token ID. Their original coins all use Token.TOKENID_MINIMA, so these
particular token assertions cannot distinguish two different token IDs.

GETINID is different: after checking that the transaction input index exists,
it reads the coin ID from the corresponding Witness coin proof. The valid test
adds both proofs; its invalid method does not. Both source input coin IDs are
COINID_OUTPUT, so the original valid assertions alone would also pass an incorrect
implementation returning that constant. Supplemental fixtures distinguish them.

The inspected sibling `tools/minima-core-cpp/src/kissvm/functions/Functions.cpp`
reads GETINID from transaction inputs and returns raw amounts without the pinned
token scaling. Its GETINTOK also substitutes empty bytes for a missing token.
Those entry points are incompatible with the pinned source. The current native
implementations already select the required data and are reused unchanged.

## Amounts and nullable token metadata

The amount tests create three inputs and three outputs. Their custom Token has
scale 10: the displayed token quantity multiplies its stored amount by ten ten
times. GETINAMT must return 75 × 10¹⁰ for the second input, while GETOUTAMT must
return 30 × 10¹⁰ for the second output. The source compares these as long integers;
the native tests preserve that conversion. The third coin carries a custom token
ID with no Token metadata, so its amount lookup must raise ExecutionException.

There are deliberate translation details even where source code looks accidental:

- GETINAMT's valid fixture clears `out2` metadata after adding it to the transaction;
  its invalid fixture does not perform that assignment.
- GETOUTAMT's valid fixture assigns the absent metadata to `out3`; its invalid
  fixture assigns it to the already-inserted `out2` instead.

These exact assignments are retained. Native Coin copies use the existing shared
object handle, so mutating an inserted coin still updates that coin in the
transaction. The original getter assertions do not independently prove every
such alias mutation; preserving the fixture is distinct from claiming coverage.

## Assertions and execution

| Original class | Constructors | Valid method | Invalid method | Total |
| --- | ---: | ---: | ---: | ---: |
| GETINADDRTests | 4 | 4 | 7 | 15 |
| GETINAMTTests | 4 | 5 | 7 | 16 |
| GETINIDTests | 4 | 4 | 7 | 15 |
| GETINTOKTests | 4 | 4 | 7 | 15 |
| GETOUTADDRTests | 4 | 4 | 7 | 15 |
| GETOUTAMTTests | 4 | 5 | 7 | 16 |
| GETOUTTOKTests | 4 | 4 | 7 | 15 |

All 21 annotated methods execute 107 successful-path assertions per runtime.
The unannotated address helper is also translated and called. Wrong arity,
negative/out-of-range indexes and wrong value types retain the source's specific
ExecutionException expectation. A different native exception fails the test.

The runner adapts OriginalStateTests and its Python driver. It verifies the exact
annotated method set, default options and absence of lifecycle methods before
calling unchanged Java methods on fresh instances with real JUnit assertions.
Native counters track executed checks; Java counts come from reviewed successful
source paths. The two runtimes generate independent random fixtures, so these
random values are not reported as identical Java/C++ inputs.

## Supplemental comparisons

The existing KissVMOracle and native minima_kissvm probes provide a richer fixed
fixture. Nineteen expressions check addresses, token IDs, amounts and witness
coin IDs. The latter are `0xA0`, `0xA1` and `0xA2`, distinct from transaction IDs.
The custom token uses scale two, yielding 25 input tokens and 12.5 output tokens.
That fractional output also checks behavior beyond the original integer results.

Each runtime must produce the expected value and no exception: 38 independent
expectation checks. Complete result records, including number wire encoding,
instruction counts, context and traces, are also compared across runtimes.
Custom token-ID checks compare against the fixture's actual Token ID. Matching
generic errors cannot pass. These checks supplement the source assertions; they
do not replace any original case or establish full node parity.

Run from the repository root:

```sh
cmake --build cpp/build --target GETINADDRTests GETINAMTTests GETINIDTests GETINTOKTests GETOUTADDRTests GETOUTAMTTests GETOUTTOKTests minima_kissvm MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_getter_tests$' --output-on-failure
```

Use `cpp/build-sanitized` for the configured sanitizer build.
[Checkpoint evidence](../../validation/original-getter-tests-work-in-progress/progress.json)
records results, source pin and remaining files. Full getter API, arbitrary
mutation, malformed inputs, concurrency and complete node behavior remain
unverified.

Exercise: design a fixture where a transaction input and its witness proof have
different coin IDs. Predict GETINID's result and identify why the original test's
COINID_OUTPUT values could conceal a wrong implementation.
