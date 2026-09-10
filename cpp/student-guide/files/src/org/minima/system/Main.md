# Main — assembling the node and coordinating its lifetime

**Teaching status:** Authored explanation of the pinned Java body, native ownership adaptations, executed comparisons and remaining runtime validation.

**Port status:** `implemented_partial`; parity `not_verified`. Main now compiles; the complete working node is unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/Main.java](../../../../../../../upstream-study/src/org/minima/system/Main.java) | [src/org/minima/system/Main.cpp](../../../../../../src/org/minima/system/Main.cpp) |

## What this file does

Main extends MessageProcessor and owns the node's processor, miner, network manager, send-poll manager and notification manager. It publishes a singleton, exposes lifecycle flags, and dispatches periodic and event messages. The complete pinned Java body has been read for this lesson. The native class now assembles the existing component implementations and attaches their runtime callbacks. Successful full startup is still unverified.

## Construction is an ordered sequence of effects

The superclass establishes the MAIN message processor. Main records uptime and publishes itself before most services exist. It optionally loads the JNI library; a caught Error logs and exits. It creates the timer processor. In private mode it checks `databases/userprefs.db`: when that file exists and CLEAN is false it keeps the existing network; otherwise it sets CLEAN and GENESIS. If CLEAN is selected, it deletes the chosen data folder through MiniFile.

It then creates MinimaDB and loads all databases, applies optional MySQL configuration, and reads persisted slave-mode preferences. Those preferences can change the connection list and several network flags. It warms up hashing, removes the archive-restore folder, measures hash speed, stores the measurement, and attempts an initial batch of three default wallet keys. Key-creation Exception is logged without aborting the remaining constructor.

Next it creates NotifyManager, TxPoWProcessor and TxPoWMiner. Genesis creation, when requested, happens before NetworkManager construction. It clears invalid peers, creates the network and send-poll manager, derives the automine interval from `1000 / blockSpeed`, and schedules periodic tasks. Near the end it rejects slave mode with multiple comma-separated hosts by exiting, clears peer/IBD caches, and only then sets STARTUP_COMPLETE.

Publishing the singleton is therefore not the same event as completing startup. Any faithful native construction strategy must preserve observable ownership and readiness while preventing unsafe use of incomplete native objects.

## Genesis ties several objects together

`doGenesis` asks the wallet for a new simple address, creates GenesisTxPoW for that address, inserts it into TxPoWDB, creates a TxBlock with GenesisMMR, wraps it in a tree node, installs that node as the tree root, and marks the TxPoW as on the main chain. A matching genesis hash alone would not prove these database and ownership effects.

## Timers and message dispatch

`processMessage` immediately returns while shutting down or restoring. Otherwise its else-if chain selects one handler. Several handlers schedule their next occurrence **before** doing work or checking an early-return condition; a test must observe timer effects as well as the final result.

