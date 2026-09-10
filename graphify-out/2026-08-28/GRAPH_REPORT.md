# Graph Report - minima-core  (2026-08-23)

## Corpus Check
- 462 files · ~251,491 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 4548 nodes · 20705 edges · 212 communities (77 shown, 135 thin omitted)
- Extraction: 77% EXTRACTED · 23% INFERRED · 0% AMBIGUOUS · INFERRED: 4715 edges (avg confidence: 0.8)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `c081533d`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- org.junit.Test
- Expression
- .getTxPoW
- .getJSONReply
- Contract
- MiniData
- ScriptProof
- MinimaDB.java
- JSONObject
- .isEqual
- .checkExactParamNumber
- .runCommand
- .run
- .parseTokens
- UserDB
- P2PState
- ParamKeys
- .runCommand
- .getHexParam
- Message
- MinimaDB
- .log
- java.io.DataOutputStream
- .getParameter
- RamDB
- GenerateKey
- TreeKey
- MMRData
- MMREntryNumber
- HORSTSignature
- java.net.InetSocketAddress
- HTTPServer
- .runFunction
- .getBytes
- SeedRow
- MiniNumberTests
- .getNumber
- Coin
- MegaMMR
- .getKnownPeers
- java.io.DataInputStream
- .loadAllDB
- MMRProofChunk
- NIOClientInfo
- backup
- TxPoW
- Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write
- TxBlock
- .parse
- P2P2Functions
- .testConstructors
- MMR
- IBD
- .main
- MegaMMRIBD
- .runCommand
- SPHINCS Signature Verification KISS VM Script
- .runCommand
- .toString
- MessageProcessor
- .runFunction
- .toString
- SendPollManager
- .runCommand
- txncoinlock
- Yylex
- .signData
- .getDB
- restore
- ParamConfigurer
- P2P2DB
- ItemList
- .readDataStream
- .runFunction
- TxPoWList
- MiniString
- Pulse
- .ReadFromStream
- txnaddamount
- hash
- printtree
- status
- txnlock
- tutorial
- connect
- JsonDB
- Tx-PoW
- network
- RPCClient
- mmrproof
- MinimaException
- benchmark
- jniminingtest
- jnitest
- logs
- maths
- quit
- test
- timemilli
- disconnect
- message
- p2pstate
- sendsign
- Maths
- DUMMYFUNCTION
- webhooks
- .toString
- .isTrue
- .convertMiniDataVersion
- .readDataStream
- reset
- balance
- coincheck
- coinexport
- coinimport
- coinnotify
- cointrack
- consolidate
- convert
- getaddress
- hashtest
- incentivecash
- random
- seedrandom
- slavenode
- sphincs
- tokencreate
- tokenvalidate
- trace
- peers
- ping
- rpc
- newscript
- removescript
- runscript
- scripts
- coins
- history
- keys
- tokens
- txpow
- multisig
- multisigread
- send
- sendnosign
- sendpost
- sendview
- rawfrom
- rawtxnfrom
- sign
- verify
- tutorial
- txnbasics
- txncheck
- txnclear
- txncreate
- txndelete
- txnexport
- txnimport
- txninput
- txnlist
- txnoutput
- txnpost
- txnscript
- txnsign
- txnstate
- txnview
- whitepaper
- minima-core
- .convertMinimaAddress
- Stack
- automine
- gradlew
- Storage-less MMR UTXO Database
- .JSONPretty
- createfrom
- postfrom
- checkaddress
- .newFile
- The Burn
- buildjars.sh
- debugflag
- .readDataStream
- healthcheck
- .runCommand
- missingcmd
- newaddress
- printmmr
- nodecount
- .convertMiniDataVersion
- constructfrom
- createtokenfrom
- sendfrom
- signfrom
- txnmine
- txnminepost
- txnmmr
- toParamKey
- Yytoken
- JSONStreamAware
- archive
- jnifunctions
- block
- scanchain
- txnauto
- Override

## God Nodes (most connected - your core abstractions)
1. `MiniData` - 444 edges
2. `JSONObject` - 409 edges
3. `Command` - 383 edges
4. `MiniNumber` - 334 edges
5. `Contract` - 258 edges
6. `MinimaFunction` - 231 edges
7. `NumberValue` - 206 edges
8. `Value` - 197 edges
9. `TxPoW` - 196 edges
10. `HexValue` - 185 edges

