# Graph Report - minima-core  (2026-08-28)

## Corpus Check
- 462 files · ~251,491 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 4540 nodes · 20570 edges · 204 communities (79 shown, 125 thin omitted)
- Extraction: 77% EXTRACTED · 23% INFERRED · 0% AMBIGUOUS · INFERRED: 4732 edges (avg confidence: 0.8)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `c081533d`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- org.junit.Test
- Expression
- NIOClientInfo
- TxPowTree
- .getDB
- Contract
- MiniData
- Coin
- Streamable
- JSONObject
- .getTxPoWID
- .checkExactParamNumber
- .runCommand
- .run
- .parseTokens
- UserDB
- .generateTxPoW
- ParamKeys
- .runCommand
- .getHexParam
- Message
- MiniData.java
- .log
- .writeDataStream
- .runFunction
- RamDB
- .runCommand
- GenerateKey
- .sign
- MMREntryNumber
- FORSSignature
- java.net.InetSocketAddress
- HTTPServer
- .getAllParameters
- .ReadFromStream
- .getParameter
- MiniNumberTests
- .getNumber
- Wallet
- .runCommand
- .getKnownPeers
- java.io.DataInputStream
- Cascade
- MMRProofChunk
- P2PState
- backup
- TxPoW
- Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write
- TxBlock
- .parse
- Main
- GlobalExpression
- MMR
- IBD
- .runCommand
- .WriteToStream
- .runCommand
- SPHINCS Signature Verification KISS VM Script
- .getBytes
- .toString
- MessageProcessor
- .runFunction
- .runFunction
- SendPollManager
- .getTxPoW
- txncoinlock
- Yylex
- TimerProcessor
- .getInstance
- .loadObjectSlow
- ParamConfigurer
- .MinimaDB
- ItemList
- .runCommand
- .runFunction
- TxPoWList
- MiniString
- Pulse
- MinimaUncaughtException
- .runCommand
- hash
- printtree
- status
- txnlock
- tutorial
- decryptbackup
- mysqlcoins
- Tx-PoW
- NIOTraffic
- RPCClient
- mmrproof
- MinimaException
- benchmark
- jniminingtest
- .calculateTXPOWID
- logs
- .runCommand
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
- ContainerFactory
- .readDataStream
- consolidatefrom
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
- JSONAware
- tokencreate
- tokenvalidate
- trace
- peers
- Greeting
- rpc
- newscript
- removescript
- runscript
- scripts
- coins
- .runCommand
- tokens
- txpow
- multisig
- multisigread
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
- .to0xString
- Stack
- automine
- gradlew
- Storage-less MMR UTXO Database
- Minima
- createfrom
- postfrom
- checkaddress
- .newFile
- The Burn
- buildjars.sh
- debugflag
- healthcheck
- missingcmd
- newaddress
- printmmr
- nodecount
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
- .runCommand
- scanchain
- txnauto

## God Nodes (most connected - your core abstractions)
1. `MiniData` - 449 edges
2. `JSONObject` - 409 edges
3. `Command` - 383 edges
4. `MiniNumber` - 333 edges
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

## Communities (204 total, 125 thin omitted)

### Community 0 - "org.junit.Test"
Cohesion: 0.04
Nodes (46): org.junit.Test, HexValue, Override, NumberValue, StringValue, TxPoWProcessorTest, ConstantExpressionTests, FunctionExpressionTests (+38 more)

### Community 1 - "Expression"
Cohesion: 0.04
Nodes (28): Expression, FunctionExpression, Override, Override, OperatorExpression, ASSERTstatement, Override, EXECstatement (+20 more)

### Community 2 - "NIOClientInfo"
Cohesion: 0.07
Nodes (9): NIOClientInfo, Level, DEBUG, INFO, NODE_RUNNER_MSG, P2PFunctions, Override, UtilFuncs (+1 more)

### Community 3 - "TxPowTree"
Cohesion: 0.10
Nodes (4): Override, TxPowTree, TxPoWTreeNodeAction, TxPowTreeTest

### Community 4 - ".getDB"
Cohesion: 0.07
Nodes (4): DataInputStream, Authorizer, Override, NotifyManager

### Community 5 - "Contract"
Cohesion: 0.11
Nodes (10): Contract, ExecutionException, MinimaParseException, ConstantExpression, VariableExpression, MinimaFunction, BooleanValue, Value (+2 more)

### Community 6 - "MiniData"
Cohesion: 0.06
Nodes (16): java.math.MathContext, java.security.KeyPair, MiniData, MiniNumber, MMRData, MMRProof, DataOutputStream, Token (+8 more)

