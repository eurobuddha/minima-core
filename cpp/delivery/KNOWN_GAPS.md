# Known gaps and acceptance limits

A successful build, a runnable node and passing tests are different claims.
The delivery must state which tests actually completed on its exact source candidate.

## Source mapping

456 of 457 mapped Java paths have native counterparts. Of these, 455 are active
partial implementations and one preserves inactive source. The remaining file is
`src/org/minima/utils/jni/jnifunctions.cpp`. The inspected repository does not provide
the external native method bodies or sufficient contracts to reproduce them without
inventing behavior. The existing JNI command caller tests cover available dispatch
and error behavior, not the unavailable native mining implementation.

## Behavioral parity

Full class/API parity remains `not_verified` throughout the mapping. Existing
validation includes original Java test methods, cross-language output/wire checks,
state/ownership tests, concurrency/error tests, and native node workflows. Each
suite states its limits. Passing a finite corpus does not certify all Java virtual
dispatch, object aliasing, exceptional paths, runtime/provider behavior or races.

The current entry-point audit leaves twelve candidates: TxPoWMiner, EncryptDecrypt,
CryptoPackage, PasswordCrypto, SignVerify, MMR, MegaMMR, TreeKey, ArchiveManager,
Cascade, TxPoWOnChainDB and TxPoWSqlDB. These classes already have native bodies,
but their source demonstration entry points remain missing. See the latest
validation candidate list for source paths and dependency details.

## Platform and delivery

The executable targets Apple Silicon macOS 26+. It is locally ad-hoc signed, not
notarized for public distribution. Other operating systems/architectures require
build and runtime validation. MDS/MiniDapp web UI is outside this minima-core console.

The database dependency is an ahead-of-time native build of Java H2/MySQL, not a
handwritten C++ database replacement. Its source/toolchain information is included.
The original port scope is still incomplete if it requires every dependency and
external JNI body to be implemented entirely in C++.

## Acceptance questions for the team

Is the exact delivery source build reproducible? Which registered tests passed,
failed, or remained unrun? Does the extracted runtime work without development
libraries? Can a new isolated wallet join the designated peer and reopen its saved
chain? Which source API behaviors are still unsupported? Keep these answers
separate in the release decision.