## Surprising Connections (you probably didn't know these)
- `SPHINCS Signature Verification KISS VM Script` --semantically_similar_to--> `Merkle Signature Scheme`  [INFERRED] [semantically similar]
  src/org/minima/utils/sphincs/kissvm.txt → Minima_wp_v9.pdf
- `SPHINCS Signature Verification KISS VM Script` --semantically_similar_to--> `Quantum Secure Cryptography (KECCAK + WOTS)`  [INFERRED] [semantically similar]
  src/org/minima/utils/sphincs/kissvm.txt → Minima_wp_v9.pdf
- `Minima Core Full Node Application` --implements--> `Minima Protocol`  [INFERRED]
  README.md → Minima_wp_v9.pdf
- `MegaMMR Node Mode (-megammr)` --references--> `Storage-less MMR UTXO Database`  [INFERRED]
  README.md → Minima_wp_v9.pdf
- `KISS VM Script Grammar (BNF)` --implements--> `Minima Scripting Language (Smart Contracts, Tokens)`  [INFERRED]
  src/org/minima/system/commands/base/tutorial.txt → Minima_wp_v9.pdf

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **SPHINCS On-Chain Verification Flow** — src_org_minima_utils_sphincs_kissvm_sphincs_verification_script, src_org_minima_utils_sphincs_kissvm_transaction_message_hash, src_org_minima_utils_sphincs_kissvm_fors_root_check, src_org_minima_utils_sphincs_kissvm_horst_tree_proof [EXTRACTED 1.00]
- **User-Generated PoW Security Model** — minima_wp_v9_txpow, minima_wp_v9_maxima, minima_wp_v9_pulse_network, minima_wp_v9_burn, minima_wp_v9_distributed_pow, minima_wp_v9_hashcash [EXTRACTED 1.00]
- **Compact Chain Storage Model (runs on a phone)** — minima_wp_v9_cascading_chain, minima_wp_v9_mmr_utxo, minima_wp_v9_adaptive_block_size, minima_wp_v9_complete_node [INFERRED 0.85]

## Communities (212 total, 135 thin omitted)

### Community 0 - "org.junit.Test"
Cohesion: 0.04
Nodes (46): org.junit.Test, HexValue, Override, NumberValue, StringValue, TxPoWProcessorTest, ConstantExpressionTests, FunctionExpressionTests (+38 more)

### Community 1 - "Expression"
Cohesion: 0.05
Nodes (26): BooleanExpression, Expression, Override, OperatorExpression, ASSERTstatement, Override, EXECstatement, Override (+18 more)

### Community 3 - ".getTxPoW"
Cohesion: 0.07
Nodes (4): Override, TxPoWTreeNodeAction, Override, TxPowTreeTest

### Community 5 - "Contract"
Cohesion: 0.12
Nodes (9): Contract, ExecutionException, MinimaParseException, ConstantExpression, MinimaFunction, BooleanValue, Value, Transaction (+1 more)

### Community 6 - "MiniData"
Cohesion: 0.05
Nodes (15): java.math.MathContext, MiniData, MathContext, MiniNumber, Greeting, Magic, DataOutputStream, TxHeader (+7 more)

### Community 7 - "ScriptProof"
Cohesion: 0.14
Nodes (10): ScriptProof, GETINADDRTests, GETINAMTTests, GETINIDTests, GETINTOKTests, VERIFYINTests, GETOUTADDRTests, GETOUTAMTTests (+2 more)

### Community 8 - "MinimaDB.java"
Cohesion: 0.06
Nodes (30): BufferedInputStream, FileInputStream, GZIPInputStream, java.io.BufferedInputStream, java.io.FileInputStream, java.sql.Connection, java.sql.PreparedStatement, java.sql.Statement (+22 more)

### Community 9 - "JSONObject"
Cohesion: 0.09
Nodes (9): TxnDB, TxnRow, Address, Command, CommandException, CommandRunner, JSONArray, JSONAware (+1 more)

### Community 10 - ".isEqual"
Cohesion: 0.08
Nodes (3): TxPoWChecker, SPHINCSUtils, TransactionTests

### Community 11 - ".checkExactParamNumber"
Cohesion: 0.03
Nodes (38): Override, UTF8, BITCOUNT, Override, BITGET, Override, BITSET, Override (+30 more)

