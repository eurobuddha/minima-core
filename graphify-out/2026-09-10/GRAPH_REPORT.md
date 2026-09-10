# Graph Report - minima-core  (2026-09-07)

## Corpus Check
- 530 files · ~341,911 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 7473 nodes · 28921 edges · 235 communities (116 shown, 119 thin omitted)
- Extraction: 79% EXTRACTED · 21% INFERRED · 0% AMBIGUOUS · INFERRED: 6196 edges (avg confidence: 0.76)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `8ea56557`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- org.junit.Test
- .testConstructors
- NIOClientInfo
- .getTxPoWID
- .getDB
- Contract
- MiniData
- .runFunction
- Main
- JSONObject
- txncheck
- .getHexParam
- index-c2615a0a.js
- NIOClient
- Expression
- UserDB
- index-fdc1a9df.js
- ParamKeys
- .runCommand
- .then
- .run
- MiniNumber
- .checkOpen
- java.io.DataInputStream
- vw
- CascadeNode
- .runCommand
- AesUtil
- TreeKey
- mmrcreate
- MMR
- q
- java.net.InetSocketAddress
- HTTPServer
- .getAllParameters
- index-legacy-f6200fe1.js
- t
- t
- NumberValue
- Coin
- MDSJS
- P2PState
- ArchiveManager
- .runCommand
- At
- .runFunction
- backup
- TxPoW
- Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write
- .forEach
- u5
- Eo
- ot
- yr
- du
- k9
- org.mozilla.javascript.Scriptable
- .runCommand
- SPHINCS Signature Verification KISS VM Script
- .getBytes
- .toJSONString
- archive
- Message
- .toString
- .runCommand
- txnaddamount
- txncoinlock
- .parse
- .log
- .to0xString
- ServiceJSRunner
- ParamConfigurer
- .saveState
- ItemList
- magic
- Ln
- .runFunction
- RPCClient
- MDSManager
- ql
- .processMessage
- .runFunction
- printtree
- status
- txnlock
- tutorial
- b4
- mysqlcoins
- Tx-PoW
- STRING
- n
- mmrproof
- MinimaException
- benchmark
- jniminingtest
- mysql
- logs
- maths
- quit
- it
- timemilli
- disconnect
- message
- p2pstate
- sendsign
- Maths
- test
- n
- qc
- r9
- w8
- bg
- reset
- balance
- coincheck
- coinexport
- java.net.Socket
- coinnotify
- JSONAware
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
- Message
- rpc
- newscript
- removescript
- runscript
- scripts
- coins
- MiniData
- .getMiniDataVersion
- tokens
- txpow
- multisig
- MessageProcessor
- sendnosign
- .runCommand
- sendview
- rawfrom
- rawtxnfrom
- sign
- verify
- tutorial
- txnbasics
- E
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
- .main
- Stack
- automine
- gradlew
- Storage-less MMR UTXO Database
- createfrom
- postfrom
- checkaddress
- MiniDAPP
- The Burn
- buildjars.sh
- debugflag
- healthcheck
- network
- missingcmd
- newaddress
- printmmr
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
- .run
- wu
- txnauto
- na
- org.minima.utils.json.JSONObject
- org.minima.system.commands.Command
- .includes
- pb
- Xn
- org.minima.system.mds.MDSManager
- e
- .loadAllDB
- ms
- .runCommand
- yp
- eo
- polyfills-legacy-9ffc4e0f.js
- mx
- java.security.cert.X509Certificate
- mdsjs/mds.js
- mds_orig.js
- block/mds.js
- wallet/mds.js
- SSLManager
- SelfSignedCertGenerator.java
- HTTPSServer.java
- DUMMYFUNCTION
- EncryptDecrypt
- coinimport
- cointrack
- M9
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

## Communities (235 total, 119 thin omitted)

### Community 0 - "org.junit.Test"
Cohesion: 0.04
Nodes (49): org.junit.Test, HexValue, Override, ScriptProof, TxPoWProcessorTest, ExpressionParserTests, CONCATTests, GETTests (+41 more)

### Community 1 - ".testConstructors"
Cohesion: 0.03
Nodes (27): FunctionExpression, Override, GlobalExpression, Override, Override, FUNCTION, Override, ASSERTstatement (+19 more)

### Community 2 - "NIOClientInfo"
Cohesion: 0.07
Nodes (10): Override, NIOClientInfo, Level, DEBUG, INFO, NODE_RUNNER_MSG, P2PFunctions, Override (+2 more)

