# Lab: recover a real node using MegaMMR

Written before running the new integration test. The previous command fixtures
checked many branches with deliberately small synthetic objects. This lab starts
the actual Java and C++ node programs, creates eight wallet keys, and saves a valid
solo genesis chain. Run both programs with `-megammr`.

Export the same saved chain from each runtime. Compare the complete backup bytes
and export actual owned coin proofs. A proof links a coin to the chain's Merkle
mountain range; matching JSON balances alone cannot establish that the proof works.
The saved-data fixture must load this real chain without replacing its tree or
MegaMMR. Call the production wallet-detail and proof-serving helpers, compare both
encodings, and check the exported file with the file-overload integrity helper.

For import, copy the baseline wallet and first set its key-use counters to 100.
Import the other runtime's MegaMMR file. The command resets chain storage but
retains the wallet, so all eight counters should still be 100 after reopening
with the other runtime. Compare chain identity, coins and proof bytes, then sign
data, reject a modified message, and check that only the signing key reaches 101.

For resync, reuse the existing loopback responder. Supply an empty archive reply
to the initial connection check, then a MegaMMRIBD containing the real saved chain
and its actual coin proofs. Compare both request frames, including the wallet's
ordered tracked addresses and public keys. Require command-driven clean shutdown,
the received proof count, and the peer file written after the other databases.
Reopen with the opposite runtime and repeat the wallet, chain and signing checks.

Both commands ignore nested coinimport failure replies in the pinned source.
A successful outer reply or a received-proof count therefore is not sufficient:
we independently inspect persisted coins and re-export their proofs. This genesis
experiment still does not demonstrate new-height advancement, cascades, recovery
of a pruned coin, full inbound network serving, or complete node parity.

Reuse `native_restore_startup.Node/ready/key_state/chain`,
`native_archive_startup.coins`, and `peers_checker_differential.Runner/Responder`.
Run normal and ASAN/UBSAN configurations and retain the reports. The native H2
AOT library is outside C++ sanitizer instrumentation.

Observed results: both full builds pass all four workflows, using ten real node
processes each. Java and C++ write identical 903-byte exports within each run;
the independent runs have different wallet seeds and genesis timestamps, so their
hashes differ. Each wallet owns one genesis coin. The serving helper returns its
actual proof in a 1069-byte response frame, and the file integrity helper succeeds.
Normal CTest took 29.52 seconds; ASAN/UBSAN took 117.13 seconds. Every workflow
preserves the chain, proof and eight key counters, then signs and rejects modified
data. Reports are retained in [saved evidence](../../validation/mega-runtime-work-in-progress).