### Community 12 - ".runCommand"
Cohesion: 0.09
Nodes (7): Override, mysql, Override, mysqlcoins, Override, ArchiveServer, MySQLConnect

### Community 13 - ".run"
Cohesion: 0.03
Nodes (12): java.nio.ByteBuffer, java.nio.channels.SelectionKey, java.nio.channels.Selector, java.nio.channels.SocketChannel, MegaMMRSyncData, Override, NIOClient, Override (+4 more)

### Community 14 - ".parseTokens"
Cohesion: 0.10
Nodes (8): ExpressionParser, LexicalTokenizer, Override, ScriptTokenizer, ExpressionParserTests, StatementParserTests, LexicalTokenizerTests, TokenTests

### Community 16 - "P2PState"
Cohesion: 0.11
Nodes (5): InetSocketAddress, Override, P2PManager, InetSocketAddress, P2PState

### Community 17 - "ParamKeys"
Cohesion: 0.03
Nodes (68): ParamKeys, allowallip, allowgenmessage, anyseed, archive, basefolder, clean, conf (+60 more)

### Community 18 - ".runCommand"
Cohesion: 0.09
Nodes (3): consolidatefrom, Override, txnutils

### Community 19 - ".getHexParam"
Cohesion: 0.06
Nodes (18): ASCII, Override, Override, OVERWRITE, Override, REV, Override, SUBSET (+10 more)

### Community 20 - "Message"
Cohesion: 0.07
Nodes (10): Override, Level, DEBUG, INFO, NODE_RUNNER_MSG, P2PFunctions, Override, Override (+2 more)

### Community 21 - "MinimaDB"
Cohesion: 0.06
Nodes (8): java.util.concurrent.locks.ReentrantReadWriteLock, org.minima.utils.SqlDB, ReentrantReadWriteLock, SeedRow, MinimaDB, TxPowTree, Wallet, P2PDB

### Community 22 - ".log"
Cohesion: 0.06
Nodes (9): java.lang.Thread.UncaughtExceptionHandler, Override, TxPoWOnChainDB, TxPoWSqlDB, CoinDB, Override, Override, MinimaUncaughtException (+1 more)

### Community 23 - "java.io.DataOutputStream"
Cohesion: 0.09
Nodes (9): java.io.DataOutputStream, DataOutputStream, Override, Override, Override, DataOutputStream, Override, DataOutputStream (+1 more)

### Community 24 - ".getParameter"
Cohesion: 0.09
Nodes (12): BOOL, Override, Override, NUMBER, Override, STRING, Override, LEN (+4 more)

### Community 25 - "RamDB"
Cohesion: 0.06
Nodes (6): RamData, RamDB, Override, mempool, RamDBTest, TestUtils

### Community 27 - "GenerateKey"
Cohesion: 0.09
Nodes (12): java.security.KeyPair, java.security.PrivateKey, java.security.PublicKey, javax.crypto.Cipher, javax.crypto.SecretKey, CryptoPackage, DataInputStream, Override (+4 more)

### Community 28 - "TreeKey"
Cohesion: 0.08
Nodes (7): MiniData, org.bouncycastle.pqc.crypto.gmss.util.WinternitzOTSignature, SignatureProof, TreeKey, TreeKeyNode, Winternitz, WinternitzOTSignature

### Community 29 - "MMRData"
Cohesion: 0.13
Nodes (4): MMRData, Override, mmrcreate, mmrleafnode

### Community 30 - "MMREntryNumber"
Cohesion: 0.13
Nodes (3): MMREntry, Override, MMREntryNumber

### Community 31 - "HORSTSignature"
Cohesion: 0.15
Nodes (5): HORST, HORSTSignature, DataInputStream, PrivateKey, PublicKey

### Community 32 - "java.net.InetSocketAddress"
Cohesion: 0.06
Nodes (11): java.net.InetSocketAddress, org.junit.Before, InetSocketAddress, InetSocketAddress, P2PDoSwap, Override, P2PGreeting, P2PPeersChecker (+3 more)

### Community 33 - "HTTPServer"
Cohesion: 0.16
Nodes (8): java.net.ServerSocket, java.net.Socket, ServerSocket, CMDHandler, HTTPServer, Override, ArchiveHandler, Override

