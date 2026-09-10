# Graph Report - minima-core  (2026-09-07)

## Corpus Check
- 530 files · ~341,911 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 7460 nodes · 28954 edges · 258 communities (119 shown, 139 thin omitted)
- Extraction: 79% EXTRACTED · 21% INFERRED · 0% AMBIGUOUS · INFERRED: 6196 edges (avg confidence: 0.76)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `9dc9d613`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- org.junit.Test
- .testConstructors
- P2PFunctions
- .getTxPoW
- .getDB
- Contract
- MiniData
- ScriptProof
- MinimaDB.java
- JSONObject
- txncheck
- MinimaFunction
- index-c2615a0a.js
- NIOClient
- Expression
- UserDB
- index-fdc1a9df.js
- ParamKeys
- .runCommand
- .then
- Message
- UserDB.java
- .log
- java.io.DataOutputStream
- vw
- RamDB
- .runCommand
- AesUtil
- TreeKey
- mmrcreate
- MMR
- HORSTSignature
- java.net.InetSocketAddress
- HTTPServer
- .runFunction
- index-legacy-f6200fe1.js
- t
- i
- NumberValue
- Coin
- MegaMMR
- P2PState
- .ReadFromStream
- Cascade
- At
- NIOClientInfo
- backup
- TxPoW
- Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write
- MinimaDB
- u5
- .stopMessageProcessor
- ot
- .getInstance
- .forEach
- org.mozilla.javascript.Scriptable
- .runCommand
- SPHINCS Signature Verification KISS VM Script
- GenerateKey
- .toString
- archive
- P2PWalkLinks
- .toString
- .runCommand
- txnaddamount
- txncoinlock
- .parse
- .Main
- .to0xString
- JsonDB
- ParamConfigurer
- .saveState
- ItemList
- magic
- .setBoolean
- decryptbackup
- MiniString
- MDSManager
- ql
- .processMessage
- hash
- printtree
- status
- txnlock
- tutorial
- b4
- mysqlcoins
- Tx-PoW
- NIOTraffic
- n
- mmrproof
- MinimaException
- benchmark
- jniminingtest
- jnifunctions
- logs
- maths
- quit
- xp
- timemilli
- disconnect
- message
- p2pstate
- sendsign
- Maths
- burn
- n
- qc
- e
- MinimaUncaughtException
- bg
- reset
- balance
- coincheck
- coinexport
- MDSCommandHandler.java
- coinnotify
- .writeDataStream
- consolidate
- convert
- getaddress
- hashtest
- incentivecash
- random
- seedrandom
- slavenode
- .toString
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
- jnitest
- tokens
- txpow
- multisig
- systemcheck
- .constructor
- sendnosign
- sendpost
- sendview
- rawfrom
- rawtxnfrom
- sign
- verify
- tutorial
- txnbasics
- du
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
- restore
- sphincs
- Stack
- automine
- gradlew
- Storage-less MMR UTXO Database
- Minima
- createfrom
- postfrom
- checkaddress
- MiniDAPP
- The Burn
- buildjars.sh
- debugflag
- connect
- healthcheck
- network
- missingcmd
- newaddress
- printmmr
- nodecount
- multisigread
- constructfrom
- createtokenfrom
- sendfrom
- signfrom
- txnmine
- txnminepost
- txnmmr
- toParamKey
- yo
- vp
- send
- .run
- block
- wu
- txnauto
- na
- t
- org.minima.utils.json.JSONObject
- org.minima.system.commands.Command
- .includes
- pb
- Xn
- .runCommand
- junit.framework.TestCase
- mg
- E
- .loadAllDB
- ms
- MMREntryNumber
- .runCommand
- P2PDB
- yp
- eo
- polyfills-legacy-9ffc4e0f.js
- mx
- .get
- MinimaTrustManager
- TxPoWTreeNodeAction
- .MinimaDB
- SeedRow
- mdsjs/mds.js
- mds_orig.js
- block/mds.js
- wallet/mds.js
- CryptoPackage
- SSLManager
- SelfSignedCertGenerator.java
- HTTPSServer
- .readDataStream
- .runFunction
- restoresync
- DUMMYFUNCTION
- .main
- .checkAuchCredentials
- coinimport
- cointrack
- M9
- MDSHubLogon
- ByteArray
- li
- MDSCommandException
- MDSInvalidIDException