### Community 7 - "Coin"
Cohesion: 0.07
Nodes (21): Override, VERIFYOUT, Address, Override, Coin, ScriptProof, Override, sphincs (+13 more)

### Community 8 - "Streamable"
Cohesion: 0.11
Nodes (11): java.io.DataOutputStream, java.sql.PreparedStatement, java.sql.Statement, MiniByte, GenesisTxPoW, NIOMessage, GeneralParams, Override (+3 more)

### Community 9 - "JSONObject"
Cohesion: 0.08
Nodes (13): java.util.concurrent.locks.ReentrantReadWriteLock, MinimaDB, TxPoWTreeNode, TxnDB, TxnRow, StateVariable, TxPoWGenerator, TxPoWSearcher (+5 more)

### Community 11 - ".checkExactParamNumber"
Cohesion: 0.04
Nodes (34): BITCOUNT, Override, BITGET, Override, ABS, Override, FLOOR, Override (+26 more)

### Community 12 - ".runCommand"
Cohesion: 0.11
Nodes (4): RawArchiveInput, Override, mysql, MySQLConnect

### Community 13 - ".run"
Cohesion: 0.04
Nodes (12): java.nio.ByteBuffer, java.nio.channels.SelectionKey, java.nio.channels.Selector, java.nio.channels.SocketChannel, Override, NIOClient, Override, NIOManager (+4 more)

### Community 14 - ".parseTokens"
Cohesion: 0.09
Nodes (10): ExpressionParser, StatementParser, LexicalTokenizer, Override, ScriptToken, ScriptTokenizer, ExpressionParserTests, StatementParserTests (+2 more)

### Community 15 - "UserDB"
Cohesion: 0.04
Nodes (4): UserDB, Override, magic, JsonDB

### Community 17 - "ParamKeys"
Cohesion: 0.03
Nodes (68): ParamKeys, allowallip, allowgenmessage, anyseed, archive, basefolder, clean, conf (+60 more)

### Community 19 - ".getHexParam"
Cohesion: 0.07
Nodes (14): Override, OVERWRITE, Override, SETLEN, Override, SUBSET, Override, PROOF (+6 more)

### Community 20 - "Message"
Cohesion: 0.05
Nodes (15): Override, Override, TxPoWProcessor, connect, Override, Override, Level, DEBUG (+7 more)

### Community 21 - "MiniData.java"
Cohesion: 0.18
Nodes (6): java.io.BufferedInputStream, java.io.FileInputStream, java.text.SimpleDateFormat, java.util.zip.GZIPInputStream, MiniFile, MiniUtil

### Community 22 - ".log"
Cohesion: 0.06
Nodes (6): Override, TxPoWOnChainDB, TxPoWSqlDB, CoinDB, Override, SqlDB

### Community 23 - ".writeDataStream"
Cohesion: 0.07
Nodes (16): Override, DataInputStream, DataOutputStream, Override, Override, Override, Override, Override (+8 more)

### Community 24 - ".runFunction"
Cohesion: 0.09
Nodes (12): ASCII, Override, Override, UTF8, Override, LEN, Override, REV (+4 more)

### Community 25 - "RamDB"
Cohesion: 0.07
Nodes (6): RamData, RamDB, Override, mempool, RamDBTest, TestUtils

### Community 26 - ".runCommand"
Cohesion: 0.14
Nodes (3): DataInputStream, Override, megammrsync

### Community 27 - "GenerateKey"
Cohesion: 0.09
Nodes (11): java.security.PrivateKey, java.security.PublicKey, javax.crypto.Cipher, javax.crypto.SecretKey, CryptoPackage, DataInputStream, Override, EncryptDecrypt (+3 more)

### Community 28 - ".sign"
Cohesion: 0.13
Nodes (5): org.bouncycastle.pqc.crypto.gmss.util.WinternitzOTSignature, SignatureProof, TreeKeyNode, Winternitz, WinternitzOTSignature

### Community 30 - "MMREntryNumber"
Cohesion: 0.13
Nodes (3): MMREntry, Override, MMREntryNumber

### Community 31 - "FORSSignature"
Cohesion: 0.09
Nodes (11): Override, FORS, FORSSignature, DataInputStream, Override, HORST, HORSTSignature, DataInputStream (+3 more)

### Community 32 - "java.net.InetSocketAddress"
Cohesion: 0.04
Nodes (20): java.net.InetSocketAddress, junit.framework.TestCase, org.junit.Before, InetSocketAddressIO, InetSocketAddress, InetSocketAddress, P2PDoSwap, Override (+12 more)