### Community 34 - ".runFunction"
Cohesion: 0.06
Nodes (15): Override, VariableExpression, EXISTS, Override, FUNCTION, Override, GET, Override (+7 more)

### Community 38 - ".getNumber"
Cohesion: 0.08
Nodes (10): Override, CEIL, Override, FLOOR, Override, Override, SQRT, Override (+2 more)

### Community 39 - "Coin"
Cohesion: 0.15
Nodes (14): TxPoWDB, TxPoWTreeNode, ScriptRow, Coin, CoinProof, Signature, MMRProof, StateVariable (+6 more)

### Community 40 - "MegaMMR"
Cohesion: 0.12
Nodes (4): MegaMMR, Override, megammr, MegaMMRBackup

### Community 41 - ".getKnownPeers"
Cohesion: 0.10
Nodes (9): junit.framework.TestCase, InetSocketAddressIOTest, InetSocketAddress, SwapLinksFunctionsTest, InetSocketAddress, QuickInetLists, QuickState, UtilFuncsTest (+1 more)

### Community 42 - "java.io.DataInputStream"
Cohesion: 0.06
Nodes (23): java.io.DataInputStream, Override, DataInputStream, Override, Override, Override, Override, Override (+15 more)

### Community 44 - "MMRProofChunk"
Cohesion: 0.11
Nodes (5): Override, DataInputStream, Override, MMRProofChunk, MiniByteTests

### Community 45 - "NIOClientInfo"
Cohesion: 0.08
Nodes (6): NIOClientInfo, P2PWalkLinks, SwapLinksFunctions, UtilFuncs, WalkLinksFuncs, QuickClients

### Community 47 - "TxPoW"
Cohesion: 0.06
Nodes (6): TxBody, DataOutputStream, TxPoW, TimedChecker, GenesisTxPoW, RelayPolicy

### Community 48 - "Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write"
Cohesion: 0.10
Nodes (19): 2a. An `OutOfMemoryError` is an `Error`, not an `Exception` — `1770d8a`, 2b. Stream the IBD in batches — `a9ef09d`, 2c. Heap exhaustion is process-global — `49feed4`, 2d. The MMR tree is the real ceiling — `33c438d`, Also in this commit — `sendsign` burns a leaf per input coin, Branches, Evidence, minima-core — what we changed in our fork, and why (+11 more)

### Community 49 - "TxBlock"
Cohesion: 0.12
Nodes (3): ArchiveManager, TxBlockDB, TxBlock

### Community 51 - "P2P2Functions"
Cohesion: 0.15
Nodes (5): Level, DEBUG, INFO, NODE_RUNNER_MSG, P2P2Functions

### Community 52 - ".testConstructors"
Cohesion: 0.06
Nodes (9): Override, FunctionExpression, Override, GlobalExpression, Override, Override, VERIFYOUT, ContractTests (+1 more)

### Community 53 - "MMR"
Cohesion: 0.12
Nodes (3): DataInputStream, MMR, GenesisMMR

### Community 55 - ".main"
Cohesion: 0.14
Nodes (8): FORS, FORSSignature, DataInputStream, SPHINCS, DataInputStream, Override, SPHINCSSignature, TransactionTest

### Community 56 - "MegaMMRIBD"
Cohesion: 0.21
Nodes (3): MegaMMRIBD, Override, megammrsync

### Community 58 - "SPHINCS Signature Verification KISS VM Script"
Cohesion: 0.10
Nodes (21): Bitcoin (Satoshi Nakamoto), Complete Node, Ethereum, Immutable / Ossified Protocol, Merkle Signature Scheme, Minima Protocol, Quantum Secure Cryptography (KECCAK + WOTS), Minima Scripting Language (Smart Contracts, Tokens) (+13 more)

### Community 59 - ".runCommand"
Cohesion: 0.13
Nodes (3): Override, PasswordCrypto, TxHeaderTests

### Community 60 - ".toString"
Cohesion: 0.15
Nodes (4): Override, JSONValue, Override, JSONWriter

### Community 61 - "MessageProcessor"
Cohesion: 0.03
Nodes (11): Override, systemcheck, NetworkManager, Override, P2P2Manager, Server, Override, MessageProcessor (+3 more)

### Community 62 - ".runFunction"
Cohesion: 0.18
Nodes (6): HEX, Override, Override, POW, Override, SIGDIG

