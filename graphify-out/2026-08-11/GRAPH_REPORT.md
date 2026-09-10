# Graph Report - .  (2026-07-27)

## Corpus Check
- 465 files · ~249,248 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 4666 nodes · 18568 edges · 188 communities (87 shown, 101 thin omitted)
- Extraction: 75% EXTRACTED · 25% INFERRED · 0% AMBIGUOUS · INFERRED: 4716 edges (avg confidence: 0.8)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- KISS VM Functions & Params
- Contract Execution Engine
- Node Commands & Mining
- TxPoW Database
- Wallet & Transaction DB
- KISS Script Expressions
- Chain Magic & Sync
- Coin Relevance & Script Fns
- Core DB & Archive Glue
- MinimaDB Tables
- Transaction Rows & Witness
- KISS Bool/Number Functions
- Archive Manager
- NIO Networking
- Script Expression Parser
- UserDB & MiniDapp Prefs
- P2P Greeting & Logging
- Startup Params
- Mempool & Block Queries
- KISS Crypto/Hex Functions
- Message Processors
- DB Locking & Coin Notify
- On-chain TxPoW DB
- Cascade Serialization
- KISS Address Functions
- Community 25
- Community 26
- Community 27
- Community 28
- Community 29
- Community 30
- Community 31
- Community 32
- Community 33
- Community 34
- Community 35
- Community 36
- Community 37
- Community 38
- Community 39
- Community 40
- Community 41
- Community 42
- Community 43
- Community 44
- Community 45
- Community 46
- Community 47
- Community 48
- Community 49
- Community 50
- Community 51
- Community 52
- Community 53
- Community 54
- Community 55
- Community 56
- Community 57
- Community 58
- Community 59
- Community 60
- Community 61
- Community 62
- Community 63
- Community 64
- Community 65
- Community 66
- Community 67
- Community 68
- Community 69
- Community 70
- Community 71
- Community 72
- Community 73
- Community 74
- Community 75
- Community 76
- Community 77
- Community 78
- Community 79
- Community 80
- Community 81
- Community 82
- Community 83
- Community 84
- Community 85
- Community 86
- Community 87
- Community 88
- Community 89
- Community 90
- Community 91
- Community 92
- Community 93
- Community 94
- Community 95
- Community 96
- Community 97
- Community 98
- Community 99
- Community 100
- Community 101
- Community 102
- Community 103
- Community 104
- Community 105
- Community 106
- Community 107
- Community 108
- Community 109
- Community 110
- Community 111
- Community 112
- Community 113
- Community 114
- Community 115
- Community 116
- Community 117
- Community 118
- Community 119
- Community 120
- Community 121
- Community 122
- Community 123
- Community 124
- Community 125
- Community 126
- Community 127
- Community 128
- Community 129
- Community 130
- Community 131
- Community 132
- Community 133
- Community 134
- Community 135
- Community 136
- Community 137
- Community 138
- Community 139
- Community 140
- Community 141
- Community 142
- Community 143
- Community 144
- Community 145
- Community 146
- Community 147
- Community 148
- Community 149
- Community 150
- Community 151
- Community 152
- Community 153
- Community 154
- Community 155
- Community 156
- Community 157
- Community 158
- Community 159
- Community 160
- Community 161
- Community 162
- Community 163
- Community 164
- Community 165
- Community 166
- Community 167
- Community 168
- Community 169
- Community 170
- Community 171
- Community 172
- Community 173
- Community 174
- Community 175
- Community 176
- Community 177
- Community 178
- Community 179
- Community 180
- Community 181
- Community 182
- Community 183
- Community 185

## God Nodes (most connected - your core abstractions)
1. `JSONObject` - 395 edges
2. `Command` - 383 edges
3. `MiniData` - 332 edges
4. `MiniNumber` - 232 edges
5. `MinimaFunction` - 231 edges
6. `NumberValue` - 206 edges
7. `Value` - 197 edges
8. `TxPoW` - 196 edges
9. `Contract` - 193 edges
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

## Communities (188 total, 101 thin omitted)

### Community 0 - "KISS VM Functions & Params"
Cohesion: 0.04
Nodes (59): Override, CONCATTests, Test, GETTests, Test, Test, LENTests, Test (+51 more)