### Community 3 - ".getTxPoWID"
Cohesion: 0.05
Nodes (8): Override, TxPoWTreeNodeAction, TimedChecker, Override, TxPoWProcessor, block, Override, TxPowTreeTest

### Community 4 - ".getDB"
Cohesion: 0.08
Nodes (3): DataInputStream, JSONObject, JSONObject

### Community 5 - "Contract"
Cohesion: 0.12
Nodes (10): Contract, ExecutionException, MinimaParseException, ConstantExpression, MinimaFunction, BooleanValue, StringValue, Value (+2 more)

### Community 6 - "MiniData"
Cohesion: 0.04
Nodes (27): TxPowTree, MiniData, Override, SignatureProof, MMRData, DataInputStream, MMRProof, TxBody (+19 more)

### Community 7 - ".runFunction"
Cohesion: 0.05
Nodes (20): GETINADDR, Override, GETINID, Override, GETINTOK, Override, Override, SUMINPUTS (+12 more)

### Community 8 - "Main"
Cohesion: 0.04
Nodes (36): FileInputStream, java.io.BufferedInputStream, java.io.FileInputStream, java.nio.charset.Charset, java.sql.Connection, java.sql.PreparedStatement, java.sql.Statement, java.text.SimpleDateFormat (+28 more)

### Community 9 - "JSONObject"
Cohesion: 0.06
Nodes (31): java.util.concurrent.locks.ReentrantReadWriteLock, org.minima.database.archive.ArchiveManager, org.minima.database.archive.TxBlockDB, org.minima.database.cascade.Cascade, org.minima.database.txpowdb.TxPoWDB, org.minima.database.txpowtree.TxPowTree, org.minima.database.userprefs.txndb.TxnDB, org.minima.database.userprefs.UserDB (+23 more)

### Community 11 - ".getHexParam"
Cohesion: 0.04
Nodes (26): ASCII, Override, Override, UTF8, BITCOUNT, Override, BITGET, Override (+18 more)

### Community 12 - "index-c2615a0a.js"
Cohesion: 0.01
Nodes (230): _0(), _3, _7, _8(), a0(), a4, a7, Aa() (+222 more)

### Community 13 - "NIOClient"
Cohesion: 0.04
Nodes (13): java.nio.ByteBuffer, java.nio.channels.SelectionKey, java.nio.channels.Selector, java.nio.channels.SocketChannel, Override, NIOClient, Override, NIOManager (+5 more)

### Community 14 - "Expression"
Cohesion: 0.06
Nodes (23): BooleanExpression, Expression, ExpressionParser, Override, OperatorExpression, VariableExpression, EXECstatement, IFstatement (+15 more)

### Community 16 - "index-fdc1a9df.js"
Cohesion: 0.02
Nodes (141): $2(), a7(), Af(), B5(), Bo(), Bt, by(), C4 (+133 more)

### Community 17 - "ParamKeys"
Cohesion: 0.03
Nodes (69): ParamKeys, allowallip, allowgenmessage, anyseed, archive, basefolder, clean, conf (+61 more)

### Community 18 - ".runCommand"
Cohesion: 0.07
Nodes (7): Override, burn, Override, consolidatefrom, Override, txnutils, MiniNumberTests

### Community 19 - ".then"
Cohesion: 0.03
Nodes (30): a9, An(), Bc(), D0(), Ft, gi(), go(), He (+22 more)

### Community 20 - ".run"
Cohesion: 0.04
Nodes (14): connect, Override, Override, nodecount, Override, ping, Handler, NIOHandoff (+6 more)

### Community 21 - "MiniNumber"
Cohesion: 0.04
Nodes (12): java.math.MathContext, TxBlockDB, MathContext, MiniNumber, Greeting, Override, Magic, DataOutputStream (+4 more)

### Community 22 - ".checkOpen"
Cohesion: 0.06
Nodes (5): Override, TxPoWOnChainDB, CoinDB, Override, SqlDB

### Community 23 - "java.io.DataInputStream"
Cohesion: 0.02
Nodes (52): java.io.DataInputStream, java.io.DataOutputStream, DataInputStream, Cascade, DataInputStream, DataOutputStream, Override, Override (+44 more)

### Community 24 - "vw"
Cohesion: 0.03
Nodes (50): _9(), af, ag(), b9(), Ct(), cw(), Ec(), Ff() (+42 more)