### Community 63 - ".toString"
Cohesion: 0.10
Nodes (10): ADDRESS, Override, Override, REPLACE, Override, REPLACEFIRST, Override, SUBSTR (+2 more)

### Community 64 - "SendPollManager"
Cohesion: 0.16
Nodes (5): Override, sendpoll, Override, SendPollManager, SendPollMessage

### Community 68 - ".signData"
Cohesion: 0.20
Nodes (4): KeyRow, org.minima.objects.base.MiniData, org.minima.objects.keys.Signature, KeyRow

### Community 69 - ".getDB"
Cohesion: 0.05
Nodes (6): Override, ScriptRow, Override, vault, NotifyManager, BIP39

### Community 71 - "ParamConfigurer"
Cohesion: 0.17
Nodes (4): org.junit.runner.RunWith, ParamConfigurer, UnknownArgumentException, ParamConfigurerTest

### Community 74 - ".readDataStream"
Cohesion: 0.36
Nodes (3): DataInputStream, DataOutputStream, Override

### Community 75 - ".runFunction"
Cohesion: 0.20
Nodes (4): Override, SAMESTATE, Override, STATE

### Community 76 - "TxPoWList"
Cohesion: 0.12
Nodes (10): DataInputStream, Override, TxPoWList, decryptbackup, DataInputStream, Override, DataInputStream, DataInputStream (+2 more)

### Community 77 - "MiniString"
Cohesion: 0.12
Nodes (5): java.nio.charset.Charset, javax.net.ssl.SSLContext, MiniString, JSONParser, ParseException

### Community 79 - ".ReadFromStream"
Cohesion: 0.19
Nodes (5): DataInputStream, DataOutputStream, DataInputStream, Override, CoinTests

### Community 88 - "Tx-PoW"
Cohesion: 0.25
Nodes (8): Cascading Proof Chain, Distributed PoW, GHOST Consensus, IOTA Tangle, Maxima P2P Information Network, P2Pool, Pulse Network, Tx-PoW

### Community 91 - "mmrproof"
Cohesion: 0.36
Nodes (3): Override, mmrleafnode, mmrproof

### Community 108 - ".toString"
Cohesion: 0.18
Nodes (3): Override, Override, MiniStringTests

### Community 110 - ".convertMiniDataVersion"
Cohesion: 0.28
Nodes (3): DataInputStream, Override, Override

### Community 111 - ".readDataStream"
Cohesion: 0.39
Nodes (3): DataInputStream, DataOutputStream, Override

### Community 170 - "minima-core"
Cohesion: 0.40
Nodes (4): Build, MegaMMR node, minima-core, Testing on private network

### Community 174 - "gradlew"
Cohesion: 0.60
Nodes (3): gradlew script, die(), warn()

### Community 175 - "Storage-less MMR UTXO Database"
Cohesion: 0.40
Nodes (5): Storage-less MMR UTXO Database, Peter Todd MMR (Delayed TXO Commitments), MegaMMR Node Mode (-megammr), PROOF Merkle Proof Function, HORST Tree Proof Verification Loop (Step 2)

### Community 181 - "The Burn"
Cohesion: 0.67
Nodes (3): Adaptive Block Size, The Burn, HashCash

### Community 185 - ".readDataStream"
Cohesion: 0.38
Nodes (3): DataInputStream, Override, TokenTests

