# Changes in the timed delivery window

## Newly converted files

No new mapped counterpart was added after checkpoint `8e83642`. The inventory is
456 counterparts for 457 Java files, with `jnifunctions.cpp` absent. The work below
fills behavior in existing counterparts. It does not promote them to full parity.

## Behavior added or corrected

Sixteen missing Java demonstration entry points were added. They call the existing
native helpers, retain source operation order, and have normal and ASAN/UBSAN
Java/native comparisons. The finite missing-main audit now has zero candidates;
that audit does not enumerate or certify every class API.

| Existing counterpart | Behavior filled |
| --- | --- |
| `system/commands/send/send.cpp` | Source demonstration and coin ordering |
| `kissvm/Contract.cpp` | Demonstration, trace logging and notifications |
| `database/txpowtree/TxPoWTreeNode.cpp` | Source tree demonstration |
| `system/network/p2p/P2PFunctions.cpp` | Availability demonstration; ordinary exceptions versus Java Error |
| `utils/encrypt/EncryptDecrypt.cpp` | Encryption/decryption demonstration |
| `utils/encrypt/PasswordCrypto.cpp` | Password envelope demonstration |
| `utils/encrypt/CryptoPackage.cpp` | Key package demonstration |
| `utils/encrypt/SignVerify.cpp` | Signature demonstration |
| `objects/mmr/MMR.cpp` | Demonstration and exact printer byte/index behavior |
| `objects/mmr/MegaMMR.cpp` | Demonstration with real random draws and pruning |
| `system/brains/TxPoWMiner.cpp` | Complete 2,010,000-hash demonstration workload |
| `objects/keys/TreeKey.cpp` | Successive stateful signature demonstration |
| `database/cascade/Cascade.cpp` | File-based demonstration and default logger notifications |
| `database/archive/ArchiveManager.cpp` | Durable archive demonstration and reopen sequence |
| `database/txpowdb/onchain/TxPoWOnChainDB.cpp` | Repeated on-chain row insertion demonstration |
| `database/txpowdb/sql/TxPoWSqlDB.cpp` | Relevant TxPoW persistence demonstration |

Paths above are relative to `cpp/src/org/minima/`.
`MiniFile` missing-file loading also now uses MinimaLogger. Other malformed-file
logging paths are outside this correction's scope.

## Validation of those changes

| Batch | Completed comparison evidence per build | Teaching lab |
| --- | --- | --- |
| Core | 24 comparisons, 87 assertions | [Core mains](../student-guide/new-tests/core-main-methods.md) |
| Crypto | 24 runs, 24 cross-decryptions, 222 assertions | [Crypto mains](../student-guide/new-tests/crypto-main-methods.md) |
| MMR | 30 comparisons, 112 assertions | [MMR mains](../student-guide/new-tests/mmr-main-methods.md) |
| Runtime | 18 comparisons, 121 assertions | [Runtime mains](../student-guide/new-tests/runtime-main-methods.md) |
| Database | 18 runs, 36 cross-reads, 465 assertions | [Database mains](../student-guide/new-tests/database-main-methods.md) |

Each batch passed in both normal and sanitizer builds. The H2 dependency itself
is not sanitizer instrumented. Random outputs are checked through cross-language
read/decrypt and internal relationships, rather than equating independent draws.
Database identity allocation and Java/C++ object lifetime remain explicit limits.

## Test and delivery infrastructure

The full-size HORST comparison passed after its teardown allowance was changed
from five to sixty seconds; the original failure is retained. No cryptographic
workload was reduced. The preview verifier now waits for all connection readiness
conditions within its existing 150-second deadline and captures failure tracebacks.
Live workflow failures remain recorded; see [validation status](VALIDATION_STATUS.md).

Exported snapshots validate the pinned source and mapping against the delivery
manifest without depending on the original checkout or sibling project catalogue.
Ten positive/negative snapshot checks cover corrupt or missing source/map inputs.
Student evidence links use committed reports, and the packager audits all 457
lessons in the actual exported project. Packaging checks every archive payload and
verifies the provenance of the native database library actually bundled.

## Blockers and next actions

Obtain the original five JNI method implementations and ABI before implementing
`jnifunctions.cpp`. Establish a reproducible successful Java/native live startup and
reopen comparison before changing consensus processing rules. Expand per-file API,
exception, ownership and concurrency validation beyond the present finite corpus.
See [known gaps](KNOWN_GAPS.md) and the [delivery review](REVIEW.md).
