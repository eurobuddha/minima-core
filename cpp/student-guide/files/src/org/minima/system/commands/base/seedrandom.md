# seedrandom — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/seedrandom.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/seedrandom.cpp)

## What this file does

Derive a deterministic value from the wallet base seed and a user modifier. The same seed and modifier produce the same value; the command does not draw new random bytes.

## Follow the logic

1. Check Wallet.isBaseSeedAvailable before parsing any parameter. The exact stored String 0x00 is the locked sentinel. A locked wallet returns DB locked! even if modifier is missing; missing seed state can instead throw a null-pointer error.
2. Read and trim the required modifier using Command’s Java String rules. Preserve that UTF-16 text for the response. Construct MiniString from its UTF-8 bytes; Java replaces an isolated surrogate with a question-mark byte.
3. Serialize MiniString, which produces a four-byte length followed by its bytes. Wrap the resulting wire in MiniData. The first Crypto.hashObjects call serializes that object again, then serializes MiniData(0xDEADDEAD), and hashes their concatenation using SHA3-256.
4. Read the actual cached SeedRow and parse its stored hex seed. Hash the serialized first digest followed by the serialized seed using Crypto.hashObjects again. No SQL query is needed for these cached getters.
5. Return the trimmed modifier and final 0x-prefixed digest through the source reply envelope. The native command is registered, including the existing runner permission classification.

## How the translation is approached

The counterpart composes the existing Wallet, SeedRow, Command, MiniString, MiniData and Crypto implementations. getParamUTF16 retains source response text; the existing Java Unicode encoder supplies MiniString bytes. sqlRequired and the existing Java null-pointer exception preserve missing-owner behavior. The translation introduces no alternative key derivation or new hashing dependency.

## What the tests tell us

[The lab written before the tests](../../../../../../../new-tests/seed-random-command.md) and [the executable corpus](../../../../../../../../tools/seed_random_corpus.py) compare 309 programs, 782 operations and 14,616 SQL observations with Java, using only synthetic fixture seeds. Python hashlib independently verifies both hashes with all four-byte framing boundaries. Repeated and changed inputs, Unicode, boundary whitespace, embedded zero bytes, seed encodings, locked state, malformed fields, partial wallet initialization/update and public permissions are covered. Both full builds and comparisons pass (12.30s normal, 5.50s sanitizer); all four selected regressions pass (23.31s/75.25s). No production discrepancy was found. Fixture stdout is UTF-8, so isolated-surrogate response observations become question marks; this is an explicit transport boundary, not a claim of exhaustive retained String identity.

## Check your understanding

If M is the modifier’s UTF-8 bytes and B(X) prefixes X with its four-byte length, what enters the first hash? B(B(M)) followed by B(DE AD DE AD). Why is hashing the plain modifier and plain seed incompatible? It omits both object boundaries and the separating marker.

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. Synthetic wallet rows and finite vectors do not prove live wallet integration, durable SQL, concurrent seed replacement behavior, arbitrary retained String aliases or the source’s cryptographic separation claim. No user private seed was accessed.

Pinned Java SHA-256: `77834ac236d270855391d060611e4ed718ea8cf3aa9d47794df46013bee03680`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