### Community 25 - "CascadeNode"
Cohesion: 0.06
Nodes (8): CascadeNode, Override, RamData, RamDB, Override, mempool, RamDBTest, TestUtils

### Community 26 - ".runCommand"
Cohesion: 0.08
Nodes (6): BufferedInputStream, DataInputStream, Override, megammr, Override, megammrsync

### Community 27 - "AesUtil"
Cohesion: 0.33
Nodes (3): javax.crypto.Cipher, javax.crypto.SecretKey, AesUtil

### Community 28 - "TreeKey"
Cohesion: 0.04
Nodes (14): java.sql.ResultSet, org.bouncycastle.pqc.crypto.gmss.util.WinternitzOTSignature, KeyRow, SeedRow, Override, TreeKey, TreeKeyNode, Winternitz (+6 more)

### Community 29 - "mmrcreate"
Cohesion: 0.36
Nodes (3): Override, mmrcreate, mmrleafnode

### Community 30 - "MMR"
Cohesion: 0.06
Nodes (8): MegaMMR, DataInputStream, DataOutputStream, MMR, MMREntry, Override, MMREntryNumber, GenesisMMR

### Community 31 - "q"
Cohesion: 0.05
Nodes (12): $a(), Ba(), dm(), $m(), N6(), nr, od(), P6() (+4 more)

### Community 32 - "java.net.InetSocketAddress"
Cohesion: 0.04
Nodes (23): java.net.InetSocketAddress, junit.framework.TestCase, org.junit.Before, InetSocketAddressIO, InetSocketAddress, InetSocketAddress, P2PDoSwap, Override (+15 more)

### Community 33 - "HTTPServer"
Cohesion: 0.29
Nodes (4): java.net.ServerSocket, ServerSocket, HTTPServer, Override

### Community 34 - ".getAllParameters"
Cohesion: 0.06
Nodes (14): EXISTS, Override, GET, Override, CONCAT, Override, Override, MAX (+6 more)

### Community 35 - "index-legacy-f6200fe1.js"
Cohesion: 0.02
Nodes (99): Aa(), ar(), as(), ay(), bd(), Bm(), br(), bt() (+91 more)

### Community 36 - "t"
Cohesion: 0.03
Nodes (119): _1(), $2(), _5(), a8(), ak(), ap(), av(), ax() (+111 more)

### Community 37 - "t"
Cohesion: 0.06
Nodes (68): F2(), Wn(), x2(), _4(), _a(), ao(), ax(), b4() (+60 more)

### Community 38 - "NumberValue"
Cohesion: 0.04
Nodes (30): Override, Override, Override, NUMBER, ABS, Override, CEIL, Override (+22 more)

### Community 39 - "Coin"
Cohesion: 0.07
Nodes (8): Coin, DataInputStream, DataOutputStream, Override, StateVariable, Override, send, GenesisCoin

### Community 40 - "MDSJS"
Cohesion: 0.11
Nodes (12): APIService, COMMSService, FILEService, JSONArray, KEYPAIRService, NETService, org.minima.system.mds.runnable.api.APIService, CMDcommand (+4 more)

### Community 41 - "P2PState"
Cohesion: 0.08
Nodes (9): InetSocketAddress, Override, P2PManager, InetSocketAddress, P2PState, SwapLinksFunctions, SwapLinksFunctionsTest, InetSocketAddress (+1 more)

### Community 44 - "At"
Cohesion: 0.03
Nodes (25): sv(), Ad(), At, av(), Ci(), Ds, fv(), gv() (+17 more)

### Community 45 - ".runFunction"
Cohesion: 0.15
Nodes (6): Override, PREVSTATE, Override, SAMESTATE, Override, STATE

### Community 47 - "TxPoW"
Cohesion: 0.04
Nodes (12): DataInputStream, DataOutputStream, Override, TxPoW, TxPoWChecker, Override, scanchain, history (+4 more)

### Community 48 - "Part 1 — Security: `Wallet.signData` is an unsynchronized read-modify-write"
Cohesion: 0.10
Nodes (19): 2a. An `OutOfMemoryError` is an `Error`, not an `Exception` — `1770d8a`, 2b. Stream the IBD in batches — `a9ef09d`, 2c. Heap exhaustion is process-global — `49feed4`, 2d. The MMR tree is the real ceiling — `33c438d`, Also in this commit — `sendsign` burns a leaf per input coin, Branches, Evidence, minima-core — what we changed in our fork, and why (+11 more)