### Community 1 - "Contract Execution Engine"
Cohesion: 0.03
Nodes (50): Contract, Expression, FunctionExpression, Override, GlobalExpression, Override, Override, VariableExpression (+42 more)

### Community 2 - "Node Commands & Mining"
Cohesion: 0.02
Nodes (44): automine, Override, block, Override, checkaddress, Override, debugflag, Override (+36 more)

### Community 3 - "TxPoW Database"
Cohesion: 0.04
Nodes (14): DataInputStream, DataOutputStream, Override, TxPowTree, TxPoWTreeNode, TxPoWTreeNodeAction, TimedChecker, TxPoWGenerator (+6 more)

### Community 5 - "KISS Script Expressions"
Cohesion: 0.12
Nodes (5): MinimaFunction, HexValue, NumberValue, StringValue, Value

### Community 6 - "Chain Magic & Sync"
Cohesion: 0.04
Nodes (9): MathContext, MiniNumber, Magic, Token, burn, Override, AddressAmount, TestParams (+1 more)

### Community 7 - "Coin Relevance & Script Fns"
Cohesion: 0.07
Nodes (25): Override, VERIFYIN, Address, Coin, ScriptProof, SPHINCSUtils, TransactionTest, GETINADDRTests (+17 more)

### Community 8 - "Core DB & Archive Glue"
Cohesion: 0.06
Nodes (8): GZIPInputStream, KeyPair, SendPollManager, Main, InetSocketAddress, Level, P2PParams, MessageProcessor

### Community 9 - "MinimaDB Tables"
Cohesion: 0.13
Nodes (4): TxnDB, ResultSet, ScriptRow, JSONObject

### Community 10 - "Transaction Rows & Witness"
Cohesion: 0.04
Nodes (20): DataInputStream, DataOutputStream, Override, TxnRow, Override, MiniString, DataInputStream, StateVariable (+12 more)

### Community 11 - "KISS Bool/Number Functions"
Cohesion: 0.04
Nodes (30): BOOL, Override, Override, NUMBER, Override, STRING, Override, OVERWRITE (+22 more)

### Community 12 - "Archive Manager"
Cohesion: 0.06
Nodes (9): ArchiveManager, Override, PreparedStatement, TxBlockDB, TxBlock, Override, Connection, PreparedStatement (+1 more)

### Community 13 - "NIO Networking"
Cohesion: 0.04
Nodes (12): ByteBuffer, SelectionKey, Selector, Override, SocketChannel, NIOClient, Override, NIOManager (+4 more)

### Community 14 - "Script Expression Parser"
Cohesion: 0.09
Nodes (14): ExpressionParser, StatementParser, LexicalTokenizer, Override, ScriptToken, ScriptTokenizer, ExpressionParserTests, Test (+6 more)

### Community 15 - "UserDB & MiniDapp Prefs"
Cohesion: 0.04
Nodes (8): UserDB, Authorizer, JSONArray, JSONAware, JsonDB, MiniFormat, SimpleDateFormat, MiniUtil

### Community 16 - "P2P Greeting & Logging"
Cohesion: 0.10
Nodes (5): InetSocketAddress, Override, P2PManager, P2PState, SwapLinksFunctions

### Community 17 - "Startup Params"
Cohesion: 0.03
Nodes (68): ParamKeys, allowallip, allowgenmessage, anyseed, archive, basefolder, clean, conf (+60 more)

### Community 19 - "KISS Crypto/Hex Functions"
Cohesion: 0.05
Nodes (20): ASCII, Override, Override, UTF8, BITCOUNT, Override, BITGET, Override (+12 more)

### Community 20 - "Message Processors"
Cohesion: 0.05
Nodes (12): Override, Override, Override, P2P2Manager, Override, Override, Override, Message (+4 more)

### Community 21 - "DB Locking & Coin Notify"
Cohesion: 0.05
Nodes (5): ReentrantReadWriteLock, MinimaDB, Override, P2P2DB, P2PDB