## Knowledge Gaps
- **107 isolated node(s):** `Scope: six files`, `The defect`, `Why it is reachable in ordinary use`, `Evidence`, `The change` (+102 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **135 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Command` connect `JSONObject` to `.getJSONReply`, `Contract`, `MiniData`, `MinimaDB.java`, `.runCommand`, `.runCommand`, `RamDB`, `MMRData`, `Coin`, `MegaMMR`, `backup`, `MegaMMRIBD`, `.runCommand`, `MessageProcessor`, `SendPollManager`, `.runCommand`, `txncoinlock`, `.getDB`, `restore`, `TxPoWList`, `MiniString`, `txnaddamount`, `hash`, `printtree`, `status`, `txnlock`, `tutorial`, `connect`, `network`, `mmrproof`, `benchmark`, `jniminingtest`, `jnitest`, `logs`, `maths`, `quit`, `test`, `timemilli`, `disconnect`, `message`, `p2pstate`, `sendsign`, `webhooks`, `reset`, `balance`, `coincheck`, `coinexport`, `coinimport`, `coinnotify`, `cointrack`, `consolidate`, `convert`, `getaddress`, `hashtest`, `incentivecash`, `random`, `seedrandom`, `slavenode`, `sphincs`, `tokencreate`, `tokenvalidate`, `trace`, `peers`, `ping`, `rpc`, `newscript`, `removescript`, `runscript`, `scripts`, `coins`, `history`, `keys`, `tokens`, `txpow`, `multisig`, `multisigread`, `send`, `sendnosign`, `sendpost`, `sendview`, `rawfrom`, `rawtxnfrom`, `sign`, `verify`, `tutorial`, `txnbasics`, `txncheck`, `txnclear`, `txncreate`, `txndelete`, `txnexport`, `txnimport`, `txninput`, `txnlist`, `txnoutput`, `txnpost`, `txnscript`, `txnsign`, `txnstate`, `txnview`, `whitepaper`, `automine`, `createfrom`, `postfrom`, `checkaddress`, `debugflag`, `healthcheck`, `.runCommand`, `missingcmd`, `newaddress`, `printmmr`, `nodecount`, `constructfrom`, `createtokenfrom`, `sendfrom`, `signfrom`, `txnmine`, `txnminepost`, `txnmmr`, `archive`, `block`, `scanchain`, `txnauto`?**
  _High betweenness centrality (0.164) - this node is a cross-community bridge._
- **Why does `JSONObject` connect `JSONObject` to `.getTxPoW`, `.getJSONReply`, `Contract`, `MiniData`, `ScriptProof`, `MinimaDB.java`, `.isEqual`, `.runCommand`, `.run`, `P2PState`, `.runCommand`, `Message`, `.log`, `RamDB`, `.getInstance`, `tutorial`, `TreeKey`, `MMRData`, `MMREntryNumber`, `java.net.InetSocketAddress`, `SeedRow`, `Coin`, `whitepaper`, `java.io.DataInputStream`, `.convertMinimaAddress`, `MMRProofChunk`, `automine`, `.getKnownPeers`, `TxPoW`, `.JSONPretty`, `NIOClientInfo`, `.testConstructors`, `MMR`, `.runCommand`, `.runCommand`, `.toString`, `.runCommand`, `missingcmd`, `MessageProcessor`, `SendPollManager`, `.runCommand`, `.signData`, `.getDB`, `JSONStreamAware`, `MiniString`, `txnlock`, `tutorial`, `JsonDB`?**
  _High betweenness centrality (0.159) - this node is a cross-community bridge._
- **Why does `MiniData` connect `MiniData` to `org.junit.Test`, `Expression`, `.getTxPoW`, `.getJSONReply`, `Contract`, `ScriptProof`, `MinimaDB.java`, `JSONObject`, `.isEqual`, `.run`, `UserDB`, `P2PState`, `send`, `.runCommand`, `.getHexParam`, `Message`, `MinimaDB`, `.log`, `java.io.DataOutputStream`, `.getInstance`, `GenerateKey`, `TreeKey`, `MMRData`, `HORSTSignature`, `java.net.InetSocketAddress`, `.getBytes`, `Coin`, `.getKnownPeers`, `java.io.DataInputStream`, `.convertMinimaAddress`, `MMRProofChunk`, `NIOClientInfo`, `TxPoW`, `TxBlock`, `.testConstructors`, `.main`, `.readDataStream`, `.runCommand`, `.toString`, `.convertMiniDataVersion`, `.getDB`, `TxPoWList`, `MiniString`, `Pulse`, `.ReadFromStream`, `JsonDB`, `mmrproof`, `Maths`, `.convertMiniDataVersion`, `.readDataStream`?**
  _High betweenness centrality (0.121) - this node is a cross-community bridge._
- **What connects `Scope: six files`, `The defect`, `Why it is reachable in ordinary use` to the rest of the system?**
  _107 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `org.junit.Test` be split into smaller, more focused modules?**
  _Cohesion score 0.04121618191969951 - nodes in this community are weakly interconnected._
- **Should `Expression` be split into smaller, more focused modules?**
  _Cohesion score 0.054945054945054944 - nodes in this community are weakly interconnected._
- **Should `.getTxPoW` be split into smaller, more focused modules?**
  _Cohesion score 0.06842105263157895 - nodes in this community are weakly interconnected._