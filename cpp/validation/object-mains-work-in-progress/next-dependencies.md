# Remaining entry-point dependencies

Inspected the complete live main bodies in the pinned source for all 16 rows in
`remaining-main-candidates.json`. These are implementation candidates, not absent
counterparts or a complete public-API inventory. Inspect each called native helper
and its tests before implementing the next batch.

| Candidate | Source dependency and next action |
| --- | --- |
| Contract | Fixed shift script, cleanScript, debug constructor and run. Reuse VM and logger fixtures; compare complete trace. |
| TxPoWMiner | Two old hash-rate runs (10,000 and 1,000,000), one newer 1,000,000 run. Preserve full work; reuse existing hash-rate runtime/elapsed-time fixtures. |
| send | Six fixed coins and orderCoins. Reuse ordering implementation; compare all six full coin JSON outputs. |
| P2PFunctions | isNetAvailable. Reuse existing network environment injection to check success/failure without a public-network dependency. |
| EncryptDecrypt | Random data/salt/IV, AES stream envelope, then construct decrypting stream without reading it (later reads are commented out). Reuse crypto and byte stream implementations; compare envelope/decryption semantics without activating commented code. |
| CryptoPackage | Random key pair and data, package encryption/decryption. Reuse native crypto provider; check actual sizes, interoperability and result. |
| PasswordCrypto | Random 32-byte input, password encryption/decryption. Reuse compression/envelope fixtures; inspect salt/IV boundary for fixed inputs. |
| SignVerify | Provider registration, key generation, 1024-byte random message, sign/verify. Reuse actual crypto; preserve registration effect through existing provider model. |
| MMR | Ten equal leaves, printmmrtree, final blank line. Inspect print helper; source prints a 2048-character buffer with only the first 512 initialized to spaces, so embedded NULs matter. |
| MegaMMR | MMR printer plus two ten-coin construction phases, repeated pruning. Complete MMR printer first; preserve source random calls even where addresses are overwritten. |
| TreeKey | Fixed seed, 4-by-4 key tree, two signatures on the same data, full pretty JSON logs. Reuse key/signature and logger implementations; test complete output and successive signing state. |
| ArchiveManager | Deletes user.home/testfolder, loads SQL DB, inserts/loads block with repeated hackShut. Implement actual path semantics but test only with fixture-owned home/runtime mapping; never execute this demo against the user's real home. |
| TxPoWTreeNode | Three fixed strings, Java list text stored as a JSON string. Reuse JSONObject and list-text rendering; transactions is deliberately a string rather than an array. |
| Cascade | Literal Windows path, loadDB, checkCascadeCorrect. Test source missing-file behavior and fixture-owned files under the same literal spelling on this platform. |
| TxPoWOnChainDB | user.home/testsql, schema creation, three fixed rows, two lookups. Reuse native SQL runtime and fixture-owned home. |
| TxPoWSqlDB | user.home/testsql, random/time TxPoW, insert and shutdown. Reuse native SQL, time and TxPoW fixtures; compare complete serialized content with controlled inputs. |

None of these source main bodies calls the missing JNI file. The unavailable JNI
native bodies therefore do not block this independent entry-point work. Their
missing contracts remain a separate full-port blocker. A filesystem path in a demo
is a dependency to isolate during validation, not grounds to omit the demo.