### Community 22 - "On-chain TxPoW DB"
Cohesion: 0.06
Nodes (9): PreparedStatement, TxPoWOnChainDB, PreparedStatement, TxPoWSqlDB, CoinDB, Override, PreparedStatement, Connection (+1 more)

### Community 23 - "Cascade Serialization"
Cohesion: 0.06
Nodes (21): DataInputStream, DataOutputStream, Override, DataInputStream, DataOutputStream, Override, DataInputStream, DataOutputStream (+13 more)

### Community 24 - "KISS Address Functions"
Cohesion: 0.06
Nodes (17): ADDRESS, Override, Override, LEN, Override, SHA2, Override, SHA3 (+9 more)

### Community 25 - "Community 25"
Cohesion: 0.07
Nodes (8): RamData, RamDB, TxPoWDB, Override, mempool, Test, RamDBTest, TestUtils

### Community 26 - "Community 26"
Cohesion: 0.07
Nodes (16): GETINAMT, Override, GETINID, Override, Override, SUMINPUTS, GETOUTADDR, Override (+8 more)

### Community 27 - "Community 27"
Cohesion: 0.09
Nodes (11): OperatorExpression, CryptoPackage, DataInputStream, DataOutputStream, Override, EncryptDecrypt, GenerateKey, Cipher (+3 more)

### Community 28 - "Community 28"
Cohesion: 0.08
Nodes (8): DataInputStream, DataOutputStream, Override, SignatureProof, TreeKey, TreeKeyNode, Winternitz, WinternitzOTSignature

### Community 29 - "Community 29"
Cohesion: 0.09
Nodes (12): DataInputStream, DataOutputStream, Override, MMRData, DataInputStream, DataOutputStream, Override, MMRProof (+4 more)

### Community 30 - "Community 30"
Cohesion: 0.12
Nodes (4): MMREntry, MathContext, Override, MMREntryNumber

### Community 31 - "Community 31"
Cohesion: 0.09
Nodes (12): FORS, FORSSignature, DataInputStream, DataOutputStream, Override, HORST, HORSTSignature, DataInputStream (+4 more)

### Community 32 - "Community 32"
Cohesion: 0.08
Nodes (11): Override, nodecount, InetSocketAddressIO, InetSocketAddress, InetSocketAddress, Override, P2PGreeting, GreetingTest (+3 more)

### Community 33 - "Community 33"
Cohesion: 0.07
Nodes (10): ServerSocket, NetworkManager, HTTPServer, Override, Socket, Server, ArchiveHandler, ArchiveServer (+2 more)

### Community 34 - "Community 34"
Cohesion: 0.08
Nodes (12): EXISTS, Override, GET, Override, CONCAT, Override, Override, MAX (+4 more)

### Community 35 - "Community 35"
Cohesion: 0.09
Nodes (6): MiniData, AddressAmount, GenesisCoin, SPHINCS, RequestUnit, TimedRequest

### Community 36 - "Community 36"
Cohesion: 0.08
Nodes (8): ResultSet, SeedRow, Override, PreparedStatement, Wallet, Override, newaddress, BIP39

### Community 37 - "Community 37"
Cohesion: 0.08
Nodes (18): DataInputStream, DataOutputStream, Override, DataInputStream, DataOutputStream, DataInputStream, DataOutputStream, Override (+10 more)

### Community 38 - "Community 38"
Cohesion: 0.09
Nodes (11): BooleanExpression, Override, DEC, Override, INC, Override, Override, BooleanExpressionTests (+3 more)

### Community 40 - "Community 40"
Cohesion: 0.11
Nodes (10): IBD, DataInputStream, DataOutputStream, Override, Override, megammr, DataInputStream, DataOutputStream (+2 more)

### Community 41 - "Community 41"
Cohesion: 0.13
Nodes (6): InetSocketAddress, SwapLinksFunctionsTest, InetSocketAddress, QuickInetLists, QuickState, WalkLinksFuncsTest

### Community 42 - "Community 42"
Cohesion: 0.10
Nodes (13): DataInputStream, DataOutputStream, Override, TxPoWList, decryptbackup, DataInputStream, Override, DataInputStream (+5 more)

### Community 43 - "Community 43"
Cohesion: 0.13
Nodes (8): Cascade, DataInputStream, DataOutputStream, Override, CascadeNode, DataInputStream, DataOutputStream, Override