| Message family | Source behavior |
|---|---|
| TXPOWMINED | Ignore a result that is neither transaction nor block. Optionally log a found main-network block. Serialize a TXPOW NIO message with local UID `0x00` and post it through the incoming-message checks. |
| SYSTEMCLEAN | Schedule the next five-minute task, then request garbage collection. |
| CLEANDB_RAM | Reschedule for 30 minutes; clean RAM records, save state and refresh SQL. If a tree root exists, clear CoinDB entries older than root height minus 200 and reopen it. |
| CLEANDB_SQL | Reschedule for 12 hours; clean SQL TxPoW records and ask the archive to apply its cleanup policy. |
| AUTOBACKUP_MYSQL | Skip while importing, still rescheduling. Otherwise conditionally run archive and coin backup commands, inspect their status, log results/errors, and reschedule for two hours. |
| AUTOBACKUP_TXPOW | When importing is false and both relevant backup flags permit it, create a MySQL connection, initialize it, save the supplied TxPoW and shut the connection down. |
| DO_RESCUE | Select the message host or configured rescue host. With a host, mark keys initialized, run megammrsync, log its result and halt the process. With no host, log that omission. |
| PULSE | Reschedule first. Slave nodes return; other nodes create and broadcast a pulse. |
| NEWBLOCK / BALANCE / MINING | Build event data and forward NEWBLOCK, NEWBALANCE or MINING notifications. Mining data includes the supplied starting/stopping flag. |
| NETRESTART | Request all-peer disconnection, sleep ten seconds, reset the first-IBD timer, clear peer/IBD caches and call restartNIO. |
| AUTOBACKUP | Reschedule for 24 hours; run backup if configured, then recalculate and store hash speed even when backup was disabled. |
| NETRESET | Reset traffic statistics and reschedule for 24 hours. |
| SHUTDOWN | Call the shutdown lifecycle. |
| INIT_KEYS | Try batches of eight keys until initialization reports complete; failures are logged, and incomplete work is retried after ten seconds. |
| CHECKER | Reschedule for 180 seconds; return with a log if no tip exists, otherwise warn when the ID is unchanged, store the current ID and broadcast a ping. |
| P2PNETMDS_CHECKER | Reschedule for 30 minutes. With P2P and internet access, inspect tip age; an absent tip returns early. A tip older than 30 minutes or P2P queue larger than 50 can request a restart, avoiding a duplicate request in the same handler. Then clear peer/IBD caches. |
| LOAD_ALL_KEYS | Emit start/progress/finish listener events while completing keys one at a time; then apply the requested key-use count. |
| CALLCHECKER | Read an optional timer flag and log a diagnostic. |

Some declared message constants have no matching branch in this method. Unknown message types reach the end without an action. Comments about “20 minutes” do not override the actual 30-minute P2PNETMDS_TIMER expression.

The constructor uses different **initial** delays for some periodic tasks: RAM cleanup starts after ten seconds for genesis or three minutes otherwise; SQL cleanup after ten minutes; initial key continuation after 30 seconds; ordinary backup after five minutes. Later handlers use their recurring intervals.

## Shutdown, restore and restart are separate paths

`shutdown(compact)` first rejects an already-active shutdown, then sets the shutdown flag. Inside its try block it tells the wallet to stop key work, stops general services, stops final services, saves all databases with the requested compact flag, stops/waits for Main's own processor, and notifies the listener. A caught Exception logs failure; the method does not roll back the flag or automatically complete the remaining steps.

`shutdownGenProcs` stops the timer processor, asks the network to shut down, and stops miner and send-poll processors. It waits in 250 ms increments for network completion; only after the accumulated wait is **greater than** 10,000 ms does it force hard shutdown. `shutdownFinalProcs` optionally shuts NotifyManager down once, then stops and waits for TxPoWProcessor.

`NotifyMainListenerOfShutDown` clears the singleton first, then uses a flag to avoid repeating the listener event. `setHasShutDown` only sets the shutdown flag. It does not perform the shutdown sequence. Similar-looking methods are not interchangeable.

`restoreReady` sets restoring and stops services without saving everything as a normal shutdown would. `restoreReadyForSync` creates a replacement processor and reloads restore-sync databases. `archiveResetReady` selectively saves/deletes/reloads archive, TxPoW and optionally wallet storage, and can reset the cascade/tree. These are real persistence operations still needing a native backend.

`resetMemFull` restarts database resources, replaces the processor and requests garbage collection. `restartNIO` returns during shutdown; otherwise it takes the database read lock, shuts down/waits for networking, waits another five seconds and creates a new NetworkManager. Its finally block releases the lock even after a caught Exception.

## Notifications, getters and small controls

Getters expose the owned managers and flags. Uptime is the current wall clock minus the stored start time. Normal and low-power mining setters select 50-second and 500-second intervals directly. `runSingleMinimaCMD` trims its input before delegating. `setSyncIBD` optionally logs before assigning its flag.

