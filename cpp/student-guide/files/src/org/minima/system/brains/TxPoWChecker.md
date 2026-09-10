# TxPoWChecker — logic, translation and tests

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/brains/TxPoWChecker.java](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java) | [src/org/minima/system/brains/TxPoWChecker.cpp](../../../../../../../src/org/minima/system/brains/TxPoWChecker.cpp) |

## Teacher's worked explanation

TxPoWChecker decides whether a proposed transaction or block satisfies the pinned
Minima rules. Think of it as a sequence of gates. Each gate assumes particular
earlier work has been done, and a failure stops the sequence. The order is part of
the behavior: a later invalid field must not hide an earlier rejection or change
which cached values have already been written.

### Understand the validation stages

**Basic validation** checks the network chain ID, gathers proof coin IDs from the
main and burn witnesses, and rejects duplicates. It then validates the main
transaction's amounts and proof relationships, checks its zero link hash, and
checks a nonempty burn transaction's link to the main transaction ID.

The private transaction check compares input and proof counts, rejects repeated
proof coins, and compares amount, address and token ID for each pair. Ordinary
input coin IDs must match their proof; the special ELTOO input ID follows the
source exception to that comparison. Token definitions must agree with token IDs,
spent coins cannot be spent again, and required scripts must exist. Outputs have
additional token-definition checks. These checks do not run the scripts or verify
signatures themselves.

**Signature validation** uses each transaction's cached ID as the signed message.
It obtains the root public key from each signature and verifies through TreeKey.
Changing the message to the overall TxPoW ID would be a different protocol, even
though both values are hashes.

**MMR validation** reconstructs the leaf data for each proof coin and asks the
tip MMR whether the proof is valid at its stated time. MMR means Merkle Mountain
Range, the authenticated structure used here for coin proofs. The `log` argument
controls the invalid-proof rejection message. A null MMR is only dereferenced if
an actual proof needs checking; an empty witness need not touch it.

**Script validation** checks the address script for every input and any additional
token script. It supplies transaction/witness data, previous state and block
globals to a Contract. A source-trimmed token script equal to `RETURN TRUE` can
skip the additional execution. The transaction's monotonic and validity flags
record whether future checks may reuse the result. A cached monotonic result is
returned without rerunning the script.

**Simple validation** assumes basic and signature checks have already occurred.
It checks difficulty, serialized size excluding block transaction IDs, stored
state size, MMR proofs, and finally scripts. This ordering explains why a null
MMR can still produce an earlier ordinary difficulty rejection.

### Now follow a whole block

The block path first checks header rules: chain ID, height after the parent,
superparents, the permitted time window relative to a median block, expected
difficulty, and updated magic parameters. It rejects an excessive count of
included transactions.

Next it gathers **all** eligible proof coin IDs, including the main block's
transaction and its listed transactions, before testing uniqueness. Suppose one
transaction contains a duplicate and a later list entry is null. Returning at the
first duplicate would skip the later null access; Java's gather-then-check order
does not. The port retains that order.

After simple-checking the relevant transactions against the parent MMR, it
constructs the resulting TxBlock/tree-node MMR and compares both the root hash
and total value with the header. An empty reconstructed root is a null-pointer
failure in Java. Ordinary Exceptions are logged by the block catch and return
false; Errors and failures in the catch's own logger escape.

The file also provides a timed wrapper, a header-only block check, mempool-coin
lookup, and superparent checking. Mempool and cascade queries enter through
native service bindings; controlled fixture bindings do not establish full
MinimaDB/Main startup.

### How the port and tests fit together

The C++ stages reuse Transaction, TreeKey, Contract, MMR, TxBlock and tree-node
implementations rather than implementing alternate validators inside the checker.
Shared-pointer overloads preserve original transaction objects; synchronous
value adapters borrow only for the call's duration. A nullable-MMR overload
preserves source access order without constructing a C++ reference to null.

The latest logger work reuses MinimaLogger and typed native exceptions. The
basic-check duplicate diagnostics enumerate Java HashSet order using the tested
JavaHashMap helper. The original list is logged separately. This matters because
logs can be observed and because a logger can throw partway through the list.

`checker_logs_differential.py` runs actual Java/C++ stages on identical
Java-produced wire objects and compares result or exception, ordered log bodies,
monotonic/validity flags and cached transaction ID. Its 100 fixtures generate
1,343 comparisons; the latest normal run has zero failures. Cases include missing
proofs/scripts, wrong chain/link/coin/token data, invalid amounts, spent coins,
token scripts, header-only TxPoW, empty signatures, failed signatures, duplicate
set allocation boundaries, null/empty MMRs and injected logger failures.

`consensus_differential.py` additionally varies relay limits, scripts, block
generation/mempool selection, chain time, difficulty and superparents. Its latest
normal run passes 2,890 comparisons. `timed_checker_errors_differential.py` adds
105 block/worker failure comparisons. Block-ownership and processor suites test
these methods in larger operation sequences. Their counts overlap conceptually:
do not add them together and call the sum “all possible consensus cases.”

### What remains unfinished

The comparisons exclude automatic stack frames and logger prefixes; generated
Java NPE expressions are normalized while their categories remain compared.
Complete nullable object graphs, collection aliases, concurrent mutations,
thread interruption and native node/database integration remain unverified or
unimplemented. A matching rejection corpus is stronger than a compiling file,
but it is not complete consensus or full-node equivalence.

**Exercise:** a transaction has an invalid amount, and the logger throws on its
first call. Predict whether the outer transaction JSON is created and whether
basic validation returns false or throws. Then repeat with the second log failing.


## Open the test evidence

- [checker_logs_differential.py](../../../../../../../tools/checker_logs_differential.py)
- [CheckerLogsOracle.java](../../../../../../../support/CheckerLogsOracle.java)
- [checker_logs.cpp](../../../../../../../support/checker_logs.cpp)
- [consensus_differential.py](../../../../../../../tools/consensus_differential.py)
- [timed_checker_errors_differential.py](../../../../../../../tools/timed_checker_errors_differential.py)
- [txblock_ownership_differential.py](../../../../../../../tools/txblock_ownership_differential.py)
- [txpow_processor_differential.py](../../../../../../../tools/txpow_processor_differential.py)

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [CascadeNode](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java), [TxPoWDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java), [TxPoWTreeNode](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [CoinProof](../../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java), [Magic](../../../../../../../../upstream-study/src/org/minima/objects/Magic.java), [ScriptProof](../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java), [Token](../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [TxBlock](../../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [TxHeader](../../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java), [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [Witness](../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Signature](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java), [TreeKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java), [MMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java), [MMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [RelayPolicy](../../../../../../../../upstream-study/src/org/minima/system/network/minima/RelayPolicy.java), [GeneralParams](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [GlobalParams](../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [checkTxPoWBlockTimed](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L47), [checkTxPoWBlock](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L59), [checkTxBlockOnly](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L194), [checkTxPoWBasic](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L256), [checkTxPoWBasic](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L323), [checkTxPoWSimple](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L453), [checkTxPoWScripts](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L488), [checkTxPoWScripts](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L500), [checkMMR](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L631), [checkMMR](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L635), [checkMMR](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L647), [checkSignatures](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L684), [checkSignatures](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L695), [checkMemPoolCoins](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L721), [checkParents](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java#L766).


Pinned Java SHA-256: `418f59781a21c8011707e8fb45220b8a1997c14cabec5f3f838907df92962bca`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