## God Nodes (most connected - your core abstractions)
1. `MiniData` - 447 edges
2. `JSONObject` - 401 edges
3. `Command` - 380 edges
4. `MiniNumber` - 327 edges
5. `Contract` - 258 edges
6. `MinimaFunction` - 231 edges
7. `NumberValue` - 206 edges
8. `Value` - 197 edges
9. `TxPoW` - 194 edges
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

## Communities (258 total, 139 thin omitted)

### Community 0 - "org.junit.Test"
Cohesion: 0.04
Nodes (41): org.junit.Test, HexValue, Override, TxPoWProcessorTest, ExpressionParserTests, CONCATTests, GETTests, LENTests (+33 more)

### Community 1 - ".testConstructors"
Cohesion: 0.06
Nodes (7): GlobalExpression, Override, Override, Override, ContractTests, GlobalExpressionTests, BooleanValueTests

### Community 2 - "P2PFunctions"
Cohesion: 0.14
Nodes (5): Level, DEBUG, INFO, NODE_RUNNER_MSG, P2PFunctions

### Community 3 - ".getTxPoW"
Cohesion: 0.08
Nodes (3): TxPowTree, MathContext, TxPowTreeTest

### Community 4 - ".getDB"
Cohesion: 0.06
Nodes (9): GZIPInputStream, DataInputStream, DataInputStream, DataInputStream, DataInputStream, DataInputStream, JSONObject, JSONObject (+1 more)

### Community 5 - "Contract"
Cohesion: 0.10
Nodes (13): Contract, ExecutionException, MinimaParseException, ConstantExpression, VariableExpression, RETURNstatement, Statement, StatementBlock (+5 more)

### Community 6 - "MiniData"
Cohesion: 0.03
Nodes (34): java.io.DataInputStream, java.math.MathContext, java.security.KeyPair, DataInputStream, Override, TxPoWList, MiniData, MiniNumber (+26 more)

### Community 7 - "ScriptProof"
Cohesion: 0.05
Nodes (30): GETINADDR, Override, GETINAMT, Override, GETINTOK, Override, Override, SUMINPUTS (+22 more)

### Community 8 - "MinimaDB.java"
Cohesion: 0.04
Nodes (41): FileInputStream, java.io.BufferedInputStream, java.io.FileInputStream, java.nio.channels.SelectionKey, java.sql.PreparedStatement, java.sql.Statement, java.text.SimpleDateFormat, java.util.zip.GZIPInputStream (+33 more)

### Community 9 - "JSONObject"
Cohesion: 0.08
Nodes (14): TxnDB, TxnRow, Address, TxPoWGenerator, Override, mempool, Command, CommandException (+6 more)

### Community 11 - "MinimaFunction"
Cohesion: 0.02
Nodes (61): ASCII, Override, BOOL, Override, HEX, Override, Override, NUMBER (+53 more)

### Community 12 - "index-c2615a0a.js"
Cohesion: 0.01
Nodes (209): _0(), _3, _7, _8(), a0(), a4, a7, Aa() (+201 more)

### Community 13 - "NIOClient"
Cohesion: 0.04
Nodes (11): java.nio.ByteBuffer, java.nio.channels.Selector, java.nio.channels.SocketChannel, Override, NIOClient, Handler, NIOHandoff, Override (+3 more)

### Community 14 - "Expression"
Cohesion: 0.04
Nodes (29): Expression, ExpressionParser, FunctionExpression, Override, ASSERTstatement, Override, EXECstatement, Override (+21 more)

### Community 16 - "index-fdc1a9df.js"
Cohesion: 0.02
Nodes (136): $1(), $2(), a7(), Ad(), Af(), B5(), Bo(), Bt (+128 more)

### Community 17 - "ParamKeys"
Cohesion: 0.03
Nodes (69): ParamKeys, allowallip, allowgenmessage, anyseed, archive, basefolder, clean, conf (+61 more)

