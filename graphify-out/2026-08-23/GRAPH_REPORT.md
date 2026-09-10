# Graph Report - minima-core  (2026-08-11)

## Corpus Check
- 462 files · ~251,094 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 4538 nodes · 20713 edges · 205 communities (71 shown, 134 thin omitted)
- Extraction: 77% EXTRACTED · 23% INFERRED · 0% AMBIGUOUS · INFERRED: 4716 edges (avg confidence: 0.8)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `ec862a74`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- org.junit.Test
- Contract
- JSONArray
- TxPowTree
- .getJSONReply
- MinimaFunction
- MiniData
- ScriptProof
- Main
- JSONObject
- .calculateCoins
- .checkExactParamNumber
- .log
- NIOClient
- .parseTokens
- UserDB
- P2PState
- ParamKeys
- .runCommand
- .getHexParam
- Message
- .loadAllDB
- .checkOpen
- java.io.DataOutputStream
- .runFunction
- TxPoWDB
- .runCommand
- GenerateKey
- .sign
- MMRData
- MMR
- HORSTSignature
- java.net.InetSocketAddress
- .run
- .runFunction
- Wallet
- MiniNumberTests
- .getNumber
- Coin
- .runCommand
- .getKnownPeers
- java.io.DataInputStream
- Cascade
- .isEqual
- P2PWalkLinks
- backup
- .run
- Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write
- TxBlock
- .parse
- P2P2Functions
- NIOTraffic
- NIOClientInfo
- .main
- MegaMMRIBD
- .runCommand
- SPHINCS Signature Verification KISS VM Script
- systemcheck
- .toString
- TimerProcessor
- .runFunction
- .runFunction
- .PostTimerMessage
- .getDB
- txncoinlock
- Yylex
- P2PDB
- .isEqual
- .getBytes
- ParamConfigurer
- ABS
- ItemList
- .readDataStream
- Greeting
- decryptbackup
- MiniString
- Pulse
- restoresync
- txnaddamount
- hash
- printtree
- status
- txnlock
- tutorial
- connect
- MinimaUncaughtException
- Tx-PoW
- network
- mmrcreate
- mmrproof
- MinimaException
- benchmark
- jniminingtest
- jnitest
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
- sendpoll
- .runCommand
- ContainerFactory
- mysqlcoins
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
- User Instruction Rule 0 (Follow Explicit Instructions)
- healthcheck
- magic
- missingcmd
- newaddress
- printmmr
- nodecount
- consolidatefrom
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

## God Nodes (most connected - your core abstractions)
1. `MiniData` - 449 edges
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

## Communities (205 total, 134 thin omitted)

### Community 0 - "org.junit.Test"
Cohesion: 0.04
Nodes (42): org.junit.Test, NumberValue, TxPoWProcessorTest, OperatorExpressionTests, CONCATTests, GETTests, LENTests, REVTests (+34 more)

### Community 1 - "Contract"
Cohesion: 0.04
Nodes (32): Contract, Expression, FunctionExpression, Override, GlobalExpression, Override, Override, OperatorExpression (+24 more)

### Community 2 - "JSONArray"
Cohesion: 0.11
Nodes (5): StateVariable, CommandRunner, JSONArray, JSONAware, MiniUtil

### Community 3 - "TxPowTree"
Cohesion: 0.07
Nodes (6): Override, TxPowTree, TxPoWTreeNodeAction, Override, TxPoWProcessor, TxPowTreeTest

### Community 4 - ".getJSONReply"
Cohesion: 0.06
Nodes (4): Override, txnauto, Authorizer, Override

### Community 5 - "MinimaFunction"
Cohesion: 0.12
Nodes (11): ExecutionException, MinimaParseException, ConstantExpression, MinimaFunction, VERIFYIN, BooleanValue, HexValue, StringValue (+3 more)

### Community 6 - "MiniData"
Cohesion: 0.04
Nodes (17): java.math.MathContext, MiniData, MathContext, MiniNumber, Magic, TxBody, TxHeader, TxPoW (+9 more)

