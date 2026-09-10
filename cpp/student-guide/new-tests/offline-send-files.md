# Lab: viewing, signing and posting transaction files

Written before the first tests for sendview, sendsign and sendpost. These files
carry a complete TxPoW, unlike the TxnRow format used by txnimport/txnview.
The translation reuses MiniFile, TxPoW, Wallet, ScriptProof, Witness, vault and
the existing miner receiver. Reusing a file reader does not make two wire formats
interchangeable.

## Follow a file through the commands

sendview resolves the filename, reads its bytes, converts a TxPoW and returns its
JSON. It does not test for a missing file first. Conversion can log a read error
and return null, followed by a null-receiver failure when JSON is requested.

sendsign reads and converts first, then optionally unlocks a locked wallet.
It visits main inputs followed by burn inputs, adds each script proof, and signs
the corresponding transaction ID. Its required-signature list is never populated:
the source therefore signs repeated input keys again. Witness.addSignature separately discards a second signature with the same root
public key. The distinction matters: two inputs using one key cause two signing
calls but leave one signature in that transaction’s witness. Skipping the second
signing call changes the recorded key-use writes even when final bytes match.
Relocking occurs on the normal path, not in a universal finally block. It writes
a new signedtransaction-TIMESTAMP.txn and returns the absolute filename.

sendpost builds its response JSON before calling the asynchronous miner. The
miner then modifies the TxPoW's body hash and queues its owner. Compare both the
returned snapshot and the queued object to detect accidental reordering.

## Experiments and observations

The existing wallet-command harness writes synthetic input files in a
per-program temporary directory. We compare metadata, exact file bytes,
wallet SQL calls, signature verification, miner queues and failures. Filenames
created from independent wall clocks are checked for their required shape
and normalized only after their contents have been inspected.

Include repeated inputs and both main/burn signing, missing script rows, password
failure and failure after unlocking, truncated input files, missing paths and
wrong parameter types. Observe directory creation even when opening a file fails.
Also exercise the shared writer directly: append versus replace, empty versus
nonempty directories, a file used as a parent, and a NUL in a filename. Java
creates a file before opening its output stream; those steps can throw different
exception classes. Exact I/O errors need their own observations: the older
utility suite's generic ERROR result cannot distinguish exception categories.

## The small demonstration entry point

The source also has a main method that ignores its arguments and prints MiniData
encodings for integers 0 through 511. Reuse the existing BigInt constructor and
MiniData conversion, then capture actual stdout from each implementation. Compare
all 512 lines, including the byte-width boundary at 256; do not treat a successful
compilation as proof of this separate method. Test both null and empty arguments.

## Evidence and limits

The final normal and ASAN/UBSAN comparisons each pass 201 programs, 558
operations and 9,773 recorded SQL calls (23.32s and 97.59s respectively). Each
build also passes the 1,361-case utility suite. The two demo programs compare all
512 lines each. All twelve selected checks pass in both builds: the normal batch
took 133.68s and the sanitizer batch 515.77s. The longer sanitizer vault check
passed in 515.76s. These are selected checks, not the entire 167-entry inventory.

The first observed production mismatch was a missing file: Java raised
FileNotFoundException but native code raised a generic stream runtime error.
A later output-parent case showed Java IOException after key-use writes where
native code again returned a generic error. Both baseline reports are retained
in the normal build directory. MiniFile now reuses the existing Unix path helpers
and follows create/open/write ordering with typed I/O exceptions.

The first signature-count assertion also failed against Java itself. That was a
test-design error, corrected by reading Witness.addSignature: repeated signing
and retained signature counts are different observations. A differential test
needs independent expectations, and those expectations can be wrong too.

Fixture SQL rows do not prove durable native storage. Main startup, network
acceptance, arbitrary stream races, all reference identities and complete platform
filesystem equivalence remain unverified. The synthetic TxPoWs isolate these
commands; they are not evidence of accepted network transactions.


## Reproduce the experiment

Build the existing native harness, then run the comparison:

```sh
cmake --build cpp/build --target wallet_commands
ctest --test-dir cpp/build -R '^offline_send_commands_differential$' --output-on-failure
```

Use cpp/build-sanitized for the instrumented build. The driver writes
`offline-send-commands-results.json` in the selected build directory. Read its
scope field along with its passed field.

- [Input corpus and independent expectations](../../tools/offline_send_corpus.py)
- [Comparison driver](../../tools/wallet_commands_differential.py)
- [Java observer](../../support/WalletCommandsOracle.java)
- [C++ observer](../../support/wallet_commands.cpp)
- [Shared file implementation](../../src/org/minima/utils/MiniFile.cpp)