### Community 49 - ".forEach"
Cohesion: 0.11
Nodes (6): G9(), If(), pe, ty(), u1(), Ws()

### Community 50 - "u5"
Cohesion: 0.05
Nodes (65): Ad(), ah(), b1(), c5(), ca(), cc(), cs(), _d() (+57 more)

### Community 51 - "Eo"
Cohesion: 0.12
Nodes (13): a5(), Eo(), gd(), ih(), oc(), oh(), pc(), Ro() (+5 more)

### Community 52 - "ot"
Cohesion: 0.03
Nodes (10): dv(), e4(), Er(), hv(), mr(), ot(), pm(), qy() (+2 more)

### Community 53 - "yr"
Cohesion: 0.17
Nodes (16): F3(), ai(), b3(), d3, Dp(), Fp(), g3(), j3() (+8 more)

### Community 54 - "du"
Cohesion: 0.19
Nodes (13): qm(), cd(), cu(), dd(), du(), fd(), fu(), Hi() (+5 more)

### Community 55 - "k9"
Cohesion: 0.09
Nodes (8): f8(), k8, k9(), kl(), q9, v9(), wl(), y4()

### Community 56 - "org.mozilla.javascript.Scriptable"
Cohesion: 0.07
Nodes (20): org.mozilla.javascript.Context, org.mozilla.javascript.ContextFactory, org.mozilla.javascript.NativeJavaObject, org.mozilla.javascript.Scriptable, org.mozilla.javascript.WrapFactory, APICallback, APIService, COMMSService (+12 more)

### Community 58 - "SPHINCS Signature Verification KISS VM Script"
Cohesion: 0.10
Nodes (21): Bitcoin (Satoshi Nakamoto), Complete Node, Ethereum, Immutable / Ossified Protocol, Merkle Signature Scheme, Minima Protocol, Quantum Secure Cryptography (KECCAK + WOTS), Minima Scripting Language (Smart Contracts, Tokens) (+13 more)

### Community 59 - ".getBytes"
Cohesion: 0.05
Nodes (18): GZIPInputStream, DataInputStream, Override, TxPoWList, decryptbackup, DataInputStream, Override, DataInputStream (+10 more)

### Community 60 - ".toJSONString"
Cohesion: 0.11
Nodes (5): Minima, JSONStreamAware, JSONValue, Override, JSONWriter

### Community 62 - "Message"
Cohesion: 0.10
Nodes (3): P2PWalkLinks, WalkLinksFuncs, Message

### Community 63 - ".toString"
Cohesion: 0.08
Nodes (12): ADDRESS, Override, Override, LEN, Override, REPLACE, Override, REPLACEFIRST (+4 more)

### Community 64 - ".runCommand"
Cohesion: 0.15
Nodes (5): Override, sendpoll, Override, SendPollManager, SendPollMessage

### Community 67 - ".parse"
Cohesion: 0.07
Nodes (4): ContainerFactory, ContentHandler, Yylex, Yytoken

### Community 68 - ".log"
Cohesion: 0.03
Nodes (19): java.lang.Thread.UncaughtExceptionHandler, org.minima.utils.messages.Message, JSONObject, JSONObject, JSONObject, Message, Override, Override (+11 more)

### Community 69 - ".to0xString"
Cohesion: 0.05
Nodes (7): DataInputStream, DataOutputStream, Override, DataInputStream, hash, Override, BaseConverter

### Community 70 - "ServiceJSRunner"
Cohesion: 0.24
Nodes (5): Message, org.minima.database.minidapps.MiniDAPP, MDSHubInstall, MDSManager, ServiceJSRunner

### Community 71 - "ParamConfigurer"
Cohesion: 0.15
Nodes (5): MiniNumber, org.junit.runner.RunWith, ParamConfigurer, UnknownArgumentException, ParamConfigurerTest

### Community 75 - "Ln"
Cohesion: 0.32
Nodes (8): Ka(), L0(), Ln(), qa(), Rl(), w1(), Wf(), Xa()

### Community 78 - "MDSManager"
Cohesion: 0.07
Nodes (12): JsonDB, org.minima.system.network.rpc.Server, org.minima.utils.JsonDB, org.minima.utils.SqlDB, APICommand, COMMSCommand, KEYPAIRcommand, SQLcommand (+4 more)

