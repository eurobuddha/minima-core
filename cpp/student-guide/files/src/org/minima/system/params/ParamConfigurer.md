# ParamConfigurer — Parsing input, applying settings, and preserving side effects

**Teaching status:** Authored source algorithm, native adaptation and test explanation; complete parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/params/ParamConfigurer.java](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java) | [src/org/minima/system/params/ParamConfigurer.cpp](../../../../../../../src/org/minima/system/params/ParamConfigurer.cpp) |

## What this file does

ParamConfigurer turns configuration-file entries, environment variables and command-line tokens into a map of 67 recognized parameter keys. It later applies each key's consumer to shared settings. It also owns two receiver flags: daemon starts false and the shutdown hook starts true. The methods return the same receiver so callers can chain them.

Separate two phases when reading it: **collecting values** changes the private parameter map; **configuring** invokes consumers that change globals, create folders, log, fetch a peer list, or terminate a process. `checkParams` actually performs both phases, so it is not a side-effect-free validator.

## The three input paths

1. `usingConfFile` searches the supplied tokens for the first exact `-conf` and uses the following token as a filename. Each UTF-8 line is split at equals signs; only the first two pieces matter. Keys and values are not generally trimmed. Unknown keys are ignored. Recognized duplicate keys fail during collection, before the collected map is merged into the receiver. A missing value becomes an empty string. A line consisting only of equals signs triggers the source array-index failure after Java discards trailing empty split pieces.
2. `usingEnvVariables` lowercases names using the default locale and selects the `minima_` prefix. It removes that prefix, substitutes an empty string for a null value, and then resolves recognized keys. Duplicate names after lowercasing can fail even when the eventual key would be unknown. Values retain their case. Turkish locale is therefore part of the fixture matrix.
3. `usingProgramArgs` visits every token. A token beginning with a dash must name a recognized key after removing one dash, or it throws UnknownArgumentException. A recognized key takes the following token only if that token is not another parameter; otherwise its value is `true`. Looking ahead can itself throw for an unknown dash-prefixed token. Negative numeric text such as `-1` is consequently interpreted as an unknown option in this route. Environment input can deliver that signed value directly to a numeric consumer.

When a caller invokes file, environment and argument methods in that order, later writes to the same parameter replace earlier values. The precedence comes from the call sequence, not a separate priority engine. For example, file port 999, environment port 8888 and argument port 7777 leave 7777 in the map before configure runs.

## Applying values is not argument-order execution

`configure` traverses the HashMap and invokes each consumer. Java enum keys use identity hashes; traversal is neither command-line order nor guaranteed enum declaration order. Several consumers write overlapping fields, such as desktop/server/mobile link policy. The native map reuses Java-style hashing/traversal with process-stable enum identities. Tests bind the Java process's actual enum hashes to the native map to compare one identical ordering.

An exception during a consumer can leave earlier consumers' effects in place. Configure has no rollback. Repeating it applies the retained map again. When SHOW_PARAMS is true, it logs a heading and each retained key/value after applying consumers. A consumer that returns without changing a field does not reset that field.

## Read the consumer families precisely

The complete source key names/help strings are retained in [ParamConfigurerKeys.inc](../../../../../../../src/org/minima/system/params/ParamConfigurerKeys.inc); the 67 translated bodies are in [ParamConfigurerConsumers.inc](../../../../../../../src/org/minima/system/params/ParamConfigurerConsumers.inc). Use this map to understand their different rules:

| Family | Representative behavior to preserve |
|---|---|
| Paths: data, basefolder, mdsinit | Create directories, ignore mkdirs' boolean result, then record an absolute path. Data adds the base-version subfolder. |
| Host and port | Host also sets IS_HOST_SET. Port parses an int but does not recompute other ports here. |
| RPC | `rpc` only logs its deprecation message. rpcpassword enables authentication; rpcenable and rpccrlf assign either true or false based on exact `true`. |
| Logging and device flags | Some flags only act for exact `true`; desktop/mobile also disable incoming links, while server enables them. The two network-call options differ in whether poll messages are shown. |
| Discovery and connections | connect disables P2P and records its list. p2p2 enables P2P2 and disables P2P. p2pnodes fetches values beginning with `http` using RPCClient, catching I/O failure and logging; other values are stored directly. |
| Startup modes | genesis sets CLEAN and GENESIS. solo sets PRIVATE and TEST_PARAMS, applies the test profile and disables P2P. slavenode sets five fields for its specific mode. |
| Test profile | test applies TestParams; testchainlength trims/parses a number, updates the stored profile and reapplies it only when test mode is already active. |
| Numeric options | port and mysqldbdelay parse their text directly. hashtest and txpowdbstore trim first. txpowdbstore clamps values below 3 to 3 and logs. |
| Value-independent switches | noshutdownhook disables its receiver flag regardless of text. allowgenmessage, publicmds, enablejni, anyseed and dbignorelocks enable their fields; nossl disables SSL generation. |
| Metadata and credentials | Some values are retained exactly (seed, database details, passwords where the body says so); mdspassword, publicmdsuid and rescuenode trim. publicmdsuid also enables public MDS. |
| Pruning and storage flags | Exact-true branches enable their specific flags. Pruning state or tokens also enables the main prune flag. Flags alone do not implement storage or delete coins here. |
| Help | Prints all source help rows in enum order and exits with status 1. It is not a successful configure return. |

