# Lab: creating a transaction for offline signing

Written before the first sendnosign tests. This command supplies the file used
by sendview, sendsign and sendpost. Read its own source before assuming it is the
send command with a single flag switched off.

## Follow the source

Parse recipients, total, token, debug, burn, split and state. Walk back the
configured confirmation depth before searching for unspent simple coins. The
coinage helper defaults to zero, although coinage is absent from the permitted
parameter list: a direct invocation and the command runner can therefore differ.

Reuse send.selectCoins to choose inputs, reject mining/mempool reservations,
construct MMR/script proofs and calculate change. For each recipient, divide by
the original split value but emit only split.getAsInt() outputs. The source accepts
fractional splits in [1,20] and has no final rounding output. Preserve that behaviour
and test it; do not import send's newer rounding algorithm.

State values are cast to String. A numeric JSON state value is not silently
converted to text. Recipient outputs retain state, while change does not. The
command does not check the change address's public/private key consistency.

Construct a custom-token burn through createBurnTransaction with sign=false.
Generate the full TxPoW, calculate its ID, then write the specified filename or
unsignedtransaction-TIMESTAMP.txn. The permitted dryrun parameter is never read:
it does not prevent file output. No wallet signing or miner submission occurs.

## Planned experiment

Reuse the twelve-node real MMR fixture and its custom-token variants. Observe
metadata, direct/runner parameter gates, selected inputs, proofs, outputs, change,
SQL calls, logs and exact file bytes. Chain lengths around confirmation depth,
fractional splits, string versus numeric state, insufficient funds, reservations,
missing scripts, file failures and unsigned burns expose source-specific branches.

Run create, view, sign, verify and post in sequence with real native components.
Verify signatures against transaction IDs and check the miner's retained queue.
Distinguish these isolated operations from a live network's acceptance of the
transaction. Fixture SQL requests do not prove durable wallet storage.

New TxPoWs contain independent random PRNG bytes. Verify their length and locate
those bytes through the real wire layout; compare every remaining byte and decoded
field. Do not normalize signatures, proofs or arbitrary differences. Compare the
original file before any deliberate fixture replay used to align later mining.
Timestamp filenames need shape checks and narrowly scoped normalization too.

## Evidence

The final comparisons each pass 309 programs, 1,248 operations and 20,081 recorded
SQL calls: 28.74s normal and 94.27s under ASAN/UBSAN. All seven selected checks
pass per build, including the funding and prior offline-file regressions. The
initial seven-check runs failed only the incorrect zero-total assertion; the
corrected unsigned test was then rerun alone. Both full builds compile without
warnings. This is seven selected checks out of 168 registered, and isolated
command evidence rather than complete-node certification.


## Correcting an independent expectation

The first additional zero-amount assertion wrongly expected a file. Java and C++
actually both reject total zero with `Invalid Minima amount to send.. 0`, after
proof construction but before output construction. The retained normal-build
`unsigned-zero-amount-observations.json` records both results. A zero individual
recipient within a positive-total multi-send is a separate case; it must not be
confused with a zero transaction total. The expanded corpus tests both.


## Run and inspect

```sh
cmake --build cpp/build --target wallet_commands
ctest --test-dir cpp/build -R '^unsigned_send_command_differential$' --output-on-failure
```

Use cpp/build-sanitized for ASAN/UBSAN. The driver writes
`unsigned-send-command-results.json` in the chosen build directory.

- [Actual corpus and independent assertions](../../tools/unsigned_send_corpus.py)
- [Comparison driver](../../tools/wallet_commands_differential.py)
- [Java observer](../../support/WalletCommandsOracle.java)
- [C++ observer](../../support/wallet_commands.cpp)
- [Mapped command](../../src/org/minima/system/commands/send/sendnosign.cpp)
- [Reused recipient helpers](../../src/support/SendRecipients.hpp)