### Community 7 - "ScriptProof"
Cohesion: 0.11
Nodes (13): Override, VERIFYOUT, Address, ScriptProof, GETINADDRTests, GETINAMTTests, GETINIDTests, GETINTOKTests (+5 more)

### Community 8 - "Main"
Cohesion: 0.06
Nodes (24): GZIPInputStream, java.io.BufferedInputStream, java.io.FileInputStream, java.nio.channels.SelectionKey, java.sql.PreparedStatement, java.sql.Statement, java.text.SimpleDateFormat, java.util.zip.GZIPInputStream (+16 more)

### Community 9 - "JSONObject"
Cohesion: 0.10
Nodes (8): java.util.concurrent.locks.ReentrantReadWriteLock, ReentrantReadWriteLock, MinimaDB, TxnDB, TxnRow, Command, CommandException, JSONObject

### Community 11 - ".checkExactParamNumber"
Cohesion: 0.03
Nodes (34): BOOL, Override, Override, OVERWRITE, CEIL, Override, DEC, Override (+26 more)

### Community 12 - ".log"
Cohesion: 0.05
Nodes (6): Override, Override, mysql, Override, ArchiveServer, MySQLConnect

### Community 13 - "NIOClient"
Cohesion: 0.04
Nodes (9): java.nio.ByteBuffer, java.nio.channels.Selector, java.nio.channels.SocketChannel, Override, NIOClient, Override, Override, NIOServer (+1 more)

### Community 14 - ".parseTokens"
Cohesion: 0.09
Nodes (10): ExpressionParser, StatementParser, LexicalTokenizer, Override, ScriptToken, ScriptTokenizer, ExpressionParserTests, StatementParserTests (+2 more)

### Community 15 - "UserDB"
Cohesion: 0.04
Nodes (4): UserDB, Override, P2P2DB, JsonDB

### Community 16 - "P2PState"
Cohesion: 0.12
Nodes (6): InetSocketAddress, Override, P2PManager, InetSocketAddress, P2PState, SwapLinksFunctions

### Community 17 - "ParamKeys"
Cohesion: 0.03
Nodes (68): ParamKeys, allowallip, allowgenmessage, anyseed, archive, basefolder, clean, conf (+60 more)

### Community 19 - ".getHexParam"
Cohesion: 0.06
Nodes (18): BITCOUNT, Override, BITGET, Override, BITSET, Override, Override, SETLEN (+10 more)

### Community 20 - "Message"
Cohesion: 0.06
Nodes (10): Level, DEBUG, INFO, NODE_RUNNER_MSG, P2PFunctions, Override, Override, Override (+2 more)

### Community 22 - ".checkOpen"
Cohesion: 0.07
Nodes (4): Override, TxPoWOnChainDB, TxPoWSqlDB, SqlDB

### Community 23 - "java.io.DataOutputStream"
Cohesion: 0.04
Nodes (35): java.io.DataOutputStream, Override, Override, Override, Override, DataOutputStream, DataOutputStream, Override (+27 more)

### Community 24 - ".runFunction"
Cohesion: 0.06
Nodes (16): ASCII, Override, Override, UTF8, Override, LEN, Override, REV (+8 more)

### Community 25 - "TxPoWDB"
Cohesion: 0.07
Nodes (7): RamData, RamDB, TxPoWDB, Override, mempool, RamDBTest, TestUtils

### Community 26 - ".runCommand"
Cohesion: 0.09
Nodes (3): Override, SPHINCSUtils, TransactionTests

### Community 27 - "GenerateKey"
Cohesion: 0.08
Nodes (13): java.security.KeyPair, java.security.PrivateKey, java.security.PublicKey, javax.crypto.Cipher, javax.crypto.SecretKey, CryptoPackage, DataInputStream, Override (+5 more)

### Community 28 - ".sign"
Cohesion: 0.20
Nodes (4): org.bouncycastle.pqc.crypto.gmss.util.WinternitzOTSignature, TreeKeyNode, Winternitz, WinternitzOTSignature

### Community 29 - "MMRData"
Cohesion: 0.09
Nodes (6): SignatureProof, MMRData, DataInputStream, Override, MMRProof, MMRProofChunk