`PostNotifyEvent` wraps event/data into JSON and posts only for recipient `*` when NotifyManager exists. It does not notify every possible listener merely because its comment names them. The listener-only overload with no supplied data can propagate an Exception from the listener; the overload accepting data catches and suppresses Exception. That asymmetry belongs in the port and its tests.

## How the native owner is assembled

The [Main header](../../../../../../src/org/minima/system/Main.hpp) and body reuse MinimaDB, NotifyManager, TxPoWProcessor, TxPoWMiner, NetworkManager and SendPollManager. `bindRuntime` connects NIO dispatch, greetings, IBD, command receivers and logging to the actual Main-owned components. The SQL interfaces require a supplied connection implementation; a missing driver must fail explicitly. The MegaMMR snapshot receiver still needs its command implementation.

C++ cannot safely start a worker holding shared ownership during an ordinary constructor. `create` first allocates the object, publishes its shared owner, binds callbacks, then starts the processor and runs the startup body. If startup throws, it stops and joins the owners already created, clears the failed singleton and rethrows. It keeps the timer it actually created so failure before timer construction cannot mistakenly require a timer during cleanup.

Owner assignment is also observable native work: destroying the previous object may join a worker or call Main again. The implementation swaps owners under a mutex and destroys retired owners after unlocking. The listener registry follows the same rule. A native test uses a listener destructor that calls back into the registry; releasing that object under the mutex would deadlock.

Atomic flags and mutex-protected shared owners replace unsafe native data races. This does not certify every Java thread interleaving. The managed-thread helper retains each worker's diagnostic name while its body and uncaught-exception handler run; nested workers restore the previous name. Native RAII manages memory, with an optional collection hint replacing JVM `System.gc`. Native library loading and process termination are explicit platform boundaries.

## Understanding the executed tests

The [runtime lab](../../../../../new-tests/main-runtime.md) was written before the tests. The [comparison driver](../../../../../../tools/main_runtime_differential.py) runs [Java](../../../../../../support/MainRuntimeOracle.java) and [C++](../../../../../../support/main_runtime.cpp) fixtures over the same operations. Both call actual Main method bodies with unstarted workers and real MinimaDB child objects. The recording SQL driver supplies rows and records calls; it does not execute or persist SQL.

All **336 programs and 897 operations** match in normal and ASAN/UBSAN builds. Each operation records flags, errors, logs, timers, SQL effects, listener calls and retained JSON. Uptime and timer deadlines must fall within measured wall-clock windows before their clock-dependent values are normalized. Listener tests distinguish the two overloads, nullable data, retained mutations and shutdown notification after singleton clearing. Message cases test shutdown/restoring gates, missing owners, rescheduling before failure, exact message boxes and repeated checker calls.

The MySQL configuration helper and the single-TxPoW backup branch use the existing MySQLConnect implementation. Cases include malformed configuration, Unicode and null credentials, wrong preference types, negative delay, real TxPoW encoding and injected SQL/runtime failures. This tests Main's call order and catch scope without asserting that a production MySQL driver exists.

Two discovered discrepancies make useful debugging examples. A Boolean preference containing a String produced native `bad_variant_access`; Java throws ClassCastException. A credential preference containing a Boolean exposed the same problem for strings. JsonDB now explicitly checks those types. Message getters likewise preserve null-unboxing versus wrong-box failures. Tests compare exception classes while allowing runtime-specific cast/null diagnostic prose to differ.

The [native ownership test](../../../../../../support/main_ownership.cpp) separately exercises the real `Main::create` factory. One failure occurs while loading a library, before any timer; another occurs at the first SQL connection. The supplied halt/exit callbacks throw only in this fixture. Assertions verify publication before SQL, cleared ownership after failure, released worker references, reentrant listener destruction and nested thread names. Only a private temporary folder is created; networking and key generation are never reached.

## What remains to validate

A successful startup fixture must still check private first-run/CLEAN behavior, every initialization stage, genesis insertion, initial timers and full worker shutdown. Restart tests need the strict 10-second timeout, interrupted-sleep behavior and database-lock release. Enabled backup/rescue commands require their own implementations. Real persistence across restart, a native SQL backend, a native MySQL driver, the Minima entry point and compatible peer behavior remain necessary for a working node. These finite comparisons do not certify the complete Main API, every reference alias, platform behavior or concurrency.