### Community 18 - ".runCommand"
Cohesion: 0.09
Nodes (5): Override, consolidatefrom, Override, txnutils, TransactionTests

### Community 19 - ".then"
Cohesion: 0.02
Nodes (34): $a(), An(), Ba(), Bc(), dm(), Ft, He, Io (+26 more)

### Community 20 - "Message"
Cohesion: 0.05
Nodes (8): Level, DEBUG, INFO, NODE_RUNNER_MSG, P2P2Functions, Message, MessageStack, SwapLinksFunctionsTest

### Community 22 - ".log"
Cohesion: 0.03
Nodes (19): NetworkManager, ArchiveManager, Override, Override, TxPoWOnChainDB, TxPoWSqlDB, CoinDB, Override (+11 more)

### Community 23 - "java.io.DataOutputStream"
Cohesion: 0.06
Nodes (17): java.io.DataOutputStream, Override, Override, Override, Override, Override, Override, Override (+9 more)

### Community 24 - "vw"
Cohesion: 0.03
Nodes (52): _9(), ag(), b9(), Ci(), Ct(), Ec(), eg(), ge() (+44 more)

### Community 25 - "RamDB"
Cohesion: 0.08
Nodes (4): RamData, RamDB, RamDBTest, TestUtils

### Community 27 - "AesUtil"
Cohesion: 0.33
Nodes (3): javax.crypto.Cipher, javax.crypto.SecretKey, AesUtil

### Community 28 - "TreeKey"
Cohesion: 0.07
Nodes (9): org.bouncycastle.pqc.crypto.gmss.util.WinternitzOTSignature, KeyRow, TreeKey, TreeKeyNode, Winternitz, Override, keys, SPHINCS (+1 more)

### Community 29 - "mmrcreate"
Cohesion: 0.36
Nodes (3): Override, mmrcreate, mmrleafnode

### Community 30 - "MMR"
Cohesion: 0.10
Nodes (6): DataInputStream, DataOutputStream, Override, MMR, MMREntry, GenesisMMR

### Community 31 - "HORSTSignature"
Cohesion: 0.10
Nodes (8): Override, FORS, HORST, HORSTSignature, DataInputStream, Override, PrivateKey, PublicKey

### Community 32 - "java.net.InetSocketAddress"
Cohesion: 0.06
Nodes (11): java.net.InetSocketAddress, InetSocketAddress, InetSocketAddress, P2PDoSwap, Override, P2PGreeting, P2PManager, P2PPeersChecker (+3 more)

### Community 33 - "HTTPServer"
Cohesion: 0.29
Nodes (4): java.net.ServerSocket, ServerSocket, HTTPServer, Override

### Community 34 - ".runFunction"
Cohesion: 0.06
Nodes (17): Override, EXISTS, Override, FUNCTION, Override, GET, Override, CONCAT (+9 more)

### Community 35 - "index-legacy-f6200fe1.js"
Cohesion: 0.02
Nodes (88): ar(), as(), ay(), Bm(), br(), bt(), cf(), co() (+80 more)

### Community 36 - "t"
Cohesion: 0.04
Nodes (103): _1(), $2(), _5(), a5(), a9, ak(), ap(), Au() (+95 more)

### Community 37 - "i"
Cohesion: 0.04
Nodes (98): F3(), Wn(), x2(), _4(), _a(), a3(), ai(), ao() (+90 more)

### Community 38 - "NumberValue"
Cohesion: 0.07
Nodes (16): BooleanExpression, Override, Override, Override, OperatorExpression, ABS, Override, CEIL (+8 more)

### Community 39 - "Coin"
Cohesion: 0.11
Nodes (12): TxPoWDB, TxPoWTreeNode, ScriptRow, Wallet, Coin, CoinProof, StateVariable, Token (+4 more)

### Community 40 - "MegaMMR"
Cohesion: 0.13
Nodes (6): Override, MegaMMR, Override, megammr, Override, MegaMMRBackup