### Community 30 - "MMR"
Cohesion: 0.07
Nodes (8): DataInputStream, DataOutputStream, Override, MMR, Override, MMREntry, Override, MMREntryNumber

### Community 31 - "HORSTSignature"
Cohesion: 0.13
Nodes (7): Override, HORST, HORSTSignature, DataInputStream, Override, PrivateKey, PublicKey

### Community 32 - "java.net.InetSocketAddress"
Cohesion: 0.06
Nodes (15): java.net.InetSocketAddress, junit.framework.TestCase, InetSocketAddressIO, InetSocketAddress, InetSocketAddress, P2PDoSwap, Override, P2PGreeting (+7 more)

### Community 33 - ".run"
Cohesion: 0.13
Nodes (9): java.net.ServerSocket, java.net.Socket, ServerSocket, CMDHandler, HTTPServer, Override, Server, ArchiveHandler (+1 more)

### Community 34 - ".runFunction"
Cohesion: 0.07
Nodes (12): EXISTS, Override, GET, Override, CONCAT, Override, Override, MAX (+4 more)

### Community 36 - "Wallet"
Cohesion: 0.12
Nodes (3): java.sql.ResultSet, SeedRow, Wallet

### Community 38 - ".getNumber"
Cohesion: 0.06
Nodes (13): BooleanExpression, Override, Override, Override, SQRT, Override, Override, Override (+5 more)

### Community 39 - "Coin"
Cohesion: 0.16
Nodes (13): TxPoWTreeNode, ScriptRow, Coin, CoinProof, Signature, TreeKey, DataOutputStream, Token (+5 more)

### Community 40 - ".runCommand"
Cohesion: 0.09
Nodes (9): BufferedInputStream, FileInputStream, Override, Override, MegaMMR, Override, megammr, Override (+1 more)

### Community 41 - ".getKnownPeers"
Cohesion: 0.11
Nodes (5): SwapLinksFunctionsTest, InetSocketAddress, QuickInetLists, QuickState, WalkLinksFuncsTest

### Community 42 - "java.io.DataInputStream"
Cohesion: 0.06
Nodes (14): java.io.DataInputStream, DataInputStream, Override, TxPoWList, DataInputStream, DataInputStream, DataInputStream, DataInputStream (+6 more)

### Community 43 - "Cascade"
Cohesion: 0.09
Nodes (8): DataInputStream, RawArchiveInput, Cascade, DataInputStream, DataOutputStream, Override, CascadeNode, IBD

### Community 47 - ".run"
Cohesion: 0.08
Nodes (4): TimedChecker, block, Override, Override

### Community 48 - "Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write"
Cohesion: 0.12
Nodes (16): 2a. An `OutOfMemoryError` is an `Error`, not an `Exception` — `1770d8a`, 2b. Stream the IBD in batches — `a9ef09d`, 2c. Heap exhaustion is process-global — `49feed4`, 2d. The MMR tree is the real ceiling — `33c438d`, Also in this commit — `sendsign` burns a leaf per input coin, Branches, Evidence, minima-core — what we changed in our fork, and why (+8 more)

### Community 49 - "TxBlock"
Cohesion: 0.09
Nodes (6): ArchiveManager, TxBlockDB, DataInputStream, TxBlock, archive, Override

### Community 51 - "P2P2Functions"
Cohesion: 0.15
Nodes (5): Level, DEBUG, INFO, NODE_RUNNER_MSG, P2P2Functions

### Community 54 - "NIOClientInfo"
Cohesion: 0.11
Nodes (3): NIOClientInfo, UtilFuncs, QuickClients

### Community 55 - ".main"
Cohesion: 0.11
Nodes (9): FORS, FORSSignature, DataInputStream, Override, SPHINCS, DataInputStream, Override, SPHINCSSignature (+1 more)

### Community 58 - "SPHINCS Signature Verification KISS VM Script"
Cohesion: 0.10
Nodes (21): Bitcoin (Satoshi Nakamoto), Complete Node, Ethereum, Immutable / Ossified Protocol, Merkle Signature Scheme, Minima Protocol, Quantum Secure Cryptography (KECCAK + WOTS), Minima Scripting Language (Smart Contracts, Tokens) (+13 more)