### Community 44 - "Community 44"
Cohesion: 0.10
Nodes (7): DataInputStream, DataOutputStream, Override, MiniByte, NIOMessage, Test, MiniByteTests

### Community 45 - "Community 45"
Cohesion: 0.12
Nodes (4): InetSocketAddress, P2PWalkLinks, InetSocketAddress, WalkLinksFuncs

### Community 46 - "Community 46"
Cohesion: 0.08
Nodes (8): TxPoW, TxPoWMiner, backup, Override, SimpleDateFormat, Override, txnminepost, GenesisTxPoW

### Community 48 - "Community 48"
Cohesion: 0.08
Nodes (13): ConstantExpression, Override, BooleanValue, Override, Override, ConstantExpressionTests, Test, Test (+5 more)

### Community 49 - "Community 49"
Cohesion: 0.10
Nodes (7): BufferedInputStream, FileInputStream, DataInputStream, RawArchiveInput, archive, Override, MiniFile

### Community 50 - "Community 50"
Cohesion: 0.11
Nodes (5): JSONValue, ContainerFactory, ContentHandler, JSONParser, Yytoken

### Community 51 - "Community 51"
Cohesion: 0.10
Nodes (7): connect, Override, Level, DEBUG, INFO, NODE_RUNNER_MSG, P2P2Functions

### Community 52 - "Community 52"
Cohesion: 0.10
Nodes (10): DataInputStream, DataOutputStream, Override, DataOutputStream, Override, DataInputStream, DataOutputStream, Override (+2 more)

### Community 53 - "Community 53"
Cohesion: 0.10
Nodes (5): Override, status, Override, network, NIOTraffic

### Community 54 - "Community 54"
Cohesion: 0.11
Nodes (5): NIOClientInfo, InetSocketAddress, UtilFuncs, QuickClients, UtilFuncsTest

### Community 55 - "Community 55"
Cohesion: 0.15
Nodes (8): DataInputStream, DataOutputStream, Override, Signature, DataInputStream, DataOutputStream, Override, SPHINCSSignature

### Community 56 - "Community 56"
Cohesion: 0.14
Nodes (8): CoinProof, DataInputStream, DataOutputStream, Override, DataInputStream, DataOutputStream, Override, MegaMMRIBD

### Community 57 - "Community 57"
Cohesion: 0.14
Nodes (5): Level, DEBUG, INFO, NODE_RUNNER_MSG, P2PFunctions

### Community 58 - "Community 58"
Cohesion: 0.10
Nodes (21): Bitcoin (Satoshi Nakamoto), Complete Node, Ethereum, Immutable / Ossified Protocol, Merkle Signature Scheme, Minima Protocol, Quantum Secure Cryptography (KECCAK + WOTS), Minima Scripting Language (Smart Contracts, Tokens) (+13 more)

### Community 59 - "Community 59"
Cohesion: 0.15
Nodes (4): DataInputStream, DataOutputStream, Override, MegaMMR

### Community 60 - "Community 60"
Cohesion: 0.16
Nodes (4): Override, JSONStreamAware, Override, JSONWriter

### Community 61 - "Community 61"
Cohesion: 0.14
Nodes (4): Override, systemcheck, Override, TimerProcessor

### Community 62 - "Community 62"
Cohesion: 0.18
Nodes (6): HEX, Override, Override, POW, Override, SIGDIG

### Community 63 - "Community 63"
Cohesion: 0.20
Nodes (4): FUNCTION, Override, Override, REPLACE

### Community 64 - "Community 64"
Cohesion: 0.18
Nodes (3): Override, sendpoll, SendPollMessage

### Community 65 - "Community 65"
Cohesion: 0.18
Nodes (3): Override, webhooks, NotifyManager

### Community 66 - "Community 66"
Cohesion: 0.15
Nodes (5): InetSocketAddress, P2PPeersChecker, InetSocketAddress, Level, P2PTestParams

### Community 68 - "Community 68"
Cohesion: 0.24
Nodes (5): Before, InetSocketAddress, InetSocketAddress, Test, P2PDBTest