### Community 41 - "P2PState"
Cohesion: 0.14
Nodes (4): InetSocketAddress, P2PState, InetSocketAddress, QuickState

### Community 42 - ".ReadFromStream"
Cohesion: 0.08
Nodes (11): Override, DataInputStream, DataOutputStream, Override, Override, Override, Override, Override (+3 more)

### Community 43 - "Cascade"
Cohesion: 0.16
Nodes (6): Cascade, DataInputStream, DataOutputStream, Override, CascadeNode, Override

### Community 44 - "At"
Cohesion: 0.04
Nodes (17): sv(), As(), At, av(), gv(), h4(), ht(), ih (+9 more)

### Community 45 - "NIOClientInfo"
Cohesion: 0.11
Nodes (6): NIOClientInfo, InetSocketAddress, Override, SwapLinksFunctions, UtilFuncs, QuickClients

### Community 47 - "TxPoW"
Cohesion: 0.04
Nodes (12): DataInputStream, DataOutputStream, Override, TxPoW, TimedChecker, TxPoWChecker, Override, Override (+4 more)

### Community 48 - "Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write"
Cohesion: 0.10
Nodes (19): 2a. An `OutOfMemoryError` is an `Error`, not an `Exception` — `1770d8a`, 2b. Stream the IBD in batches — `a9ef09d`, 2c. Heap exhaustion is process-global — `49feed4`, 2d. The MMR tree is the real ceiling — `33c438d`, Also in this commit — `sendsign` burns a leaf per input coin, Branches, Evidence, minima-core — what we changed in our fork, and why (+11 more)

### Community 49 - "MinimaDB"
Cohesion: 0.07
Nodes (13): java.util.concurrent.locks.ReentrantReadWriteLock, org.minima.database.archive.ArchiveManager, org.minima.database.archive.TxBlockDB, org.minima.database.cascade.Cascade, org.minima.database.txpowdb.TxPoWDB, org.minima.database.txpowtree.TxPowTree, org.minima.database.userprefs.txndb.TxnDB, org.minima.database.userprefs.UserDB (+5 more)

### Community 50 - "u5"
Cohesion: 0.04
Nodes (75): Ad(), ah(), b1(), c5(), ca(), cc(), cs(), _d() (+67 more)

### Community 52 - "ot"
Cohesion: 0.03
Nodes (11): dv(), e4(), Er(), gh(), hv(), mr(), ot(), pm() (+3 more)

### Community 54 - ".getInstance"
Cohesion: 0.05
Nodes (11): BufferedInputStream, DataInputStream, IBD, Override, Override, TxPoWProcessor, Override, MegaMMRIBD (+3 more)

### Community 55 - ".forEach"
Cohesion: 0.03
Nodes (27): a8(), af, bw(), D0(), d3(), f8(), G9(), gi() (+19 more)

### Community 56 - "org.mozilla.javascript.Scriptable"
Cohesion: 0.06
Nodes (23): org.mozilla.javascript.Context, org.mozilla.javascript.ContextFactory, org.mozilla.javascript.NativeJavaObject, org.mozilla.javascript.Scriptable, org.mozilla.javascript.WrapFactory, NETcommand, APICallback, APIService (+15 more)

### Community 58 - "SPHINCS Signature Verification KISS VM Script"
Cohesion: 0.10
Nodes (21): Bitcoin (Satoshi Nakamoto), Complete Node, Ethereum, Immutable / Ossified Protocol, Merkle Signature Scheme, Minima Protocol, Quantum Secure Cryptography (KECCAK + WOTS), Minima Scripting Language (Smart Contracts, Tokens) (+13 more)

### Community 59 - "GenerateKey"
Cohesion: 0.21
Nodes (3): EncryptDecrypt, GenerateKey, PasswordCrypto

### Community 60 - ".toString"
Cohesion: 0.11
Nodes (7): CMDcommand, Override, Override, JSONStreamAware, JSONValue, Override, JSONWriter

### Community 63 - ".toString"
Cohesion: 0.11
Nodes (8): ADDRESS, Override, Override, REPLACE, Override, REPLACEFIRST, Override, ScriptValueTests