### Community 79 - "ql"
Cohesion: 0.05
Nodes (47): a1(), a3(), Ar(), B2(), bi(), cl(), E2(), ed() (+39 more)

### Community 80 - ".processMessage"
Cohesion: 0.18
Nodes (4): Override, webhooks, Override, NotifyManager

### Community 86 - "b4"
Cohesion: 0.06
Nodes (8): b4(), c3(), en(), fp, gg, I4(), Rf(), xl()

### Community 88 - "Tx-PoW"
Cohesion: 0.25
Nodes (8): Cascading Proof Chain, Distributed PoW, GHOST Consensus, IOTA Tangle, Maxima P2P Information Network, P2Pool, Pulse Network, Tx-PoW

### Community 90 - "n"
Cohesion: 0.05
Nodes (78): _3(), a3(), a9(), ac(), Al(), ar(), b(), Bh() (+70 more)

### Community 91 - "mmrproof"
Cohesion: 0.36
Nodes (3): Override, mmrleafnode, mmrproof

### Community 99 - "it"
Cohesion: 0.09
Nodes (36): Ag(), As(), b0(), bp(), dl(), ep(), es(), ga() (+28 more)

### Community 107 - "n"
Cohesion: 0.16
Nodes (60): bf(), a(), af(), aw(), b(), Bf(), bw(), c() (+52 more)

### Community 108 - "qc"
Cohesion: 0.07
Nodes (54): ic(), ir(), Or(), ys(), dy(), cs(), Dc(), ed() (+46 more)

### Community 109 - "r9"
Cohesion: 0.10
Nodes (19): bl(), cv(), d3(), Fa(), i9, kv(), lw(), om() (+11 more)

### Community 111 - "bg"
Cohesion: 0.07
Nodes (56): Je(), $1(), a0(), ap(), bc(), bg(), _c(), ca() (+48 more)

### Community 116 - "java.net.Socket"
Cohesion: 0.18
Nodes (8): java.net.Socket, org.minima.system.mds.polling.PollStack, MDSCompleteHandler, MDSManager, MDSCommandHandler, CMDHandler, ArchiveHandler, Override

### Community 127 - ".toString"
Cohesion: 0.08
Nodes (27): _0(), bs(), c6(), Et(), eu(), In(), iu(), jh() (+19 more)

### Community 140 - ".getMiniDataVersion"
Cohesion: 0.08
Nodes (9): NetworkManager, NotifyManager, SendPollManager, DataOutputStream, Override, jnitest, jnifunctions, TxPoWMiner (+1 more)

### Community 144 - "MessageProcessor"
Cohesion: 0.05
Nodes (8): org.minima.system.network.minima.NIOManager, Override, systemcheck, NetworkManager, Server, Override, MessageProcessor, MessageStack

### Community 155 - "E"
Cohesion: 0.07
Nodes (44): Qh(), Ai(), Ao(), av(), Bi(), bl(), Da(), dl() (+36 more)

### Community 171 - ".main"
Cohesion: 0.14
Nodes (4): Override, sphincs, SPHINCS, TransactionTest

### Community 174 - "gradlew"
Cohesion: 0.60
Nodes (3): gradlew script, die(), warn()

### Community 175 - "Storage-less MMR UTXO Database"
Cohesion: 0.40
Nodes (5): Storage-less MMR UTXO Database, Peter Todd MMR (Delayed TXO Commitments), MegaMMR Node Mode (-megammr), PROOF Merkle Proof Function, HORST Tree Proof Verification Loop (Step 2)

### Community 180 - "MiniDAPP"
Cohesion: 0.09
Nodes (7): java.util.zip.ZipEntry, Override, MDSDB, JSONObject, MiniDAPP, MDSHubUpdate, ZipExtractor

### Community 181 - "The Burn"
Cohesion: 0.67
Nodes (3): Adaptive Block Size, The Burn, HashCash

### Community 203 - "yo"
Cohesion: 0.10
Nodes (31): Ba(), bo(), cn(), dn(), Et(), gr(), gt(), Ia() (+23 more)

### Community 204 - "vp"
Cohesion: 0.07
Nodes (43): Ks(), An(), au(), cd(), cm(), cu(), dd(), Df() (+35 more)

### Community 206 - ".run"
Cohesion: 0.08
Nodes (8): DataOutputStream, MDSManager, Override, MDSFileHandler, JSONObject, MultipartData, MultipartParser, MDSPublicPage

