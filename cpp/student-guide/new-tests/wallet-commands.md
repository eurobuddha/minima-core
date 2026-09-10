# Lab: connecting commands to the real wallet implementation

**Status:** 675 programs pass in normal and ASAN/UBSAN builds. All thirteen selected regression suites pass in each build. The full native node and native H2 storage remain incomplete.

## Four responsibilities

| Java file and its native counterpart | Responsibility |
| --- | --- |
| [scripts.java](../../../upstream-study/src/org/minima/system/commands/scripts/scripts.java) → [scripts.cpp](../../src/org/minima/system/commands/scripts/scripts.cpp) | List wallet script rows, or find the row for one address. |
| [newscript.java](../../../upstream-study/src/org/minima/system/commands/scripts/newscript.java) → [newscript.cpp](../../src/org/minima/system/commands/scripts/newscript.cpp) | Optionally clean a script, derive its address, remove an existing entry and add the new row. |
| [removescript.java](../../../upstream-study/src/org/minima/system/commands/scripts/removescript.java) → [removescript.cpp](../../src/org/minima/system/commands/scripts/removescript.cpp) | Convert the address parameter and ask the wallet to remove its row. |
| [sign.java](../../../upstream-study/src/org/minima/system/commands/signatures/sign.java) → [sign.cpp](../../src/org/minima/system/commands/signatures/sign.cpp) | Parse data and public key, ask the wallet to sign, then serialize the signature. |

Each command implements metadata, valid parameter names, its operation and a fresh-instance factory. All four are in the native CommandRunner registry. Help strings retain the pinned source text, including its unusual carriage-return prefix in removescript help; metadata tests compare them exactly.

## Reuse before adding code

The native Wallet already owns the relevant SQL statement sequence, script cache and TreeKey operations. The command translations call those methods through the existing MinimaDB owner. They reuse Command's parameter rules, Address derivation, Contract.cleanScript, ScriptRow JSON and Signature serialization. No alternate wallet or storage model is introduced.

For testing, [WalletCommandsOracle.java](../../support/WalletCommandsOracle.java) executes the pinned Java command, MinimaDB and Wallet bodies. [wallet_commands.cpp](../../support/wallet_commands.cpp) executes their native counterparts. Both reuse the earlier [recording JDBC oracle](../../support/SqlDBOracle.java) and [native recording driver](../../support/SqlRecordingFixture.hpp). This driver records statement calls and returns supplied rows. It does not execute or persist SQL.

## Trace replacement without assuming a transaction

Consider newscript with a script already present. The command derives its address, performs a lookup, removes the existing row, and calls addScript. These are separate wallet calls; there is no rollback in this command.

The fixture injects a failure at successive SQL calls. If insertion throws a checked SQLException, Wallet.addScript logs it and returns null. The command then fails when it tries to convert that missing row to JSON. The removal has already been requested. A translation that silently returns success or restores the old entry would change the source behavior.

There is another subtlety: Wallet.removeScript does not remove entries from its in-memory relevance caches. The experiment queries cache membership after each operation, including the address derived from RETURN TRUE. It can therefore detect an incorrect cache change even when returned JSON looks correct.

## Listing and removal have different failure paths

scripts without an address asks for all rows and converts the returned list to a JSON array. A checked SQL failure partway through collecting rows can leave a partial list. Searching one missing row instead raises CommandException. A supplied 0x address is not canonicalized on this command's direct lookup path; Mx input is converted. This differs from removescript, which uses the shared address parameter helper.

removescript returns Script removed after its wallet call. Wallet catches checked SQL failures internally, so the command can return that text after a logged SQL failure. This reply alone does not prove that a row was removed. The test preserves the source behavior and makes its limit visible.

## Signing has an observable write after the cryptography

sign parses data before publickey, so missing or invalid data wins when both are wrong. Wallet reconstructs a TreeKey from the supplied query row, signs the data, updates the stored use count, and returns the signature. The command serializes those signature bytes.