### Community 64 - ".runCommand"
Cohesion: 0.16
Nodes (5): Override, sendpoll, Override, SendPollManager, SendPollMessage

### Community 67 - ".parse"
Cohesion: 0.06
Nodes (4): ContainerFactory, ContentHandler, Yylex, Yytoken

### Community 68 - ".Main"
Cohesion: 0.07
Nodes (8): NotifyManager, SendPollManager, Override, P2P2Manager, Override, TimerProcessor, TxPoWMiner, TxPoWProcessor

### Community 69 - ".to0xString"
Cohesion: 0.03
Nodes (9): Override, DataInputStream, DataOutputStream, DataInputStream, Override, vault, Override, test (+1 more)

### Community 71 - "ParamConfigurer"
Cohesion: 0.15
Nodes (5): MiniNumber, org.junit.runner.RunWith, ParamConfigurer, UnknownArgumentException, ParamConfigurerTest

### Community 77 - "MiniString"
Cohesion: 0.05
Nodes (12): java.nio.charset.Charset, java.sql.Connection, javax.net.ssl.SSLContext, Override, MiniString, Greeting, Override, JSONParser (+4 more)

### Community 78 - "MDSManager"
Cohesion: 0.04
Nodes (18): JsonDB, org.minima.system.network.rpc.Server, org.minima.utils.JsonDB, APICommand, KEYPAIRcommand, SQLcommand, MDSHub, MDSHubDelete (+10 more)

### Community 79 - "ql"
Cohesion: 0.05
Nodes (61): a1(), bh(), bi(), cl(), d7, E2(), ed(), es() (+53 more)

### Community 80 - ".processMessage"
Cohesion: 0.16
Nodes (4): Override, webhooks, Override, NotifyManager

### Community 86 - "b4"
Cohesion: 0.05
Nodes (17): a3(), Ar(), B2(), b4(), c3(), en(), fp, gg (+9 more)

### Community 88 - "Tx-PoW"
Cohesion: 0.25
Nodes (8): Cascading Proof Chain, Distributed PoW, GHOST Consensus, IOTA Tangle, Maxima P2P Information Network, P2Pool, Pulse Network, Tx-PoW

### Community 90 - "n"
Cohesion: 0.06
Nodes (57): a9(), ac(), Al(), ar(), b(), Bh(), bv(), c3() (+49 more)

### Community 91 - "mmrproof"
Cohesion: 0.36
Nodes (3): Override, mmrleafnode, mmrproof

### Community 99 - "xp"
Cohesion: 0.06
Nodes (60): Je(), Ks(), Ag(), b0(), bp(), _c(), cc(), Ci() (+52 more)

### Community 107 - "n"
Cohesion: 0.16
Nodes (60): bf(), a(), af(), aw(), b(), Bf(), bw(), c() (+52 more)

### Community 108 - "qc"
Cohesion: 0.07
Nodes (55): ic(), ir(), Or(), ys(), dy(), Dc(), ed(), Fc() (+47 more)

### Community 109 - "e"
Cohesion: 0.06
Nodes (34): bl(), _c(), ck(), cv(), Da(), Fa(), i9, Is() (+26 more)

### Community 110 - "MinimaUncaughtException"
Cohesion: 0.33
Nodes (3): java.lang.Thread.UncaughtExceptionHandler, Override, MinimaUncaughtException

### Community 111 - "bg"
Cohesion: 0.09
Nodes (44): g4, Ze(), a0(), ap(), bc(), bg(), ca(), ce() (+36 more)

### Community 116 - "MDSCommandHandler.java"
Cohesion: 0.09
Nodes (13): java.net.Socket, COMMSCommand, Override, MDSCompleteHandler, MEGAPOLLcommand, NOTIFYcommand, POLLcommand, MDSCommandHandler (+5 more)

### Community 127 - ".toString"
Cohesion: 0.07
Nodes (28): _0(), bs(), c6(), Ds, Et(), eu(), iu(), J6() (+20 more)

### Community 144 - "systemcheck"
Cohesion: 0.22
Nodes (3): Override, systemcheck, Override

