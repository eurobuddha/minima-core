# Minima — the launcher and command-line lifecycle

**Teaching status:** Authored explanation of the complete Java body, missing native integration and proposed validation. No completed native runtime is claimed.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/Minima.java](../../../../../../upstream-study/src/org/minima/Minima.java) | `src/org/minima/Minima.cpp` (planned) |

## What this file does

Minima is the application's outer launcher and embedding interface. [Main](system/Main.md) owns the running node; this class prepares settings, constructs Main, arranges shutdown, and handles terminal input. Its native counterpart is still absent, so the following is a source explanation and a plan for validating the eventual translation.

`mainStarter` starts a separate unnamed Java thread whose body calls static `main` with the supplied argument array. `getMain` forwards to the current Main singleton. The `runMinimaCMD` overloads default to pretty JSON and user ID `0x00`, then call the shared CommandRunner. The method computes a trimmed local string but passes the original input to `runMultiCommand`; a port must not silently substitute that local variable. Pretty mode formats the entire result array. Compact mode unwraps exactly one result into an object string; zero or multiple results remain an array.

## Startup in source order

1. Set the running flag, derive the default `~/.minima/<base-version>` folder, call GeneralParams.resetDefaults, and assign the folder path.
2. Collect the configuration file, environment and command-line arguments in that order, then apply them. Only UnknownArgumentException is caught at this boundary; it is printed and the process exits with status 1. Other failures are not covered by that catch.
3. Ensure the chosen data directory exists, read daemon/shutdown-hook flags, and recompute MDS file, MDS command and RPC ports as main port +2, +3 and +4. This is where changing `-port` updates those related ports.
4. Print the welcome/version banner. Attempt to load the H2 and MySQL drivers, with the specific catches in the source. Install MinimaUncaughtException as the default thread handler.
5. Construct Main. If enabled, register a shutdown hook that skips work when Main is already shutting down, otherwise logs and calls shutdown.

Each step can affect later steps. The existing [ParamConfigurer lesson](system/params/ParamConfigurer.md) documents the actual parsing and precedence behavior. Driver loading is a real runtime dependency: compiling SQL interfaces or an exception named MVStoreException does not supply an H2 storage engine.

## Daemon mode and terminal mode

Daemon mode logs its start and sleeps in 250 ms intervals until Main reports shutdown complete. InterruptedException during those sleeps is ignored. It then returns; it does not create the stdin readers.

Terminal mode creates a charset-aware InputStreamReader and BufferedReader. While both the launcher flag and Main's running state allow it, the loop reads a line. A non-null, nonempty line is trimmed. It checks that the Main singleton still exists, runs the command batch, and prints pretty results unless the trimmed command text is exactly `quit`. It also scans the returned result objects and leaves the loop if any command field equals `quit`, allowing a multi-command result to end input processing.

The source does not break merely because readLine returns null at EOF. It simply goes around the loop again. An eventual port must expose and test that behavior instead of assuming a conventional EOF exit. Exceptions in the body are logged; failures raised by that logging call can still escape its catch body. After the loop, it attempts to close both readers, logs an I/O close failure, and logs that CLI input stopped.

## What can be reused, and what is missing

Native implementations already exist for configuration, CommandRunner, JSON formatting, the logger and the uncaught-thread boundary. Their individual tests are explained in their linked lessons. They must be connected to an actual native Main instance and storage provider before this launcher can implement the source lifecycle. The existing native RPC client executable is a client of another node; it is not this missing launcher.

No completed native Minima test result is claimed. A file bearing this class's name, a command stub, or a process that prints a banner would not fulfill the source's startup and shutdown responsibilities.

## Proposed tests for this file

Use isolated subprocesses and temporary data directories when a future native runtime exists. Compare configuration precedence and derived ports; inspect the precise catch/exit behavior for unknown arguments and other failures; exercise daemon completion; feed terminal commands with zero, one and multiple results; and check single versus batched quit behavior. Drive EOF under a bounded observation window so the test records the source loop instead of hanging indefinitely. Embedding tests should compare compact JSON shape and the original-versus-trimmed input passed to CommandRunner.

Shutdown tests must observe which receiver is used by the hook, whether a repeated request is skipped, the order of reader closure, and whether the process actually terminates. These are proposed validation cases, not executed evidence.

## Check your understanding

Why is `runMinimaCMD(" x ", false)` not fully specified by knowing what command `x` means? **Answer:** the wrapper also defines which input text reaches the runner and whether the resulting JSON is an object or an array. Why can the launcher not yet be declared ported from passing command tests? **Answer:** it also owns process startup, node construction, input handling and shutdown.

## Dependencies and reading map

Direct Minima imports: [MiniString](../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Main](../../../../../../upstream-study/src/org/minima/system/Main.java), [CommandRunner](../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [GeneralParams](../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [GlobalParams](../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [ParamConfigurer](../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java), [MiniFormat](../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [MinimaUncaughtException](../../../../../../upstream-study/src/org/minima/utils/MinimaUncaughtException.java), [JSONArray](../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Minima](../../../../../../upstream-study/src/org/minima/Minima.java#L28), [mainStarter](../../../../../../upstream-study/src/org/minima/Minima.java#L33), [run](../../../../../../upstream-study/src/org/minima/Minima.java#L38), [getMain](../../../../../../upstream-study/src/org/minima/Minima.java#L52), [runMinimaCMD](../../../../../../upstream-study/src/org/minima/Minima.java#L60), [runMinimaCMD](../../../../../../upstream-study/src/org/minima/Minima.java#L64), [runMinimaCMD](../../../../../../upstream-study/src/org/minima/Minima.java#L68), [main](../../../../../../upstream-study/src/org/minima/Minima.java#L95), [run](../../../../../../upstream-study/src/org/minima/Minima.java#L188).

## Traceability

Pinned Java SHA-256: `8aa046e25cc30190f3ff96434608f05d2eb6cf5c928c1807a8ef063b6de81d97`. Mapping: [source-map.json](../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../REVIEW.md).
