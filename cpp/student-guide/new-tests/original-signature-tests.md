# Original signature tests: membership and cryptographic verification

These three original test classes cover related but different operations.
SIGNEDBY searches the Contract's supplied signer list. MULTISIG counts matches
from that list until its requested threshold is reached. CHECKSIG decodes a real
signature and verifies it against a message and public key. Membership in a list
alone cannot establish that a cryptographic signature is valid.

## Reuse first

The C++ tests reuse the production signature functions, Contract and its value
classes. CHECKSIG uses the existing TreeKey, TreeKeyNode, Signature,
SignatureProof and Winternitz code. The original fixture constructs two
`TreeKey(seed, 16, 2)` objects, with 32-byte and 64-byte random seeds and messages;
these sizes and depths are retained. Signature objects are serialized through
the native production writer before CHECKSIG decodes them.

The inspected sibling `tools/minima-core-cpp/src/kissvm/functions/Functions.cpp`
uses RSA or a witness-based fallback for CHECKSIG. Its SIGNEDBY searches Witness
objects rather than the supplied Contract signer list. Those entry points do
not match this pinned Java source. The current native functions already follow
the source behavior and are reused unchanged.

## Follow the fixtures

SIGNEDBY generates two 20-byte values. It supplies the first value as a signer,
then checks that the first matches and the second does not. The second constructed
signer list is unused in Java and retained in the translation. The values are
random data: the source deliberately does not generate real keys for this test.

MULTISIG generates eleven signer values of sizes 20 through 64 bytes in steps
of four, skipping 44 bytes exactly as the source does. For every threshold from
one through eleven it checks three cases: enough tracked signers, one tracked
signer replaced by an untracked value, and a threshold exceeding the number of
tracked signers supplied. All 33 cases remain active. Two invalid fixtures both
use a negative threshold; the comment on the second mentions zero, but its code
still passes -1. The translation preserves that duplicate and does not claim
zero-threshold coverage from it.

CHECKSIG makes one signature with each real key. Its eight valid-input fixtures
include both correct triples and six combinations of wrong key, message or
signature. Each checks the BooleanValue type and TRUE/FALSE result. Its empty
public-key/signature fixture raises ExecutionException in the pinned production
code; the old source comment about a NullPointerException does not describe that
current guard. Every original rejection fixture is retained in source order,
using the same Contract for the method.

## Count successful-path assertions

| Original class | Constructors | Valid cases | Invalid cases | Total |
| --- | ---: | ---: | ---: | ---: |
| SIGNEDBYTests | 4 | 4 | 5 | 13 |
| MULTISIGTests | 4 | 66 | 10 | 80 |
| CHECKSIGTests | 4 | 16 | 35 | 55 |

All nine methods execute 148 assertions per runtime on successful paths. The
native helper counts each executed assertion and catches only the expected
ExecutionException. A different exception fails the test. The Java wrapper
checks the exact annotated method set, default options and absent lifecycle
methods, then directly invokes unchanged methods using real JUnit assertions.
Its reported count is derived from reviewed successful source paths, not JUnit
instrumentation. Independently generated random fixtures are not byte-for-byte
Java/C++ comparisons.

## Cross-runtime signature evidence

The supplemental driver reuses existing ObjectOracle/minima_objects probes.
Both runtimes construct two deterministic 16-key, two-level trees from fixed
32-byte and 64-byte seeds. They sign fixed messages and compare the complete
public keys, serialized signatures, use counters and verification results.
Each tree must report 256 maximum uses, one used key, a 32-byte public key and a
successful verification. A matching pair of generic ERROR replies cannot pass.

Both runtimes then verify signatures emitted by each writer. Every signature is
checked with its correct message, an altered message and a wrong public key.
That tests compatibility between the writers and readers instead of relying only
on a runtime verifying its own output. The two generation comparisons and twelve
verification comparisons have 28 independent expectation checks across the two
runtimes. These are supplemental checks, counted separately from the 148 original
assertions. They do not establish every malformed-wire, mutation, exhaustion,
concurrency or complete node behavior.

Run from the repository root:

```sh
cmake --build cpp/build --target SIGNEDBYTests MULTISIGTests CHECKSIGTests minima_objects MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_signature_tests$' --output-on-failure
```

Use `cpp/build-sanitized` for the configured sanitizer build.
[Checkpoint evidence](../../validation/original-signature-tests-work-in-progress/progress.json)
records actual outcomes, scope and remaining files. Full behavioral parity remains
unverified even when these original and supplemental tests pass.

Exercise: explain how a false signer-list entry could make SIGNEDBY return TRUE
while CHECKSIG rejects a signature. Then explain why cross-runtime verification
can detect a shared writer/reader mistake that a runtime's own round trip misses.