### Community 145 - ".constructor"
Cohesion: 0.09
Nodes (32): ax(), Br(), constructor(), cx(), du(), dx(), fx(), Gn() (+24 more)

### Community 155 - "du"
Cohesion: 0.08
Nodes (39): qm(), Ai(), Ao(), Bi(), bl(), cd(), cu(), Da() (+31 more)

### Community 174 - "gradlew"
Cohesion: 0.60
Nodes (3): gradlew script, die(), warn()

### Community 175 - "Storage-less MMR UTXO Database"
Cohesion: 0.40
Nodes (5): Storage-less MMR UTXO Database, Peter Todd MMR (Delayed TXO Commitments), MegaMMR Node Mode (-megammr), PROOF Merkle Proof Function, HORST Tree Proof Verification Loop (Step 2)

### Community 180 - "MiniDAPP"
Cohesion: 0.08
Nodes (13): java.util.zip.ZipEntry, org.minima.utils.messages.Message, org.minima.utils.messages.MessageProcessor, JSONObject, MiniDAPP, JSONObject, MDSHubInstall, MDSHubUpdate (+5 more)

### Community 181 - "The Burn"
Cohesion: 0.67
Nodes (3): Adaptive Block Size, The Burn, HashCash

### Community 203 - "yo"
Cohesion: 0.08
Nodes (37): Aa(), Ba(), bo(), cn(), dn(), Et(), Fa(), fn() (+29 more)

### Community 204 - "vp"
Cohesion: 0.09
Nodes (36): An(), au(), cd(), cu(), dd(), Df(), Dr(), Dt() (+28 more)

### Community 206 - ".run"
Cohesion: 0.09
Nodes (6): DataOutputStream, Override, MDSFileHandler, MultipartData, MultipartParser, MDSPublicPage

### Community 208 - "wu"
Cohesion: 0.11
Nodes (35): ri(), El(), ni(), Nl(), au(), Bc(), bu(), ci() (+27 more)

### Community 210 - "na"
Cohesion: 0.08
Nodes (33): jm(), D1(), Di(), ec(), f0(), fs(), Gf(), H0() (+25 more)

### Community 211 - "t"
Cohesion: 0.09
Nodes (23): F2(), ch(), Cr(), fy(), _h(), I2(), i6(), K8() (+15 more)

### Community 212 - "org.minima.utils.json.JSONObject"
Cohesion: 0.11
Nodes (8): org.minima.utils.json.JSONObject, JSONObject, Message, JSONObject, JSONObject, PendingCommand, JSONObject, PollMessage

### Community 213 - "org.minima.system.commands.Command"
Cohesion: 0.12
Nodes (10): org.minima.system.commands.Command, checkmode, Override, checkpending, Override, checkrestore, JSONObject, Override (+2 more)

### Community 214 - ".includes"
Cohesion: 0.18
Nodes (23): A6(), bk(), D6(), fk(), gk(), jl(), jt(), K (+15 more)

### Community 215 - "pb"
Cohesion: 0.13
Nodes (25): Bv(), cx(), dg(), dv(), fv(), gb(), Gg(), Gv() (+17 more)

### Community 216 - "Xn"
Cohesion: 0.11
Nodes (15): _4(), av(), Bu(), c8(), c9(), hn(), ig, mp() (+7 more)

### Community 217 - ".runCommand"
Cohesion: 0.12
Nodes (4): JSONArray, JSONObject, APIAutoResponse, Override

### Community 218 - "junit.framework.TestCase"
Cohesion: 0.13
Nodes (6): junit.framework.TestCase, InetSocketAddressIOTest, InetSocketAddress, QuickInetLists, UtilFuncsTest, WalkLinksFuncsTest

### Community 219 - "mg"
Cohesion: 0.14
Nodes (7): h4, j4(), l8(), Mc(), mg(), q4(), yl()

### Community 220 - "E"
Cohesion: 0.10
Nodes (21): Qh(), av(), bd(), E(), ha(), ie(), Kh(), le() (+13 more)

### Community 221 - ".loadAllDB"
Cohesion: 0.20
Nodes (4): ArchiveManager, TxnDB, TxPoWDB, Wallet