Pulse prerequisites retain their separate evidence: 152 programs and 946 operations in both builds verify actual DB locking, tree/pulse list identity, list replacement and partial decoding. That evidence supports MAIN_PULSE's dependency; it does not prove live network delivery.

## Check your understanding

Why does a PULSE message on a slave node still affect the timer queue? **Answer:** it schedules its successor before the slave-mode return. Why is setting the shutdown flag insufficient to implement shutdown? **Answer:** saving storage, stopping/waiting for workers, clearing ownership and notifying listeners are separate later effects.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [CoinDB](../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java), [TxPoWTreeNode](../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [UserDB](../../../../../../../upstream-study/src/org/minima/database/userprefs/UserDB.java), [ScriptRow](../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Wallet](../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Pulse](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java), [TxBlock](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [TxPoW](../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [TxPoWMiner](../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java), [TxPoWProcessor](../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWProcessor.java), [CommandRunner](../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [mysql](../../../../../../../upstream-study/src/org/minima/system/commands/backup/mysql.java), [SendPollManager](../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollManager.java), [GenesisMMR](../../../../../../../upstream-study/src/org/minima/system/genesis/GenesisMMR.java), [GenesisTxPoW](../../../../../../../upstream-study/src/org/minima/system/genesis/GenesisTxPoW.java), [NetworkManager](../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java), [NIOManager](../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [NIOMessage](../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java), [P2PFunctions](../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java), [NotifyManager](../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java), [GeneralParams](../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [GlobalParams](../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [MiniFile](../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MiniFormat](../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageListener](../../../../../../../upstream-study/src/org/minima/utils/messages/MessageListener.java), [MessageProcessor](../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java), [TimerMessage](../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java), [TimerProcessor](../../../../../../../upstream-study/src/org/minima/utils/messages/TimerProcessor.java), [MySQLConnect](../../../../../../../upstream-study/src/org/minima/utils/mysql/MySQLConnect.java).

Method navigation (declarations with bodies found in this file): [getInstance](../../../../../../../upstream-study/src/org/minima/system/Main.java#L67), [getMinimaListener](../../../../../../../upstream-study/src/org/minima/system/Main.java#L75), [setMinimaListener](../../../../../../../upstream-study/src/org/minima/system/Main.java#L78), [Main](../../../../../../../upstream-study/src/org/minima/system/Main.java#L236), [isStartUpComplete](../../../../../../../upstream-study/src/org/minima/system/Main.java#L448), [runSingleMinimaCMD](../../../../../../../upstream-study/src/org/minima/system/Main.java#L452), [setSyncIBD](../../../../../../../upstream-study/src/org/minima/system/Main.java#L463), [isSyncIBD](../../../../../../../upstream-study/src/org/minima/system/Main.java#L470), [setHasShutDown](../../../../../../../upstream-study/src/org/minima/system/Main.java#L477), [isShuttingDown](../../../../../../../upstream-study/src/org/minima/system/Main.java#L481), [isRestoring](../../../../../../../upstream-study/src/org/minima/system/Main.java#L485), [isShuttongDownOrRestoring](../../../../../../../upstream-study/src/org/minima/system/Main.java#L489), [shutdown](../../../../../../../upstream-study/src/org/minima/system/Main.java#L493), [shutdown](../../../../../../../upstream-study/src/org/minima/system/Main.java#L497), [ClearMainInstance](../../../../../../../upstream-study/src/org/minima/system/Main.java#L546), [NotifyMainListenerOfShutDown](../../../../../../../upstream-study/src/org/minima/system/Main.java#L554), [setStartUpError](../../../../../../../upstream-study/src/org/minima/system/Main.java#L573), [isStartupError](../../../../../../../upstream-study/src/org/minima/system/Main.java#L578), [getStartupErrorMsg](../../../../../../../upstream-study/src/org/minima/system/Main.java#L582), [restoreReady](../../../../../../../upstream-study/src/org/minima/system/Main.java#L586), [restoreReady](../../../../../../../upstream-study/src/org/minima/system/Main.java#L590), [restoreReadyForSync](../../../../../../../upstream-study/src/org/minima/system/Main.java#L601), [archiveResetReady](../../../../../../../upstream-study/src/org/minima/system/Main.java#L610), [archiveResetReady](../../../../../../../upstream-study/src/org/minima/system/Main.java#L614), [shutdownGenProcs](../../../../../../../upstream-study/src/org/minima/system/Main.java#L652), [shutdownFinalProcs](../../../../../../../upstream-study/src/org/minima/system/Main.java#L679), [shutdownFinalProcs](../../../../../../../upstream-study/src/org/minima/system/Main.java#L683), [resetMemFull](../../../../../../../upstream-study/src/org/minima/system/Main.java#L703), [restartNIO](../../../../../../../upstream-study/src/org/minima/system/Main.java#L720), [setNormalAutoMineSpeed](../../../../../../../upstream-study/src/org/minima/system/Main.java#L773), [setLowPowAutoMineSpeed](../../../../../../../upstream-study/src/org/minima/system/Main.java#L779), [isNormalMineMode](../../../../../../../upstream-study/src/org/minima/system/Main.java#L784), [getUptimeMilli](../../../../../../../upstream-study/src/org/minima/system/Main.java#L788), [getNetworkManager](../../../../../../../upstream-study/src/org/minima/system/Main.java#L792), [getNIOManager](../../../../../../../upstream-study/src/org/minima/system/Main.java#L796), [getNotifyManager](../../../../../../../upstream-study/src/org/minima/system/Main.java#L800), [getTxPoWProcessor](../../../../../../../upstream-study/src/org/minima/system/Main.java#L804), [getTxPoWMiner](../../../../../../../upstream-study/src/org/minima/system/Main.java#L808), [getSendPoll](../../../../../../../upstream-study/src/org/minima/system/Main.java#L812), [doGenesis](../../../../../../../upstream-study/src/org/minima/system/Main.java#L816), [getAllKeysCreated](../../../../../../../upstream-study/src/org/minima/system/Main.java#L840), [getAllDefaultKeysSize](../../../../../../../upstream-study/src/org/minima/system/Main.java#L844), [processMessage](../../../../../../../upstream-study/src/org/minima/system/Main.java#L849), [PostNotifyEvent](../../../../../../../upstream-study/src/org/minima/system/Main.java#L1318), [PostNotifyEvent](../../../../../../../upstream-study/src/org/minima/system/Main.java#L1322), [NotifyMainListenerOnly](../../../../../../../upstream-study/src/org/minima/system/Main.java#L1342), [NotifyMainListenerOnly](../../../../../../../upstream-study/src/org/minima/system/Main.java#L1359).

## Traceability

Pinned Java SHA-256: `143e67f90b0941fa9c7bd8626e6092396d64c0cb85fd5e9a5346132daf18ffa1`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Status command runtime integration

The status command now reuses the MinimaLogger Runtime's allocator memory
callbacks. Its added collectMemory callback is a platform hint with a no-op
default; Main binds its existing collection service during runtime setup.
The command calls the hint for every present clean key before parsing other
parameters or reading Main. C++ ownership does not provide JVM heap collection
or an equal heap size. The status comparison independently validates native
controlled bytes/hint counts and Java heap-format bounds. Existing logging
formatting and notification behavior are unchanged.

## RPC pause binding in progress

Main binds its existing sleep service to the rpc command. The command stops the
network RPC listener before the one-second pause, then changes configuration.
An injected interruption throws at the pause, before those writes. The Java
comparison interrupts the actual calling thread; the native fixture supplies the
matching exception through the service. Whole Java thread-interruption behavior
and live RPC concurrent-state validation remain open.