### Community 208 - "wu"
Cohesion: 0.10
Nodes (37): ri(), El(), ni(), Nl(), au(), Bc(), bu(), ci() (+29 more)

### Community 210 - "na"
Cohesion: 0.07
Nodes (42): jm(), D1(), Di(), E1(), ec(), f0(), fc(), fs() (+34 more)

### Community 212 - "org.minima.utils.json.JSONObject"
Cohesion: 0.10
Nodes (9): org.minima.utils.json.JSONObject, MEGAPOLLcommand, POLLcommand, JSONObject, PendingCommand, JSONObject, PollMessage, JSONObject (+1 more)

### Community 213 - "org.minima.system.commands.Command"
Cohesion: 0.12
Nodes (10): org.minima.system.commands.Command, checkmode, Override, checkpending, Override, checkrestore, JSONObject, Override (+2 more)

### Community 214 - ".includes"
Cohesion: 0.10
Nodes (36): A6(), bk(), bw(), D6(), fk(), fx(), gk(), Gt() (+28 more)

### Community 215 - "pb"
Cohesion: 0.13
Nodes (25): Bv(), cx(), dg(), dv(), fv(), gb(), Gg(), Gv() (+17 more)

### Community 216 - "Xn"
Cohesion: 0.09
Nodes (18): _4(), Bu(), c8(), c9(), f9(), gp(), hn(), ig (+10 more)

### Community 217 - "org.minima.system.mds.MDSManager"
Cohesion: 0.07
Nodes (13): org.minima.system.mds.MDSManager, APIAutoResponse, Override, MDSHub, MDSHubDelete, MDSHubError, MDSHubInstallError, MDSHubLoggedOn (+5 more)

### Community 219 - "e"
Cohesion: 0.10
Nodes (32): _c(), Cd(), ch(), ck(), d1(), Da(), ga(), Is() (+24 more)

### Community 221 - ".loadAllDB"
Cohesion: 0.11
Nodes (12): ArchiveManager, Cascade, MegaMMR, P2P2DB, P2PDB, ReentrantReadWriteLock, TxBlockDB, TxnDB (+4 more)

### Community 222 - "ms"
Cohesion: 0.14
Nodes (18): bs(), ds(), es(), fs(), gs(), hs(), is(), Jl() (+10 more)

### Community 226 - "yp"
Cohesion: 0.20
Nodes (17): pp(), T2(), Nf(), bp(), ep(), Fp(), Gf(), gp() (+9 more)

### Community 227 - "eo"
Cohesion: 0.14
Nodes (18): aa(), d0(), eo(), g0(), j1(), jt(), K0(), ns() (+10 more)

### Community 228 - "polyfills-legacy-9ffc4e0f.js"
Cohesion: 0.30
Nodes (14): a(), c(), d(), h(), i(), l(), n(), o() (+6 more)

### Community 229 - "mx"
Cohesion: 0.14
Nodes (3): addChildren(), mx, Os()

### Community 231 - "java.security.cert.X509Certificate"
Cohesion: 0.31
Nodes (7): java.security.cert.X509Certificate, javax.net.ssl.TrustManager, javax.net.ssl.X509TrustManager, MiniData, org.minima.objects.base.MiniData, Override, MinimaTrustManager

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

### Community 241 - "SSLManager"
Cohesion: 0.38
Nodes (3): java.security.KeyStore, javax.net.ssl.KeyManagerFactory, SSLManager

### Community 242 - "SelfSignedCertGenerator.java"
Cohesion: 0.31
Nodes (5): java.security.KeyPair, java.security.PublicKey, org.bouncycastle.asn1.x509.AuthorityKeyIdentifier, org.bouncycastle.asn1.x509.SubjectKeyIdentifier, SelfSignedCertGenerator

### Community 243 - "HTTPSServer.java"
Cohesion: 0.26
Nodes (6): javax.net.ssl.SSLContext, javax.net.ssl.SSLServerSocket, javax.net.ssl.SSLSocket, Server, HTTPSServer, Override

### Community 248 - "EncryptDecrypt"
Cohesion: 0.19
Nodes (3): java.security.PrivateKey, EncryptDecrypt, SignVerify

