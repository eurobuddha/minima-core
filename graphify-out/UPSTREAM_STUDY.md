# Minima Core upstream study

Studied 2026-09-08. Repository: https://github.com/spartacusrex-minima/minima-core

Upstream main: `5564b9d78978efcaf23679190d05253961fc6e69` (commit subject `1.1.2.8`).
Clean detached worktree: `../upstream-study/` relative to this report directory.
The original checkout remains on `feat/megammr-import-robustness`; local branches and existing untracked files were preserved. Fetch advanced origin/main from a8b34bc to 5564b9d. The existing graph describes the development checkout, including MDS additions, so it was used only for navigation; upstream source was checked directly.

## Architecture and source map

Paths below are relative to upstream-study/.

| Area | Sources inspected | Role |
|---|---|---|
| Entry and embedding | src/org/minima/Minima.java | CLI startup, configuration, mainStarter and runMinimaCMD embedding API |
| Lifecycle | src/org/minima/system/Main.java | Database startup, keys, miner/processor/network ownership, recurring maintenance and notifications |
| Concurrency | src/org/minima/utils/messages/MessageProcessor.java | Named worker threads with message queues and timer delivery |
| Persistence | src/org/minima/database/MinimaDB.java | Owns wallet, TxPoW DB, tree, cascade, archive, preferences, custom transactions and MegaMMR; read/write locking |
| Commands | src/org/minima/system/commands/CommandRunner.java | Registry, parameter parsing, JSON replies, sequential multi-command execution, administrator checks |
| Networking | src/org/minima/system/network/NetworkManager.java; minima/NIOMessage.java beneath the same network directory | NIO transport, P2P/P2P2, optional RPC, transaction admission, relay and sync messages |
| Consensus | src/org/minima/system/brains/TxPoWChecker.java; TxPoWProcessor.java | Basic/signature/MMR/script checks, block validation, tree processing, cascading and archiving |
| Submission | src/org/minima/system/commands/txn/txnpost.java; send/send.java beneath commands | Custom transaction posting; send proof/signing/mining call sites |
| Signing | src/org/minima/database/wallet/Wallet.java:793 | Synchronized key lookup, use counter, TreeKey signing and persisted use update |
| Contracts | src/org/minima/kissvm/Contract.java | Tokenize, parse statements, execute with transaction/witness/state context and resource limits |
| Full UTXO state | src/org/minima/objects/mmr/MegaMMR.java | Apply spent inputs and new outputs to the MMR and unspent-coin index |

Upstream README explicitly describes a core-only node without MDS or Maxima. Legacy names/comments remain in some source files; they do not establish that those subsystems are present.

## Traced behavior

1. Minima.main configures the node; Main loads databases and starts workers/networking. Embedding calls runMinimaCMD, which delegates to CommandRunner.
2. CommandRunner validates parameter names, assigns the caller identity and checks its write-command list. Caller 0x00 is administrator. Multi-command execution stops at the first failed result; it is not an atomic batch.
3. txnpost.postTxn builds a TxPoW from a stored transaction and witness, optionally refreshes proofs/scripts, checks size, then mines synchronously or asynchronously. Its return is not confirmation of chain inclusion (txnpost.java:101 onward).
4. Incoming MSG_TXPOW checks chain ID, basic validity, signatures and state size. Additional script, MMR, mempool and relay checks determine forwarding; some branch/context-dependent failures can still proceed to local processing. Monotonic script failures are rejected (NIOMessage.java:518 onward).
5. TxPoWProcessor waits for a known parent and all referenced transactions, validates the block, builds its TxBlock/MMR node and recalculates the tree (TxPoWProcessor.java:184). Recalculation updates main-chain flags; old nodes move to archive/cascade and, when enabled, MegaMMR (line 527).
6. TxPoWChecker checks proof validity and runs input scripts plus nontrivial token scripts. KISS execution limits are set from block Magic parameters, overriding Contract's default instruction limit (TxPoWChecker.java:500).

## Newly fetched changes

Compared with previously fetched a8b34bc:
- Wallet.signData is synchronized and checks the cached base seed directly.
- txnauto was added to the administrator-restricted command list.
- FastByteArrayStream's out-of-memory branch now requests GC and throws IllegalArgumentException instead of halting the JVM.
- Version and bundled JARs updated.

## Build and test observations

build.gradle uses Java/application/Shadow/Docker plugins, H2 2.4.240, Bouncy Castle 1.69, MySQL connector 8.0.26 and JUnit 4.13. Wrapper is Gradle 6.7.1. The build declares Java 11 source compatibility at project level but overrides compileJava source/target to 1.8; it still uses jcenter(). README's ./gardlew spelling is a typo.

The test tree includes KISS VM contracts/functions/parsing, core objects, parameters and P2P utilities. ContractTests contains active assertions. Both test bodies in test/org/minima/system/brains/TxPoWProcessorTest.java are commented out, so their presence does not establish processor coverage.

This was source study, not an exhaustive audit. No build, test suite or live node was run. No source implementation was changed. Worktree HEAD was verified equal to origin/main and its status was clean.
