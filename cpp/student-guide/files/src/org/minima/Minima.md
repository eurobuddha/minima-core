# Minima — the launcher and command-line lifecycle

**Teaching status:** Authored explanation of the complete Java body, current native boundary tests and remaining runtime integration. No completed native node is claimed.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file compiles; real solo startup now has bounded integration evidence, while full-node parity remains unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/Minima.java](../../../../../../upstream-study/src/org/minima/Minima.java) | [src/org/minima/Minima.cpp](../../../../../src/org/minima/Minima.cpp) |

## What this file does

Minima is the application's outer launcher and embedding interface. [Main](system/Main.md) owns the running node; this class prepares settings, constructs Main, arranges shutdown, and handles terminal input. Its native counterpart now connects the existing configuration, runner, logger and Main APIs. An optional native H2 provider now supplies real storage through MainServices.sql.

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

Native implementations already exist for configuration, CommandRunner, JSON formatting, the logger and the uncaught-thread boundary. Their individual tests are explained in their linked lessons. The H2-enabled executable now connects them to an actual native Main and storage provider. The RPC client and this launcher now share ConsoleInput, extracted from the already-tested RPC stdin reader. The RPC client remains a client of another node.

The native executable is named MinimaCoreC++. With MINIMA_NATIVE_H2_DIR configured, its entry point supplies NativeH2Runtime().connector() to MainServices.sql. Without that build option it retains the explicit missing-provider failure.

## Proposed tests for this file

Use isolated subprocesses and temporary data directories for remaining native runtime checks. Compare configuration precedence and derived ports; inspect the precise catch/exit behavior for unknown arguments and other failures; exercise daemon completion; feed terminal commands with zero, one and multiple results; and check single versus batched quit behavior. Drive EOF under a bounded observation window so the test records the source loop instead of hanging indefinitely. Embedding tests should compare compact JSON shape and the original-versus-trimmed input passed to CommandRunner.

Shutdown tests must observe which receiver is used by the hook, whether a repeated request is skipped, the order of reader closure, and whether the process actually terminates. Daemon startup, shutdown receiver integration and shared argument mutation remain proposed validation cases; solo CLI startup now has the additional evidence below. The public API and process-boundary evidence below covers a smaller, explicit scope.

## Executed boundary tests

The [launcher lab](../../../../new-tests/minima-entry-point.md) and
[driver](../../../../../tools/minima_entry_differential.py) now compare the unchanged
pinned Minima class with the C++ class. Both normal and ASAN/UBSAN runs pass:

- 487 wrapper cases: all three overloads, null inputs/callers, exact pretty and
  compact strings, zero/one/multiple results, invalid parameters, permissions,
  semicolon stopping, whitespace, Unicode and actual hash/automine commands.
- Five real executable comparisons for help, unknown arguments and missing config.
- Three mainStarter subprocesses for null arguments, help and unknown arguments.
  They check worker name, exception category and process status. The source JVM
  retains ordinary threads; a native host explicitly calls joinStarterThreads.
  These cases do not validate successful background node startup.
- Sixteen process pairs compare native shutdown hooks with JVM Runtime hooks:
  normal return, exit, halt, HUP/INT/TERM, one/four concurrent hooks, hook exceptions
  and attempted late registration. Hook order is unspecified, so outputs are
  compared as sorted complete lines; a barrier proves the four hooks can overlap.

The singleton probe checks identity and clearing using a constructor-free Java
Main object and an unstarted native Main. It proves getMain forwards the current
owner; it does not prove Main initialization. When H2 is not configured, a separate native process must fail
with the missing SQL/Main service message and exit after cleaning up its workers.
That expected failure is a limitation assertion, excluded from parity counts.

The first comparison found that whitespace-only input escaped as a generic native
vector error. Java accesses split[0], so the shared runner now throws the matching
ArrayIndexOutOfBoundsException. The nullable caller check occurs after parameter
validation: a null caller does not prevent an invalid-parameter reply. Empty
input yields no commands, producing an empty pretty string or compact `[]`.
Only NPE diagnostic prose is normalized in wrapper comparisons; result strings
remain exact. Isolated surrogate command input still crosses the existing UTF-8
runner boundary and is not certified by these tests.

The console reader, source-order CLI/daemon loops and hook registration compile,
and solo CLI startup has now been exercised with real database/node integration.
The remaining CLI/daemon boundaries still require controlled Java comparisons. Native host process retention, arbitrary concurrent starters, signal
handler ownership under failed installation and resource exhaustion are not
established. Do not infer these from passing wrapper tests.

## Real storage integration

The [native H2 lab](../../../../new-tests/native-h2-storage.md) explains the new
provider and [startup test](../../../../../tools/native_node_startup.py). Twelve
processes start genuine Java/native Main instances, issue hash/status/quit, save
their node directories, and reopen them in the other runtime and then the original
runtime. Both plain and AES-encrypted wallets are used; the final shutdown compacts
the databases. The test checks an exact SHA3 result, stable chain/status fields,
zero peer connections and successful shutdown. It excludes dynamic memory sizes,
uptime and log ordering. This is a bounded solo-node check, not full parity.

## Check your understanding

Why is `runMinimaCMD(" x ", false)` not fully specified by knowing what command `x` means? **Answer:** the wrapper also defines which input text reaches the runner and whether the resulting JSON is an object or an array. Why can the launcher not yet be declared ported from passing command tests? **Answer:** it also owns process startup, node construction, input handling and shutdown.

## Dependencies and reading map

Direct Minima imports: [MiniString](../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Main](../../../../../../upstream-study/src/org/minima/system/Main.java), [CommandRunner](../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [GeneralParams](../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [GlobalParams](../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [ParamConfigurer](../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java), [MiniFormat](../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [MinimaUncaughtException](../../../../../../upstream-study/src/org/minima/utils/MinimaUncaughtException.java), [JSONArray](../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Minima](../../../../../../upstream-study/src/org/minima/Minima.java#L28), [mainStarter](../../../../../../upstream-study/src/org/minima/Minima.java#L33), [run](../../../../../../upstream-study/src/org/minima/Minima.java#L38), [getMain](../../../../../../upstream-study/src/org/minima/Minima.java#L52), [runMinimaCMD](../../../../../../upstream-study/src/org/minima/Minima.java#L60), [runMinimaCMD](../../../../../../upstream-study/src/org/minima/Minima.java#L64), [runMinimaCMD](../../../../../../upstream-study/src/org/minima/Minima.java#L68), [main](../../../../../../upstream-study/src/org/minima/Minima.java#L95), [run](../../../../../../upstream-study/src/org/minima/Minima.java#L188).

## Traceability

Pinned Java SHA-256: `8aa046e25cc30190f3ff96434608f05d2eb6cf5c928c1807a8ef063b6de81d97`. Mapping: [source-map.json](../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../REVIEW.md).

## Runnable local preview

The actual native entry point is available in a frozen macOS arm64 package. Its
launcher uses separate data, port 21001 and initial peer eurobuddha.com:9001. The
[packaging lab](../../../../new-tests/executable-preview.md) explains dependency
relocation, snapshot identity and the actual startup/restart verification. This
is runtime evidence for the entry point, not a complete parity declaration.
