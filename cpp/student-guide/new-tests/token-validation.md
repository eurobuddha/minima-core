# Lab: validate a token signature and its web file

Written before the new test. This command combines two independent checks. A
decoded signature is verified over the token's **coin ID**, not its token ID or
the full metadata. A web response is stripped of line breaks by RPCClient, then
trimmed and compared with the canonical token ID. Negative validation results
belong inside the response; the command status can still be true.

Reuse the real token search, Signature decoder, TreeKey verifier, JSON parser
and RPCClient. Generate signatures with the pinned Java key code, then verify
them in both runtimes. Test missing fields, wrong types, nulls, malformed wire,
wrong roots, wrong signed messages and valid signatures. A missing signature
or signedby field means unsigned, even if the other field contains invalid data.

Use the existing loopback HTTP server. Compare actual GET requests, valid and
wrong response bodies, whitespace, line removal, status failures and redirects.
An invalid but decoded signature must not prevent the web check. A signature
decode error must prevent it. A URL type error occurs outside the download catch;
a null URL passes the string cast and fails inside that catch.

Record logs as well as replies. Java's Signature conversion catches and logs
IOException, returning null; other decoding exceptions escape. The existing
native conversion catches every standard exception and omits logging, which
must be corrected using the already tested TxPoW conversion/logging pattern.

These checks prove particular metadata relationships. They do not certify token
value, contract safety, all metadata fields, durable storage or a complete node.

## What the test found

The first expectation error concerned empty public keys. Empty hexadecimal data
is permitted by MiniData. As `signedby`, it therefore produces a root mismatch
and `valid:false`; it is not automatically a command error. The fixture now
checks that behavior. The initial expectation failure is retained.

After 108 comparisons agreed, a null imported token exposed a real exception
mismatch: Java raised NullPointerException and native discovery raised a generic
RuntimeException. The native searcher now preserves Java's category. Token
validation of exactly 0x00 still rejects before consulting that broken import.

The decoder tests also distinguish an invalid negative length (logged IOException,
then null signature) from a zero-length BigInteger (NumberFormatException escapes
without an I/O log). This validates the narrowed catch rather than merely testing
successful decoding. A wrong root still permits a successful subsequent web check.

Both builds pass 126 programs / 254 operations, with 33 real HTTP requests per
runtime. CTest durations are 3.78 seconds normal and 10.26 seconds under ASAN/UBSAN.
See [saved evidence](../../validation/token-validation-work-in-progress/progress.json).

The wider command comparison exposed a test protocol error after logging was
restored: stdout contained 12,290 lines for 12,158 inputs. A diagnostic log is
not a command result. The native command fixture now reuses the transaction
fixture's stderr logger callback, matching the Java oracle's output routing.
Production signature logging remains enabled. The corrected comparison passes
in both builds, alongside the other six affected regression comparisons.

Build target names matter: `minima_commands` is the command-line executable,
whereas this comparison runs `minima_command`. An initial sanitizer pass used
the older singular-target binary. Rebuilding that exact target and rerunning
the test established the final result against the current library.