## Knowledge Gaps
- **191 isolated node(s):** `lu`, `sx`, `yx`, `t7`, `n7` (+186 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **119 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Command` connect `JSONObject` to `.getTxPoWID`, `.getDB`, `Contract`, `MiniData`, `Main`, `txncheck`, `.runCommand`, `.run`, `MiniNumber`, `CascadeNode`, `.runCommand`, `TreeKey`, `mmrcreate`, `Coin`, `backup`, `TxPoW`, `.runCommand`, `.getBytes`, `archive`, `.runCommand`, `txnaddamount`, `txncoinlock`, `.log`, `.to0xString`, `magic`, `.processMessage`, `printtree`, `status`, `txnlock`, `tutorial`, `mysqlcoins`, `mmrproof`, `benchmark`, `jniminingtest`, `mysql`, `logs`, `maths`, `quit`, `timemilli`, `disconnect`, `message`, `p2pstate`, `sendsign`, `test`, `reset`, `balance`, `coincheck`, `coinexport`, `coinnotify`, `consolidate`, `convert`, `getaddress`, `hashtest`, `incentivecash`, `random`, `seedrandom`, `slavenode`, `tokencreate`, `tokenvalidate`, `trace`, `peers`, `rpc`, `newscript`, `removescript`, `runscript`, `scripts`, `coins`, `.getMiniDataVersion`, `tokens`, `txpow`, `multisig`, `MessageProcessor`, `sendnosign`, `.runCommand`, `sendview`, `rawfrom`, `rawtxnfrom`, `sign`, `verify`, `tutorial`, `txnbasics`, `txnclear`, `txncreate`, `txndelete`, `txnexport`, `txnimport`, `txninput`, `txnlist`, `txnoutput`, `txnpost`, `txnscript`, `txnsign`, `txnstate`, `txnview`, `whitepaper`, `.main`, `automine`, `createfrom`, `postfrom`, `checkaddress`, `debugflag`, `healthcheck`, `network`, `missingcmd`, `newaddress`, `printmmr`, `multisigread`, `constructfrom`, `createtokenfrom`, `sendfrom`, `signfrom`, `txnmine`, `txnminepost`, `txnmmr`, `txnauto`, `coinimport`, `cointrack`?**
  _High betweenness centrality (0.072) - this node is a cross-community bridge._
- **Why does `MiniData` connect `MiniData` to `org.junit.Test`, `.testConstructors`, `.getTxPoWID`, `.getDB`, `Contract`, `Main`, `JSONObject`, `.getHexParam`, `.getMiniDataVersion`, `NIOClient`, `Expression`, `UserDB`, `.runCommand`, `.run`, `MiniNumber`, `.checkOpen`, `java.io.DataInputStream`, `.runCommand`, `AesUtil`, `TreeKey`, `MMR`, `java.net.InetSocketAddress`, `Coin`, `P2PState`, `ArchiveManager`, `.main`, `TxPoW`, `.getBytes`, `Message`, `.to0xString`, `mmrproof`, `Maths`?**
  _High betweenness centrality (0.050) - this node is a cross-community bridge._
- **Why does `JSONObject` connect `JSONObject` to `.testConstructors`, `NIOClientInfo`, `.getTxPoWID`, `.getDB`, `Contract`, `MiniData`, `Main`, `.getMiniDataVersion`, `NIOClient`, `UserDB`, `MessageProcessor`, `.runCommand`, `.runCommand`, `.run`, `MiniNumber`, `.checkOpen`, `java.io.DataInputStream`, `CascadeNode`, `.runCommand`, `tutorial`, `TreeKey`, `MMR`, `java.net.InetSocketAddress`, `Coin`, `whitepaper`, `P2PState`, `.runCommand`, `automine`, `TxPoW`, `.runCommand`, `.getBytes`, `.toJSONString`, `Message`, `missingcmd`, `.runCommand`, `txncoinlock`, `.log`, `.to0xString`, `.processMessage`, `txnlock`, `tutorial`, `disconnect`, `message`, `p2pstate`, `JSONAware`?**
  _High betweenness centrality (0.045) - this node is a cross-community bridge._
- **What connects `lu`, `sx`, `yx` to the rest of the system?**
  _191 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `org.junit.Test` be split into smaller, more focused modules?**
  _Cohesion score 0.03507670850767085 - nodes in this community are weakly interconnected._
- **Should `.testConstructors` be split into smaller, more focused modules?**
  _Cohesion score 0.025757575757575757 - nodes in this community are weakly interconnected._
- **Should `NIOClientInfo` be split into smaller, more focused modules?**
  _Cohesion score 0.0653061224489796 - nodes in this community are weakly interconnected._