### Community 33 - "HTTPServer"
Cohesion: 0.13
Nodes (9): java.net.ServerSocket, java.net.Socket, ServerSocket, CMDHandler, HTTPServer, Override, ArchiveHandler, ArchiveServer (+1 more)

### Community 34 - ".getAllParameters"
Cohesion: 0.07
Nodes (12): Override, EXISTS, Override, GET, Override, CONCAT, Override, Override (+4 more)

### Community 35 - ".ReadFromStream"
Cohesion: 0.18
Nodes (3): Override, Override, MiniDataTests

### Community 36 - ".getParameter"
Cohesion: 0.13
Nodes (8): BOOL, Override, Override, NUMBER, Override, STRING, BITSET, Override

### Community 38 - ".getNumber"
Cohesion: 0.08
Nodes (12): BooleanExpression, Override, Override, CEIL, Override, DEC, Override, Override (+4 more)

### Community 39 - "Wallet"
Cohesion: 0.11
Nodes (10): java.sql.ResultSet, TxPoWDB, ScriptRow, SeedRow, Wallet, CoinProof, Signature, TreeKey (+2 more)

### Community 40 - ".runCommand"
Cohesion: 0.12
Nodes (6): Override, MegaMMR, Override, megammr, Override, MegaMMRBackup

### Community 41 - ".getKnownPeers"
Cohesion: 0.13
Nodes (6): InetSocketAddress, InetSocketAddress, QuickInetLists, QuickState, UtilFuncsTest, WalkLinksFuncsTest

### Community 43 - "Cascade"
Cohesion: 0.09
Nodes (6): Cascade, DataInputStream, DataOutputStream, Override, CascadeNode, Override

### Community 44 - "MMRProofChunk"
Cohesion: 0.11
Nodes (5): Override, Override, Override, MMRProofChunk, MiniByteTests

### Community 45 - "P2PState"
Cohesion: 0.07
Nodes (7): P2PWalkLinks, InetSocketAddress, Override, P2PManager, P2PState, SwapLinksFunctions, WalkLinksFuncs

### Community 47 - "TxPoW"
Cohesion: 0.08
Nodes (5): TxBody, DataInputStream, DataOutputStream, TxPoW, TimedChecker

### Community 48 - "Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write"
Cohesion: 0.10
Nodes (19): 2a. An `OutOfMemoryError` is an `Error`, not an `Exception` — `1770d8a`, 2b. Stream the IBD in batches — `a9ef09d`, 2c. Heap exhaustion is process-global — `49feed4`, 2d. The MMR tree is the real ceiling — `33c438d`, Also in this commit — `sendsign` burns a leaf per input coin, Branches, Evidence, minima-core — what we changed in our fork, and why (+11 more)

### Community 49 - "TxBlock"
Cohesion: 0.13
Nodes (4): ArchiveManager, TxBlockDB, DataInputStream, TxBlock

### Community 52 - "GlobalExpression"
Cohesion: 0.29
Nodes (3): GlobalExpression, Override, GlobalExpressionTests

### Community 53 - "MMR"
Cohesion: 0.11
Nodes (4): DataInputStream, DataOutputStream, MMR, GenesisMMR

### Community 55 - ".runCommand"
Cohesion: 0.11
Nodes (4): SPHINCS, DataInputStream, SPHINCSSignature, SPHINCSUtils

### Community 56 - ".WriteToStream"
Cohesion: 0.11
Nodes (7): Override, Override, Override, Override, MegaMMRIBD, Override, MegaMMRSyncData

### Community 58 - "SPHINCS Signature Verification KISS VM Script"
Cohesion: 0.10
Nodes (21): Bitcoin (Satoshi Nakamoto), Complete Node, Ethereum, Immutable / Ossified Protocol, Merkle Signature Scheme, Minima Protocol, Quantum Secure Cryptography (KECCAK + WOTS), Minima Scripting Language (Smart Contracts, Tokens) (+13 more)

### Community 59 - ".getBytes"
Cohesion: 0.09
Nodes (8): GZIPInputStream, DataInputStream, DataInputStream, DataInputStream, DataInputStream, Override, restoresync, PasswordCrypto

### Community 60 - ".toString"
Cohesion: 0.15
Nodes (4): Override, JSONValue, Override, JSONWriter

### Community 61 - "MessageProcessor"
Cohesion: 0.05
Nodes (8): Override, systemcheck, NetworkManager, Override, P2P2Manager, Server, MessageProcessor, MessageStack

### Community 62 - ".runFunction"
Cohesion: 0.18
Nodes (6): HEX, Override, Override, POW, Override, SIGDIG

