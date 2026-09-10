# Transaction checking — why object identity matters

A custom transaction lives in a TxnRow with its witness. Java's TxPoWGenerator puts those same objects into a generated TxPoW. `txncheck` clears the stored transaction's cached validation flag after generation, then asks the checker to validate the generated object. These actions only refer to the same transaction if ownership is preserved.

Read the [pinned command](../../../upstream-study/src/org/minima/system/commands/txn/txncheck.java), [native command](../../src/org/minima/system/commands/txn/txncheck.cpp), [TxBody source](../../../upstream-study/src/org/minima/objects/TxBody.java), [native body](../../src/org/minima/objects/TxBody.cpp), and [generator](../../src/org/minima/system/brains/TxPoWGenerator.cpp).

## Follow the command

The command lists token IDs in first-seen order across inputs and then outputs. A token-creation ID is listed as Minima, but totals still use the transaction's existing summation methods. It reports counts, raw per-token amounts, outputs and the input/output difference as burn. Numeric amounts are strings; counts are JSON numbers.

It next generates a temporary TxPoW, clears the original transaction's monotonic-check flag, and runs basic checks, signature verification, MMR proof checks and scripts. Signature verification uses TreeKey against the transaction ID. MMR checking short-circuits when the number of inputs and proofs differ. Scripts still run independently of that result.

The source's `validtransaction` combines basic, MMR and script results. It does **not** include `allsignaturesvalid` in that conjunction. A RETURN TRUE script can therefore yield validtransaction=true while a separately reported signature is invalid. The test explicitly preserves that behavior. This command's report must not be treated as a substitute for the node's full validation sequence.

## Reuse owners instead of copying their values

TxnRow and TxBlock already use shared owners. TxBody now follows that pattern for its transaction, witness, burn transaction and burn witness. TxPoW exposes shared-owner setters/getters alongside its existing value convenience methods. The generator's shared-owner overload retains the supplied owners; its older value overload delegates using newly owned values. MinimaDB binds the existing consensus services to the current database.

Why does this matter? Imagine the stored transaction says its scripts were checked and failed. If generation copies it, clearing the stored flag leaves the generated copy's stale flag untouched. The checker could return a cached result instead of running the script. Sharing the owner makes the flag reset visible to both paths, and the new result visible in the original row.

This does not establish that every legacy value call site preserves Java identity. The full node still needs an ownership/API review, startup and native storage integration.

## In-place reading is also an identity operation

Java TxBody.readDataStream updates its four existing field objects. It does not allocate four replacement objects. The native reader now delegates to their existing in-place readers in wire order. PRNG and difficulty are read first, followed by transaction, witness, burn transaction and burn witness; the transaction-ID list is reset only after those reads succeed.

If transaction and burnTransaction point to the same object, the later burn read changes what both fields see. If a field is null or the input ends early, preceding changes remain visible. Tests retain owners and compare identity, consumed byte count, partial state and failure category. The list's full Java aliasing semantics remain outside this native vector API.

## Experiments and limits

The [corpus](../../tools/transaction_check_corpus.py) reuses [ConsensusOracle](../../support/ConsensusOracle.java), the existing wallet-command harnesses and [comparison driver](../../tools/wallet_commands_differential.py). Fixtures cover ordinary and floating inputs, invalid amounts and links, missing scripts and proofs, token scripts, valid/invalid signatures, repeated checks, body field aliases, null fields and truncated reads.

Both runtimes use actual generator/checker/signature code. Fixture block timestamps force the generator's existing clock clamp; the generated body's random PRNG is not part of the ownership snapshot. The experiment compares replies, log messages, stored transaction/witness bytes and all three validation-cache flags. SQL remains a recording receiver. The miner and full node are not started.

Both normal and AddressSanitizer/UndefinedBehaviorSanitizer builds pass all 396 programs and 1,049 operations. The sanitizer CTest run took 177.57 seconds. All 21 selected regressions also pass in both builds: consensus, objects, checker failures, timed ownership, TxBlock ownership, miner and processor behavior, MinimaDB, transaction/wallet commands, search and registry behavior. The two mapped INC/DEC tests add four passing CTest entries per build; together these are 26 selected checks per build, not the entire 148-entry suite. Initial comparisons exposed three production differences: a missing token creator leaked bad_optional_access from TxBlock, zero/truncated-to-zero hash rates leaked a native division error, and negative hash rates produced different MiniData error text. Each fix reuses the existing implementation and Java-style exception support. The Java harness also needed a local-variable rename and removal of an extra 0x prefix; neither was a production defect.

## Exercises

1. Why is a matching JSON reply insufficient here? A copied transaction might return the same reply while leaving the stored row's flags wrong; inspect the original owner too.
2. Why inspect the generated witness after clearing the row's witness? Clearing mutates the shared witness, so both holders must see the change. Deleting the row, however, must leave the generated object's retained owners alive.
3. What does a successful body read do when its main and burn fields alias? The same object is read twice, so both fields finally show the later read's contents.
4. Can validtransaction=true coexist with allsignaturesvalid=false? Yes: that is how the pinned command combines its results, although later validation may reject the transaction.