The text overload of `checkParams` splits on the literal space character, trims each resulting token and drops empties. It does not implement shell quoting; a tab inside a token is not a separator. Tokenization occurs outside the array overload's exception-catching boundary. The array overload creates a new configurer, parses, configures and returns false when it catches an Exception. Global changes already made remain observable.

## Native adaptations and known limits

The implementation reuses optional UTF-16 strings, Java-style maps, integer parsing, locale-aware lowercasing, Unix file helpers, strict UTF-8 decoding, RPCClient and the actual logger. A Runtime object supplies enum hashes, printing and process exit hooks. Tests can observe those effects without replacing the parameter consumers with fake setters.

Opening a configuration file can produce checked I/O failure, which prints the source message and exits. Failure while traversing/decoding its lines is wrapped as UncheckedIOException and follows a different path. The native reader closes its descriptor with scope-based cleanup; the source Files.lines stream is not explicitly closed here. Full Java resource-lifetime semantics are not established.

The native array `checkParams` still catches std::exception broadly, which includes native representations of some Java Error categories that Java's `catch(Exception)` would not catch. Reentrant map modifications and arbitrary JVM globals also need further review. Tests normalize selected diagnostics; they cannot certify every message or receiver/null case.

## The original five-method test

The [original Java test](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java) checks an empty configuration, a combined argument set, rejection of an unknown option, environment filtering and file/argument precedence. The [mapped native test](../../../../../../../test/org/minima/system/params/ParamConfigurerTest.cpp) uses the same active expectations with a supplied temporary file path. The Java runner resets GeneralParams before each original method and honors the method's expected-exception annotation. The differential suite adds boundaries and side-effect observations beyond those five methods.

## Test evidence shared by these settings

[ParamConfigurerOracle.java](../../../../../../../support/ParamConfigurerOracle.java) snapshots the actual Java static fields after each operation. [param_configurer.cpp](../../../../../../../support/param_configurer.cpp) uses [ParamSnapshots.hpp](../../../../../../../support/ParamSnapshots.hpp) to observe the corresponding native fields. Before each scenario, both harnesses restore a captured baseline; a `reset` operation inside a scenario calls the production `GeneralParams.resetDefaults` instead. Those are different operations.

The [comparison driver](../../../../../../../tools/param_configurer_differential.py) tests argument, environment and file inputs, repeated configuration, reset, test-setting overrides, malformed UTF-8 and numbers, map traversal, filesystem effects and loopback HTTP. Existing normal and sanitizer [reports](../../../../../../../validation/checkpoint-361/normal/param-configurer-results.json) each record **719 scenarios, 67 handlers, four HTTP requests per runtime and three subprocess checks**, with zero failures. Those are suite totals shared by this family, not separate totals for each file. This documentation pass read the reports; it did not rerun that suite.

The driver supplies Java's observed enum identity hashes to the native map so both traverse the same arbitrary identity ordering. It normalizes logger prefixes and selected exception diagnostics while retaining their categories. The fixtures do not prove all JVM initialization, sharing, exception or concurrency semantics, and do not start a complete native node.

## Dependencies and reading map

Direct Minima imports: [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [P2PFunctions](../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java), [P2PParams](../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [RPCClient](../../../../../../../../upstream-study/src/org/minima/utils/RPCClient.java).

Method navigation (declarations with bodies found in this file): [usingConfFile](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L35), [usingEnvVariables](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L64), [usingProgramArgs](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L78), [configure](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L94), [lookAheadToNonParamKeyArg](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L108), [progArgsToParamKey](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L117), [isDaemon](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L125), [checkParams](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L129), [checkParams](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L147), [isShutDownHook](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L160), [UnknownArgumentException](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L539), [Pair](../../../../../../../../upstream-study/src/org/minima/system/params/ParamConfigurer.java#L548).

## Check your understanding

1. Why can `-port -1` fail before Integer.parseInt runs? **Answer:** lookahead treats `-1` as an option and rejects the unknown key.
2. Does `-noshutdownhook false` keep the hook enabled? **Answer:** no; that consumer ignores the value and disables the hook.
3. Why does a failed configure call require a state snapshot? **Answer:** earlier consumers may already have changed globals or produced effects.
4. Why supply enum hashes in the differential fixture? **Answer:** to compare the same arbitrary map traversal order without pretending that argument order is the source contract.

## Traceability

Pinned Java SHA-256: `3a26dee339469d087d54e828bf90fd2ffd281d7a80fe952210fc6c41ade68570`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