### Community 60 - ".toString"
Cohesion: 0.16
Nodes (4): Override, JSONValue, Override, JSONWriter

### Community 62 - ".runFunction"
Cohesion: 0.18
Nodes (6): HEX, Override, Override, POW, Override, SIGDIG

### Community 63 - ".runFunction"
Cohesion: 0.07
Nodes (14): Override, NUMBER, Override, STRING, ADDRESS, Override, FUNCTION, Override (+6 more)

### Community 64 - ".PostTimerMessage"
Cohesion: 0.11
Nodes (5): Override, SendPollManager, SendPollMessage, Override, P2P2Manager

### Community 65 - ".getDB"
Cohesion: 0.06
Nodes (5): burn, Override, Override, scanchain, NotifyManager

### Community 68 - "P2PDB"
Cohesion: 0.19
Nodes (4): org.junit.Before, P2PDB, InetSocketAddress, P2PDBTest

### Community 69 - ".isEqual"
Cohesion: 0.07
Nodes (3): KeyRow, Override, vault

### Community 70 - ".getBytes"
Cohesion: 0.16
Nodes (3): DataInputStream, Override, restore

### Community 71 - "ParamConfigurer"
Cohesion: 0.18
Nodes (4): org.junit.runner.RunWith, ParamConfigurer, UnknownArgumentException, ParamConfigurerTest

### Community 74 - ".readDataStream"
Cohesion: 0.38
Nodes (3): DataInputStream, Override, AddressTests

### Community 77 - "MiniString"
Cohesion: 0.08
Nodes (8): java.nio.charset.Charset, java.sql.Connection, javax.net.ssl.SSLContext, MiniString, JSONParser, ParseException, MinimaRPCClient, RPCClient

### Community 87 - "MinimaUncaughtException"
Cohesion: 0.33
Nodes (3): java.lang.Thread.UncaughtExceptionHandler, Override, MinimaUncaughtException

### Community 88 - "Tx-PoW"
Cohesion: 0.25
Nodes (8): Cascading Proof Chain, Distributed PoW, GHOST Consensus, IOTA Tangle, Maxima P2P Information Network, P2Pool, Pulse Network, Tx-PoW

### Community 90 - "mmrcreate"
Cohesion: 0.36
Nodes (3): Override, mmrcreate, mmrleafnode

### Community 91 - "mmrproof"
Cohesion: 0.36
Nodes (3): Override, mmrleafnode, mmrproof

### Community 170 - "minima-core"
Cohesion: 0.40
Nodes (4): Build, MegaMMR node, minima-core, Testing on private network