### Community 70 - "Community 70"
Cohesion: 0.26
Nodes (5): DataInputStream, DataOutputStream, Override, CoinTests, Test

### Community 71 - "Community 71"
Cohesion: 0.23
Nodes (4): Pair, ParamConfigurer, toParamKey(), UnknownArgumentException

### Community 72 - "Community 72"
Cohesion: 0.33
Nodes (3): AesUtil, Cipher, SecretKey

### Community 74 - "Community 74"
Cohesion: 0.24
Nodes (5): DataInputStream, DataOutputStream, Override, AddressTests, Test

### Community 75 - "Community 75"
Cohesion: 0.24
Nodes (4): Greeting, DataInputStream, DataOutputStream, Override

### Community 78 - "Community 78"
Cohesion: 0.29
Nodes (4): DataInputStream, DataOutputStream, Override, Pulse

### Community 79 - "Community 79"
Cohesion: 0.27
Nodes (5): DataInputStream, DataOutputStream, Override, Test, TokenTests

### Community 81 - "Community 81"
Cohesion: 0.25
Nodes (3): Crypto, DataInputStream, DataOutputStream

### Community 82 - "Community 82"
Cohesion: 0.38
Nodes (3): RunWith, Test, ParamConfigurerTest

### Community 83 - "Community 83"
Cohesion: 0.27
Nodes (4): DataInputStream, DataOutputStream, Override, MegaMMRSyncData

### Community 87 - "Community 87"
Cohesion: 0.25
Nodes (5): SimpleDateFormat, MinimaLogger, Override, MinimaUncaughtException, UncaughtExceptionHandler

### Community 88 - "Community 88"
Cohesion: 0.25
Nodes (8): Cascading Proof Chain, Distributed PoW, GHOST Consensus, IOTA Tangle, Maxima P2P Information Network, P2Pool, Pulse Network, Tx-PoW

### Community 89 - "Community 89"
Cohesion: 0.32
Nodes (3): DataInputStream, DataOutputStream, Override

### Community 90 - "Community 90"
Cohesion: 0.36
Nodes (3): Override, mmrcreate, mmrleafnode

### Community 91 - "Community 91"
Cohesion: 0.36
Nodes (3): Override, mmrleafnode, mmrproof

### Community 92 - "Community 92"
Cohesion: 0.29
Nodes (3): ExecutionException, MinimaException, MinimaParseException

### Community 170 - "Community 170"
Cohesion: 0.40
Nodes (3): CMDHandler, Override, Socket

### Community 174 - "Community 174"
Cohesion: 0.60
Nodes (3): gradlew script, die(), warn()

### Community 175 - "Community 175"
Cohesion: 0.40
Nodes (5): Storage-less MMR UTXO Database, Peter Todd MMR (Delayed TXO Commitments), MegaMMR Node Mode (-megammr), PROOF Merkle Proof Function, HORST Tree Proof Verification Loop (Step 2)

### Community 181 - "Community 181"
Cohesion: 0.67
Nodes (3): Adaptive Block Size, The Burn, HashCash

