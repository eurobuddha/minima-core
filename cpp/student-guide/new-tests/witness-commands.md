# Lab: adding witness data without hiding partial changes

**Verification:** the 418-program witness comparison passes normal and ASAN/UBSAN, covering 1,128 operations and 8,637 SQL calls. All nine selected suites pass: normal in 44.49 seconds and ASAN/UBSAN in 182.20 seconds. Both full native builds also pass. Full node startup and native H2 remain incomplete.

Read [txnmmr.java](../../../upstream-study/src/org/minima/system/commands/txn/txnmmr.java) beside [txnmmr.cpp](../../src/org/minima/system/commands/txn/txnmmr.cpp), then [txnscript.java](../../../upstream-study/src/org/minima/system/commands/txn/txnscript.java) beside [txnscript.cpp](../../src/org/minima/system/commands/txn/txnscript.cpp).

## What a witness supplies

A transaction describes inputs, outputs and state. Its witness supplies signatures, coin membership proofs and scripts. These two commands append parts of that witness. Successful insertion alone does not validate a transaction or spend its coins.

The translation reuses the existing TxnDB, Wallet, MMR, ScriptProof and Witness implementations. Java fixture setup attaches a real custom transaction database by the same field setup as the earlier transaction oracle. Native setup uses the existing TxnDB runtime lookup. Neither MinimaDB constructor initializes that component on its own. This fixture supplies a valid command environment; it does not exercise the missing Main startup sequence.

## Count is not identity

Suppose inputs are A, B and C, while the witness already has one proof for C. txnmmr counts one existing proof and skips input A. It then appends proofs for B and C. It does not inspect which coin that first proof describes.

The chosen historical block is the later of tip minus the proof-history allowance and all input creation heights. Newly added proofs use that one node. Existing proof timestamps and contents are untouched. A missing historical node only fails when the loop needs to dereference it: with enough existing proofs, the command can still return successfully. A missing tip fails earlier even for an empty transaction.

The tests vary proof counts, input order, duplicates, entry numbers, creation heights, history length, missing nodes and repeated commands. Java-produced fixtures include a twelve-node MMR chain. The comparison checks full transaction-database bytes after every operation, plus the cached monotonic flag. Explicit guards prevent two implementations that always fail from satisfying the test.

## A later bad script does not undo the first

In manual mode, txnscript visits the pinned JSONObject's LinkedHashMap insertion order. A blank proof string requests a plain ScriptProof. Nonblank text is decoded as MMRProof bytes. Each successful script is immediately offered to Witness.addScript, which suppresses duplicate addresses.

For example, insert RETURN 7 with a blank proof, followed by RETURN TRUE with truncated proof bytes. The second entry fails, but the first script remains. The fixture compares that retained state, not merely the error response. Supplied script text is hashed without Contract.cleanScript; whitespace and case can matter to the resulting address.

In auto mode, each input whose address lacks a witness script triggers a wallet lookup. A missing ScriptRow is skipped. Checked SQL failures are logged and swallowed by Wallet; selected runtime failures escape. The fixture records exact SQL request order and logs. The driver supplies query rows independently of prior writes, so this is not a database durability test.

## Two discrepancies, two different fixes

The first recorded mismatch was case 123 of the initial 392 programs. Both decoders rejected empty proof bytes, but the native test harness labelled its EOF exception as a generic runtime error. Reusing nativeExceptionText corrected the harness's observation. The retained baseline is cpp/build/wallet-witness-commands-io-baseline-results.json.

The next recorded mismatch was case 345 of the expanded 394 programs. A Boolean scripts parameter produced Java ClassCastException but native bad_variant_access. Command.getJSONObjectParam now reuses JavaClassCastException while preserving missing/default/null and shared-object behavior. Additional cases place malformed scripts beside auto:true and a missing transaction to verify the source's cast-before-branch and cast-before-lookup order. The baseline is cpp/build/wallet-witness-commands-object-baseline-results.json.

Initial setup attempts also exposed absent transaction databases in both fixtures, including a sanitizer null dereference in the native snapshot. The existing transaction-test setup was reused before proceeding. These setup failures were not passing command comparisons.

## Running the experiment

After a successful build:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^wallet_witness_commands_differential$'
```

The [corpus](../../tools/witness_commands_corpus.py) extends the [shared driver](../../tools/wallet_commands_differential.py), [Java oracle](../../support/WalletCommandsOracle.java) and [native harness](../../support/wallet_commands.cpp). Reports are written to cpp/build/wallet-witness-commands-results.json and the corresponding sanitizer directory. Each group has separate Java compilation and report paths.

## Limits and questions

The exact comparison excludes generated frames, logger prefixes and Java-specific null/class-cast diagnostic expressions. It does not certify arbitrary malformed object graphs, filesystem recovery, concurrency or full node integration. The initial review found value-copy coin ownership. The later [coin ownership lab](coin-ownership.md) corrects that shared-field behavior and adds 220 mutation programs. This witness-command corpus itself does not exercise those later mutations; MMRProof identity and full runtime ownership remain incomplete.

1. Does having three proofs for three inputs establish matching proof identities? **No.** This command uses positional counts, not identity validation.
2. Why can auto:true still reject a malformed scripts parameter? **The JSONObject cast happens before the auto branch.** Null is castable and is not dereferenced in that branch; a Boolean is not castable.
3. Why record state after a failed command? **Earlier additions can remain and affect the next command.**
4. Does appending a script prove that it returns true? **No.** This command constructs a proof; execution and transaction validation are separate operations.