### Community 222 - "ms"
Cohesion: 0.13
Nodes (20): bs(), cs(), ds(), es(), fs(), hs(), is(), Jl() (+12 more)

### Community 225 - "P2PDB"
Cohesion: 0.17
Nodes (4): org.junit.Before, P2PDB, InetSocketAddress, P2PDBTest

### Community 226 - "yp"
Cohesion: 0.18
Nodes (18): pp(), T2(), Nf(), bp(), ep(), Fp(), Gf(), gp() (+10 more)

### Community 227 - "eo"
Cohesion: 0.14
Nodes (18): aa(), d0(), eo(), g0(), j1(), jt(), K0(), ns() (+10 more)

### Community 228 - "polyfills-legacy-9ffc4e0f.js"
Cohesion: 0.30
Nodes (14): a(), c(), d(), h(), i(), l(), n(), o() (+6 more)

### Community 229 - "mx"
Cohesion: 0.15
Nodes (3): addChildren(), mx, Os()

### Community 230 - ".get"
Cohesion: 0.16
Nodes (12): _3(), br(), D8(), dn(), ed(), eh(), Hr(), n1() (+4 more)

### Community 231 - "MinimaTrustManager"
Cohesion: 0.31
Nodes (7): java.security.cert.X509Certificate, javax.net.ssl.TrustManager, javax.net.ssl.X509TrustManager, org.minima.objects.base.MiniData, MiniData, Override, MinimaTrustManager

### Community 233 - ".MinimaDB"
Cohesion: 0.20
Nodes (8): Cascade, MegaMMR, P2P2DB, P2PDB, ReentrantReadWriteLock, TxBlockDB, TxPowTree, UserDB

### Community 235 - "mdsjs/mds.js"
Cohesion: 0.27
Nodes (5): httpPostAsync(), httpPostAsyncPoll(), MDSPostMessage(), PollListener(), postMDSFail()

### Community 236 - "mds_orig.js"
Cohesion: 0.27
Nodes (5): httpPostAsync(), httpPostAsyncPoll(), MDSPostMessage(), PollListener(), postMDSFail()

### Community 237 - "block/mds.js"
Cohesion: 0.27
Nodes (5): httpPostAsync(), httpPostAsyncPoll(), MDSPostMessage(), PollListener(), postMDSFail()

### Community 238 - "wallet/mds.js"
Cohesion: 0.27
Nodes (5): httpPostAsync(), httpPostAsyncPoll(), MDSPostMessage(), PollListener(), postMDSFail()

### Community 239 - "CryptoPackage"
Cohesion: 0.29
Nodes (3): CryptoPackage, DataInputStream, Override

### Community 242 - "SelfSignedCertGenerator.java"
Cohesion: 0.39
Nodes (4): java.security.PublicKey, org.bouncycastle.asn1.x509.AuthorityKeyIdentifier, org.bouncycastle.asn1.x509.SubjectKeyIdentifier, SelfSignedCertGenerator

### Community 243 - "HTTPSServer"
Cohesion: 0.31
Nodes (4): javax.net.ssl.SSLServerSocket, Server, HTTPSServer, Override