## Knowledge Gaps
- **90 isolated node(s):** `buildjars.sh script`, `NODE_RUNNER_MSG`, `INFO`, `DEBUG`, `NODE_RUNNER_MSG` (+85 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **101 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Command` connect `Node Commands & Mining` to `Wallet & Transaction DB`, `KISS Script Expressions`, `Chain Magic & Sync`, `Core DB & Archive Glue`, `MinimaDB Tables`, `Community 25`, `Community 32`, `Community 36`, `Community 39`, `Community 40`, `Community 42`, `Community 46`, `Community 49`, `Community 51`, `Community 53`, `Community 61`, `Community 64`, `Community 65`, `Community 80`, `Community 84`, `Community 85`, `Community 90`, `Community 91`, `Community 93`, `Community 94`, `Community 95`, `Community 96`, `Community 97`, `Community 98`, `Community 99`, `Community 100`, `Community 101`, `Community 102`, `Community 103`, `Community 104`, `Community 109`, `Community 110`, `Community 111`, `Community 112`, `Community 113`, `Community 114`, `Community 115`, `Community 116`, `Community 117`, `Community 118`, `Community 119`, `Community 120`, `Community 121`, `Community 122`, `Community 123`, `Community 124`, `Community 125`, `Community 126`, `Community 127`, `Community 128`, `Community 129`, `Community 130`, `Community 131`, `Community 132`, `Community 133`, `Community 134`, `Community 135`, `Community 136`, `Community 137`, `Community 138`, `Community 139`, `Community 140`, `Community 141`, `Community 142`, `Community 143`, `Community 144`, `Community 145`, `Community 146`, `Community 147`, `Community 148`, `Community 149`, `Community 150`, `Community 151`, `Community 152`, `Community 153`, `Community 154`, `Community 155`, `Community 156`, `Community 157`, `Community 158`, `Community 159`, `Community 160`, `Community 161`, `Community 162`, `Community 163`, `Community 164`, `Community 165`, `Community 166`, `Community 167`, `Community 168`, `Community 169`, `Community 177`, `Community 178`, `Community 179`?**
  _High betweenness centrality (0.146) - this node is a cross-community bridge._
- **Why does `JSONObject` connect `MinimaDB Tables` to `Contract Execution Engine`, `Node Commands & Mining`, `TxPoW Database`, `Wallet & Transaction DB`, `KISS Script Expressions`, `Chain Magic & Sync`, `Coin Relevance & Script Fns`, `Core DB & Archive Glue`, `Transaction Rows & Witness`, `Archive Manager`, `NIO Networking`, `UserDB & MiniDapp Prefs`, `P2P Greeting & Logging`, `Mempool & Block Queries`, `Message Processors`, `On-chain TxPoW DB`, `Community 25`, `Community 153`, `Community 27`, `Community 29`, `Community 30`, `Community 32`, `Community 36`, `Community 39`, `Community 40`, `Community 169`, `Community 42`, `Community 44`, `Community 45`, `Community 46`, `Community 47`, `Community 51`, `Community 52`, `Community 53`, `Community 54`, `Community 55`, `Community 57`, `Community 60`, `Community 61`, `Community 64`, `Community 65`, `Community 68`, `Community 69`, `Community 75`, `Community 76`, `Community 84`, `Community 85`, `Community 96`, `Community 97`, `Community 98`, `Community 99`, `Community 100`, `Community 101`, `Community 102`, `Community 103`?**
  _High betweenness centrality (0.119) - this node is a cross-community bridge._
- **Why does `MiniData` connect `Community 35` to `Node Commands & Mining`, `TxPoW Database`, `Wallet & Transaction DB`, `KISS Script Expressions`, `Chain Magic & Sync`, `Coin Relevance & Script Fns`, `Core DB & Archive Glue`, `MinimaDB Tables`, `Transaction Rows & Witness`, `Archive Manager`, `NIO Networking`, `UserDB & MiniDapp Prefs`, `P2P Greeting & Logging`, `Mempool & Block Queries`, `KISS Crypto/Hex Functions`, `On-chain TxPoW DB`, `Cascade Serialization`, `Community 27`, `Community 28`, `Community 29`, `Community 31`, `Community 36`, `Community 39`, `Community 42`, `Community 43`, `Community 44`, `Community 45`, `Community 46`, `Community 52`, `Community 55`, `Community 56`, `Community 69`, `Community 70`, `Community 75`, `Community 76`, `Community 78`, `Community 79`, `Community 81`, `Community 83`, `Community 85`, `Community 89`, `Community 91`, `Community 105`?**
  _High betweenness centrality (0.114) - this node is a cross-community bridge._
- **What connects `buildjars.sh script`, `NODE_RUNNER_MSG`, `INFO` to the rest of the system?**
  _90 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `KISS VM Functions & Params` be split into smaller, more focused modules?**
  _Cohesion score 0.036129481378350155 - nodes in this community are weakly interconnected._
- **Should `Contract Execution Engine` be split into smaller, more focused modules?**
  _Cohesion score 0.028908826010275286 - nodes in this community are weakly interconnected._
- **Should `Node Commands & Mining` be split into smaller, more focused modules?**
  _Cohesion score 0.023310023310023312 - nodes in this community are weakly interconnected._