### Community 171 - ".to0xString"
Cohesion: 0.07
Nodes (4): Override, BaseConverter, jnifunctions, TxHeaderTests

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
- **106 isolated node(s):** `buildjars.sh script`, `NODE_RUNNER_MSG`, `INFO`, `DEBUG`, `NODE_RUNNER_MSG` (+101 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **134 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Command` connect `JSONObject` to `JSONArray`, `.getJSONReply`, `MinimaFunction`, `MiniData`, `Main`, `.log`, `TxPoWDB`, `Coin`, `.runCommand`, `backup`, `.run`, `TxBlock`, `.runCommand`, `systemcheck`, `.getDB`, `txncoinlock`, `.isEqual`, `.getBytes`, `decryptbackup`, `MiniString`, `restoresync`, `txnaddamount`, `hash`, `printtree`, `status`, `txnlock`, `tutorial`, `connect`, `network`, `mmrcreate`, `mmrproof`, `benchmark`, `jniminingtest`, `jnitest`, `logs`, `.runCommand`, `quit`, `test`, `timemilli`, `disconnect`, `message`, `p2pstate`, `sendsign`, `webhooks`, `sendpoll`, `.runCommand`, `mysqlcoins`, `reset`, `balance`, `coincheck`, `coinexport`, `coinimport`, `coinnotify`, `cointrack`, `consolidate`, `convert`, `getaddress`, `hashtest`, `incentivecash`, `random`, `seedrandom`, `slavenode`, `sphincs`, `tokencreate`, `tokenvalidate`, `trace`, `peers`, `ping`, `rpc`, `newscript`, `removescript`, `runscript`, `scripts`, `coins`, `history`, `keys`, `tokens`, `txpow`, `multisig`, `multisigread`, `send`, `sendnosign`, `sendpost`, `sendview`, `rawfrom`, `rawtxnfrom`, `sign`, `verify`, `tutorial`, `txnbasics`, `txncheck`, `txnclear`, `txncreate`, `txndelete`, `txnexport`, `txnimport`, `txninput`, `txnlist`, `txnoutput`, `txnpost`, `txnscript`, `txnsign`, `txnstate`, `txnview`, `whitepaper`, `automine`, `createfrom`, `postfrom`, `checkaddress`, `debugflag`, `healthcheck`, `magic`, `missingcmd`, `newaddress`, `printmmr`, `nodecount`, `consolidatefrom`, `constructfrom`, `createtokenfrom`, `sendfrom`, `signfrom`, `txnmine`, `txnminepost`, `txnmmr`?**
  _High betweenness centrality (0.178) - this node is a cross-community bridge._
- **Why does `JSONObject` connect `JSONObject` to `Contract`, `JSONArray`, `.getJSONReply`, `MinimaFunction`, `MiniData`, `ScriptProof`, `Main`, `.calculateCoins`, `.log`, `NIOClient`, `UserDB`, `P2PState`, `.runCommand`, `Message`, `.checkOpen`, `TxPoWDB`, `.runCommand`, `tutorial`, `MMRData`, `MMR`, `java.net.InetSocketAddress`, `Wallet`, `Coin`, `.runCommand`, `whitepaper`, `java.io.DataInputStream`, `.to0xString`, `.isEqual`, `automine`, `.getKnownPeers`, `.run`, `P2PWalkLinks`, `TxBlock`, `NIOTraffic`, `NIOClientInfo`, `.main`, `.runCommand`, `.toString`, `missingcmd`, `.PostTimerMessage`, `.getDB`, `txncoinlock`, `P2PDB`, `.isEqual`, `.getBytes`, `Greeting`, `JSONStreamAware`, `MiniString`, `txnlock`, `tutorial`, `.runCommand`, `test`, `.runCommand`?**
  _High betweenness centrality (0.136) - this node is a cross-community bridge._
- **Why does `MiniData` connect `MiniData` to `Contract`, `JSONArray`, `TxPowTree`, `.getJSONReply`, `MinimaFunction`, `ScriptProof`, `Main`, `JSONObject`, `.calculateCoins`, `NIOClient`, `UserDB`, `P2PState`, `send`, `.runCommand`, `.loadAllDB`, `.checkOpen`, `java.io.DataOutputStream`, `.runFunction`, `.runCommand`, `GenerateKey`, `.sign`, `MMRData`, `HORSTSignature`, `java.net.InetSocketAddress`, `.getDataValue`, `Wallet`, `Coin`, `.getKnownPeers`, `java.io.DataInputStream`, `Cascade`, `.to0xString`, `P2PWalkLinks`, `.run`, `TxBlock`, `.main`, `MegaMMRIBD`, `.toString`, `.getDB`, `.isEqual`, `.getBytes`, `Greeting`, `MiniString`, `Pulse`, `mmrproof`, `Maths`, `.runCommand`?**
  _High betweenness centrality (0.136) - this node is a cross-community bridge._
- **What connects `buildjars.sh script`, `NODE_RUNNER_MSG`, `INFO` to the rest of the system?**
  _106 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `org.junit.Test` be split into smaller, more focused modules?**
  _Cohesion score 0.04121663563004345 - nodes in this community are weakly interconnected._
- **Should `Contract` be split into smaller, more focused modules?**
  _Cohesion score 0.037569772434521254 - nodes in this community are weakly interconnected._
- **Should `JSONArray` be split into smaller, more focused modules?**
  _Cohesion score 0.10510510510510511 - nodes in this community are weakly interconnected._