### Community 63 - ".runFunction"
Cohesion: 0.08
Nodes (12): ADDRESS, Override, FUNCTION, Override, Override, REPLACE, Override, REPLACEFIRST (+4 more)

### Community 64 - "SendPollManager"
Cohesion: 0.14
Nodes (5): Override, sendpoll, Override, SendPollManager, SendPollMessage

### Community 65 - ".getTxPoW"
Cohesion: 0.07
Nodes (3): MathContext, burn, Override

### Community 69 - ".getInstance"
Cohesion: 0.05
Nodes (11): DataInputStream, KeyRow, Override, DataInputStream, DataOutputStream, Override, vault, Override (+3 more)

### Community 70 - ".loadObjectSlow"
Cohesion: 0.24
Nodes (3): BufferedInputStream, FileInputStream, DataInputStream

### Community 71 - "ParamConfigurer"
Cohesion: 0.18
Nodes (4): org.junit.runner.RunWith, ParamConfigurer, UnknownArgumentException, ParamConfigurerTest

### Community 72 - ".MinimaDB"
Cohesion: 0.10
Nodes (3): ReentrantReadWriteLock, Override, P2P2DB

### Community 74 - ".runCommand"
Cohesion: 0.36
Nodes (3): Override, mmrcreate, mmrleafnode

### Community 75 - ".runFunction"
Cohesion: 0.20
Nodes (4): Override, SAMESTATE, Override, STATE

### Community 76 - "TxPoWList"
Cohesion: 0.19
Nodes (5): DataInputStream, Override, TxPoWList, Override, restore

### Community 77 - "MiniString"
Cohesion: 0.09
Nodes (7): java.nio.charset.Charset, java.sql.Connection, javax.net.ssl.SSLContext, MiniString, JSONParser, ParseException, MiniFormat

### Community 79 - "MinimaUncaughtException"
Cohesion: 0.33
Nodes (3): java.lang.Thread.UncaughtExceptionHandler, Override, MinimaUncaughtException

### Community 88 - "Tx-PoW"
Cohesion: 0.25
Nodes (8): Cascading Proof Chain, Distributed PoW, GHOST Consensus, IOTA Tangle, Maxima P2P Information Network, P2Pool, Pulse Network, Tx-PoW

### Community 89 - "NIOTraffic"
Cohesion: 0.14
Nodes (3): Override, network, NIOTraffic

### Community 91 - "mmrproof"
Cohesion: 0.36
Nodes (3): Override, mmrleafnode, mmrproof

### Community 95 - ".calculateTXPOWID"
Cohesion: 0.13
Nodes (4): DataInputStream, Override, jnitest, jnifunctions

### Community 108 - ".toString"
Cohesion: 0.10
Nodes (6): Override, Override, Override, Override, Override, MiniStringTests

### Community 132 - "Greeting"
Cohesion: 0.17
Nodes (3): Greeting, Override, ping

### Community 170 - "minima-core"
Cohesion: 0.40
Nodes (4): Build, MegaMMR node, minima-core, Testing on private network

### Community 171 - ".to0xString"
Cohesion: 0.08
Nodes (5): DataOutputStream, Override, DataInputStream, DataInputStream, BaseConverter

### Community 174 - "gradlew"
Cohesion: 0.60
Nodes (3): gradlew script, die(), warn()

### Community 175 - "Storage-less MMR UTXO Database"
Cohesion: 0.40
Nodes (5): Storage-less MMR UTXO Database, Peter Todd MMR (Delayed TXO Commitments), MegaMMR Node Mode (-megammr), PROOF Merkle Proof Function, HORST Tree Proof Verification Loop (Step 2)

### Community 181 - "The Burn"
Cohesion: 0.67
Nodes (3): Adaptive Block Size, The Burn, HashCash

