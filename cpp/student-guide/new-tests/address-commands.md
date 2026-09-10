# Lab: address creation, notification state and saved preferences

**Status:** native implementations compile in normal and ASAN/UBSAN builds. The expanded 134-program comparison and all six selected regression suites pass in both builds. Full node startup and native H2 remain incomplete.

## The source files

| Source → native file | Responsibility |
| --- | --- |
| [newaddress.java](../../../upstream-study/src/org/minima/system/commands/base/newaddress.java) → [newaddress.cpp](../../src/org/minima/system/commands/base/newaddress.cpp) | Create a nondefault simple address, then obtain the wallet's key count for the reply. |
| [getaddress.java](../../../upstream-study/src/org/minima/system/commands/base/getaddress.java) → [getaddress.cpp](../../src/org/minima/system/commands/base/getaddress.cpp) | Select an existing default address. |
| [coinnotify.java](../../../upstream-study/src/org/minima/system/commands/base/coinnotify.java) → [coinnotify.cpp](../../src/org/minima/system/commands/base/coinnotify.cpp) | Add, remove or check an address in the volatile notification set. |
| [slavenode.java](../../../upstream-study/src/org/minima/system/commands/base/slavenode.java) → [slavenode.cpp](../../src/org/minima/system/commands/base/slavenode.cpp) | Update and report the saved slave-node preference. |

The translations reuse the native Wallet, MinimaDB, UserDB, Command and ScriptRow implementations. They retain the source's metadata and fresh-instance factories. They do not introduce a second wallet, a different key scheme or a connection to a master node.

## Why creation must come before counting

newaddress first calls createNewSimpleAddress(false). Wallet creates a default-sized TreeKey, inserts its key record, derives RETURN SIGNEDBY(publickey), then inserts a tracked simple script whose default flag is false. The command subsequently queries all key rows, takes their count and converts the new script row to JSON.

That sequence can produce partial effects. A later query failure cannot undo a key or script already inserted. If the query throws a checked SQL failure, Wallet catches it and may return an empty list; the command can then report total zero alongside its new address. If script insertion returned null, the count query still precedes the command's attempt to convert that row to JSON. The native code follows this order rather than calculating an assumed previous count plus one.

The fixture exercises real default-sized key generation. It injects checked failures at key insertion, script insertion and the subsequent query, and selected runtime failures. It also tries locked and unloaded wallets. An explicit success guard requires a successful newaddress reply with simple=true, default=false, track=true, total=1 and a 32-byte public key. This catches a fixture that merely fails in the same way in both languages.

The supplied SQL query still returns fixture rows independently of earlier writes. The total=1 guard verifies the command's handling of that query; it does not prove a persisted database now contains exactly one key.

## A direct method call can bypass a dispatch rule

getaddress advertises an empty valid-parameter list. The runner therefore rejects any supplied createall or type option before runCommand is entered.

A direct call to runCommand can still receive those raw parameters. If createall exists—even with null or false as its value—the source takes an empty branch whose former body is commented out, returning only its normal envelope. Otherwise it reads type with a single default but never uses the value, then retrieves an existing default address. A blank or non-string type can therefore fail even though the variable is unused.

The tests separately exercise direct calls and dispatched commands. They do not re-enable the commented key-consistency check or implement new key creation in the empty branch.

Wallet's selection is random. Exact comparisons use no rows, one row or identical duplicate rows so the expected observation is well-defined. These cases do not establish the random distribution or independent selection among distinct candidates.

## Notifications are registration, not delivery

coinnotify parses action first and the address second, then chooses add, remove or check. Address conversion therefore occurs before an unknown action is rejected. Add returns the normalized address without a found field; remove and check include that Boolean.

Repeated add calls do not create duplicate set entries. The test also adds 0x01, checks and removes it using the Java-produced Mx200104JMFSAS representation, then checks the hex form again. A guard requires true, true, false for those three found observations. The lowercase Mx input is tested separately.

The database-owner recreation case clears the registration. No script is added and no preference file is saved. The test does not mine a coin or prove NOTIFYCOIN delivery; that belongs to the chain-processing integration.

## Preferences can change without being saved

slavenode obtains UserDB. If enable is present, it adds RESTART REQUIRED to the response. Only the exact trimmed string true enables the preference and requires a host; every other nonblank string disables it and stores an empty host. The disabled branch ignores a supplied malformed host value.

The command calls saveUserDB even for a query with no enable option, then reads the enabled flag and host into its response. It does not initiate a network connection or change all live runtime settings.

The experiment uses actual temporary userprefs.db files. It compares exact serialized bytes after each operation and exercises saving disabled through MinimaDB.setAllowSaveState(false). In that mode a command can change memory while the earlier file remains intact. Reloading the file restores the earlier saved setting. Re-enabling saving and issuing a query saves the current setting. Recreating the database owner and reloading the file checks the serialized preference path; it is not a full node restart test.

## Test construction and evidence

The existing [WalletCommandsOracle.java](../../support/WalletCommandsOracle.java), [native harness](../../support/wallet_commands.cpp) and [driver](../../tools/wallet_commands_differential.py) are extended with an addresses group. Each group gets separate Java compilation and report paths, so the two CTest entries can run concurrently. Each program resets its synthetic database owner and temporary preference file; operations within that program retain state.

The comparison records returned JSON or escaped exceptions, ordered SQL calls, logs, selected wallet/notification membership, current preference values and saved bytes. It removes Java-specific null/class-cast diagnostic expressions, generated frames and logger time/memory prefixes. The expanded comparison passes **134 programs, 276 operations and 3,693 SQL calls** in each build. All six selected suites pass: mapping, address commands, the earlier 675-program wallet-command group, 1,233 runner cases, 1,120 runner-error cases and 1,130 preference cases. The normal selection finished in 28.41 seconds; ASAN/UBSAN finished in 156.12 seconds. These are finite comparisons, not a proof of complete file parity.

After a successful build, run from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^wallet_address_commands_differential$'
```

Read cpp/build/wallet-address-commands-results.json. Use the sanitizer build directory for its independent report. The earlier [wallet-command lab](wallet-commands.md) explains the shared SQL recording boundary and the error corrections already made there.

## Limits

Temporary preference bytes and reloads establish the observed file behavior, not crash safety, filesystem-error recovery or a complete restart. SQL key/script writes remain recording-driver observations; native H2 storage is absent. Distinct-row random selection, arbitrary malformed row fields, concurrent lifecycle, full Unicode/exception semantics and Main integration remain under review. Fixtures use synthetic keys and never access a live wallet or connect to the configured host.

## Questions for students

1. Why can newaddress return total zero after creating a row? **Answer:** the subsequent query can fail with a checked SQL exception and return an empty list.
2. Why can direct getaddress createall:false suppress the response? **Answer:** the branch checks parameter presence, not its Boolean value.
3. Why is a notification registration lost after owner recreation while a slave-node setting can be recovered? **Answer:** notification addresses are held in memory; the preference has a serialized file that the fixture explicitly reloads.
4. Does RESTART REQUIRED prove that a restart took place? **Answer:** no; it is response text describing when the preference should take effect.
5. Why compare both memory and file bytes while saving is disabled? **Answer:** either observation alone would hide that they can intentionally differ.