The fixture uses only synthetic keys and tests use counts 0, 1, 3 and 4, a locked seed, missing key rows and failures around the use update. It compares the actual signature bytes and requested SQL writes. A successful comparison does not prove crash-safe use-count persistence: the recording driver does not commit anything.

## Three discrepancies found along the way

1. **SQL runtime exception identity:** the first corpus stopped at program 8 of 626. Java exposed IllegalStateException, but the native generic adapter reported RuntimeException. NativeThrowable now delegates SQL exception text to the existing sqlExceptionText helper. The separate command-runner error corpus was expanded from 896 to 1,120 cases to include checked SQL and SQL runtime failures.
2. **Missing script-cleaning logs:** after that fix, an expanded run reached program 636 of 637. A malformed quoted script made Java log the parse error and return the original input. Native cleaning returned the input but wrote directly to a stream. Both cleaning token logs and cleaning-error logs now call the actual MinimaLogger. Separate clean operations exercise logging enabled and disabled.
3. **Parameter type errors:** the next run reached program 638 of 667 and found that a present null script caused native bad_variant_access rather than Java NullPointerException. Command.getParam now delegates to its existing UTF-16 accessor. This preserves missing, null, wrong-type and blank distinctions instead of duplicating the checks in every new command.

Those runs stopped at the first mismatch. Their program counts are not counts of all failing cases. The baseline reports are retained in the normal build directory with wallet-commands-baseline, wallet-commands-cleaning-baseline and wallet-commands-parameter-baseline prefixes.

## What the final experiment observes

The [driver](../../tools/wallet_commands_differential.py) runs **675 programs containing 1,907 operations**, comparing **50,275 SQL calls** per runtime. It includes:

- Full command metadata and fresh replies.
- Valid lookup, replacement, removal and signing paths.
- Checked and runtime failures at the first 31 SQL-call positions, plus no injected failure.
- Empty, duplicate and altered query rows; null result sets and commands before wallet loading.
- Admin/non-admin dispatch, invalid parameter gates and stopping within a batch.
- Exact lowercase true versus other Boolean strings, script cleaning, missing/blank/null/non-string parameters and malformed addresses.
- Locked signing, key-use boundaries, actual signature bytes, log bodies and selected cache membership.

Only Java-specific null/class-cast diagnostic expressions, generated stack-frame lines and logger time/memory prefixes are excluded. The test compares the remaining replies, exception categories, logs and SQL sequences. All thirteen selected regression suites pass in both builds, including the existing Wallet, VM, HTTP handler, command and notification comparisons. The complete selections took 36.37 seconds normal and 151.68 seconds with ASAN/UBSAN.

## Run it and read the limits

After a successful build, run from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^wallet_commands_differential$'
```

Read cpp/build/wallet-commands-results.json for program/operation counts and the first discrepancy, if any. The sanitizer build writes its own report.

Supplied query rows are intentionally independent of earlier recorded writes. Thus a later scripts call demonstrates lookup and rendering, not a persisted replacement. Full Main startup, H2/MVStore durability, concurrent wallet shutdown, arbitrary malformed row fields, native/JVM diagnostic identity and every public API boundary still require work. No live wallet, funds or private credentials are accessed by these fixtures.

## Questions for students

1. Why can Script removed accompany a failed SQL call? **Answer:** the wallet catches that checked failure and returns to the command, which then creates its normal reply.
2. Why does clean:true affect an address? **Answer:** Address hashes the cleaned script text; changing that input can change the derived key.
3. What does comparing signing-use SQL prove? **Answer:** that the same update was requested at the same point. It does not prove durable storage.
4. Why should null and absent parameters be separate cases? **Answer:** defaults apply to absence; a present null fails when Java accesses it as a string.
5. Why keep earlier passing operations in a failure report? **Answer:** they reveal effects already performed before the failure and expose an incorrect rollback or execution order.