## Knowledge Gaps
- **107 isolated node(s):** `buildjars.sh script`, `NODE_RUNNER_MSG`, `INFO`, `DEBUG`, `NODE_RUNNER_MSG` (+102 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **125 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Command` connect `JSONObject` to `.getDB`, `Contract`, `MiniData`, `Coin`, `Streamable`, `.runCommand`, `UserDB`, `Message`, `MiniData.java`, `RamDB`, `.runCommand`, `Wallet`, `.runCommand`, `backup`, `.runCommand`, `.getBytes`, `MessageProcessor`, `SendPollManager`, `.getTxPoW`, `txncoinlock`, `.getInstance`, `.runCommand`, `TxPoWList`, `MiniString`, `.runCommand`, `hash`, `printtree`, `status`, `txnlock`, `tutorial`, `decryptbackup`, `mysqlcoins`, `NIOTraffic`, `mmrproof`, `benchmark`, `jniminingtest`, `.calculateTXPOWID`, `logs`, `.runCommand`, `quit`, `test`, `timemilli`, `disconnect`, `message`, `p2pstate`, `sendsign`, `webhooks`, `consolidatefrom`, `reset`, `balance`, `coincheck`, `coinexport`, `coinimport`, `coinnotify`, `cointrack`, `consolidate`, `convert`, `getaddress`, `hashtest`, `incentivecash`, `random`, `seedrandom`, `slavenode`, `tokencreate`, `tokenvalidate`, `trace`, `peers`, `Greeting`, `rpc`, `newscript`, `removescript`, `runscript`, `scripts`, `coins`, `.runCommand`, `tokens`, `txpow`, `multisig`, `multisigread`, `sendnosign`, `sendpost`, `sendview`, `rawfrom`, `rawtxnfrom`, `sign`, `verify`, `tutorial`, `txnbasics`, `txncheck`, `txnclear`, `txncreate`, `txndelete`, `txnexport`, `txnimport`, `txninput`, `txnlist`, `txnoutput`, `txnpost`, `txnscript`, `txnsign`, `txnstate`, `txnview`, `whitepaper`, `automine`, `createfrom`, `postfrom`, `checkaddress`, `debugflag`, `healthcheck`, `missingcmd`, `newaddress`, `printmmr`, `nodecount`, `constructfrom`, `createtokenfrom`, `sendfrom`, `signfrom`, `txnmine`, `txnminepost`, `txnmmr`, `archive`, `.runCommand`, `scanchain`, `txnauto`?**
  _High betweenness centrality (0.178) - this node is a cross-community bridge._
- **Why does `MiniData` connect `MiniData` to `org.junit.Test`, `Expression`, `TxPowTree`, `.getDB`, `Contract`, `Greeting`, `Coin`, `Streamable`, `JSONObject`, `.getTxPoWID`, `.run`, `UserDB`, `.generateTxPoW`, `.runCommand`, `.getHexParam`, `MiniData.java`, `.log`, `.writeDataStream`, `.runCommand`, `GenerateKey`, `.sign`, `.CreateMMRDataLeafNode`, `FORSSignature`, `java.net.InetSocketAddress`, `.ReadFromStream`, `Wallet`, `Cascade`, `.to0xString`, `P2PState`, `TxPoW`, `TxBlock`, `Main`, `.runCommand`, `.WriteToStream`, `.getBytes`, `.toString`, `.getTxPoW`, `.getInstance`, `TxPoWList`, `MiniString`, `Pulse`, `mmrproof`, `.calculateTXPOWID`, `Maths`?**
  _High betweenness centrality (0.138) - this node is a cross-community bridge._
- **Why does `JSONObject` connect `JSONObject` to `Expression`, `NIOClientInfo`, `Greeting`, `Contract`, `MiniData`, `Coin`, `Streamable`, `.getDB`, `.getTxPoWID`, `.runCommand`, `.runCommand`, `.run`, `UserDB`, `.runCommand`, `Message`, `MiniData.java`, `.log`, `RamDB`, `.runCommand`, `tutorial`, `.sign`, `MMREntryNumber`, `java.net.InetSocketAddress`, `Wallet`, `.runCommand`, `whitepaper`, `.to0xString`, `MMRProofChunk`, `automine`, `P2PState`, `Main`, `MMR`, `.runCommand`, `.runCommand`, `.getBytes`, `.toString`, `MessageProcessor`, `missingcmd`, `SendPollManager`, `.getTxPoW`, `.getInstance`, `.runCommand`, `JSONStreamAware`, `MiniString`, `.runCommand`, `.runCommand`, `txnlock`, `tutorial`, `NIOTraffic`, `.calculateTXPOWID`, `.runCommand`, `quit`, `test`, `disconnect`, `message`, `p2pstate`, `.toString`, `JSONAware`?**
  _High betweenness centrality (0.137) - this node is a cross-community bridge._
- **What connects `buildjars.sh script`, `NODE_RUNNER_MSG`, `INFO` to the rest of the system?**
  _107 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `org.junit.Test` be split into smaller, more focused modules?**
  _Cohesion score 0.041469976143342975 - nodes in this community are weakly interconnected._
- **Should `Expression` be split into smaller, more focused modules?**
  _Cohesion score 0.036344755970924195 - nodes in this community are weakly interconnected._
- **Should `NIOClientInfo` be split into smaller, more focused modules?**
  _Cohesion score 0.06845513413506013 - nodes in this community are weakly interconnected._