## Knowledge Gaps
- **191 isolated node(s):** `lu`, `sx`, `yx`, `t7`, `n7` (+186 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **139 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Command` connect `JSONObject` to `.getDB`, `Contract`, `MiniData`, `MinimaDB.java`, `txncheck`, `.runCommand`, `.log`, `TreeKey`, `mmrcreate`, `Coin`, `MegaMMR`, `backup`, `TxPoW`, `.getInstance`, `.runCommand`, `archive`, `.runCommand`, `txnaddamount`, `txncoinlock`, `.to0xString`, `magic`, `decryptbackup`, `MiniString`, `.processMessage`, `hash`, `printtree`, `status`, `txnlock`, `tutorial`, `mysqlcoins`, `mmrproof`, `benchmark`, `jniminingtest`, `logs`, `maths`, `quit`, `timemilli`, `disconnect`, `message`, `p2pstate`, `sendsign`, `burn`, `reset`, `balance`, `coincheck`, `coinexport`, `coinnotify`, `consolidate`, `convert`, `getaddress`, `hashtest`, `incentivecash`, `random`, `seedrandom`, `slavenode`, `tokencreate`, `tokenvalidate`, `trace`, `peers`, `ping`, `rpc`, `newscript`, `removescript`, `runscript`, `scripts`, `coins`, `history`, `jnitest`, `tokens`, `txpow`, `multisig`, `systemcheck`, `sendnosign`, `sendpost`, `sendview`, `rawfrom`, `rawtxnfrom`, `sign`, `verify`, `tutorial`, `txnbasics`, `txnclear`, `txncreate`, `txndelete`, `txnexport`, `txnimport`, `txninput`, `txnlist`, `txnoutput`, `txnpost`, `txnscript`, `txnsign`, `txnstate`, `txnview`, `whitepaper`, `restore`, `sphincs`, `automine`, `createfrom`, `postfrom`, `checkaddress`, `debugflag`, `connect`, `healthcheck`, `network`, `missingcmd`, `newaddress`, `printmmr`, `nodecount`, `multisigread`, `constructfrom`, `createtokenfrom`, `sendfrom`, `signfrom`, `txnmine`, `txnminepost`, `txnmmr`, `send`, `block`, `txnauto`, `.runCommand`, `restoresync`, `coinimport`, `cointrack`?**
  _High betweenness centrality (0.059) - this node is a cross-community bridge._
- **Why does `MiniData` connect `MiniData` to `org.junit.Test`, `.testConstructors`, `.getTxPoW`, `.getDB`, `Contract`, `ScriptProof`, `MinimaDB.java`, `JSONObject`, `MinimaFunction`, `NIOClient`, `Expression`, `UserDB`, `.runCommand`, `Message`, `UserDB.java`, `.log`, `java.io.DataOutputStream`, `RamDB`, `.runCommand`, `AesUtil`, `TreeKey`, `MMR`, `HORSTSignature`, `java.net.InetSocketAddress`, `NumberValue`, `Coin`, `P2PState`, `Cascade`, `TxPoW`, `GenerateKey`, `P2PWalkLinks`, `.to0xString`, `JsonDB`, `MiniString`, `send`, `mmrproof`, `Maths`, `SeedRow`, `CryptoPackage`, `.CreateMMRDataLeafNode`?**
  _High betweenness centrality (0.055) - this node is a cross-community bridge._
- **Why does `JSONObject` connect `JSONObject` to `.testConstructors`, `P2PFunctions`, `.getTxPoW`, `.getDB`, `Contract`, `MiniData`, `ScriptProof`, `MinimaDB.java`, `NIOClient`, `systemcheck`, `.runCommand`, `Message`, `.log`, `tutorial`, `.runCommand`, `TreeKey`, `MMR`, `java.net.InetSocketAddress`, `Coin`, `whitepaper`, `P2PState`, `automine`, `NIOClientInfo`, `TxPoW`, `MiniDAPP`, `.getInstance`, `.runCommand`, `.toString`, `missingcmd`, `P2PWalkLinks`, `printmmr`, `.runCommand`, `txncoinlock`, `.Main`, `.to0xString`, `JsonDB`, `MiniString`, `.processMessage`, `txnlock`, `tutorial`, `NIOTraffic`, `junit.framework.TestCase`, `P2PDB`, `SeedRow`, `.checkAuchCredentials`?**
  _High betweenness centrality (0.054) - this node is a cross-community bridge._
- **What connects `lu`, `sx`, `yx` to the rest of the system?**
  _191 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `org.junit.Test` be split into smaller, more focused modules?**
  _Cohesion score 0.038352652101917635 - nodes in this community are weakly interconnected._
- **Should `.testConstructors` be split into smaller, more focused modules?**
  _Cohesion score 0.0613107822410148 - nodes in this community are weakly interconnected._
- **Should `P2PFunctions` be split into smaller, more focused modules?**
  _Cohesion score 0.14285714285714285 - nodes in this community are weakly interconnected._