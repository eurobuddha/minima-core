# GlobalParams — Protocol settings and version composition

**Teaching status:** Authored source algorithm, native adaptation and test explanation; complete parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/params/GlobalParams.java](../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java) | [src/org/minima/system/params/GlobalParams.cpp](../../../../../../../src/org/minima/system/params/GlobalParams.cpp) |

## What this file does

GlobalParams holds shared version and chain parameters read by consensus and related code. The fields are mutable; they are not compile-time constants. The [TestParams lesson](TestParams.md) explains the explicit operation that replaces selected global values for test mode.

| Parameter | Pinned initial value | What the number describes |
|---|---|---|
| Base/build/micro version | `1.1`, `2`, `8` | Version components |
| Block speed | `0.02` | Target blocks per second, equivalent to 50 seconds per block |
| Speed calculation window | 256 | Number of blocks used for speed/difficulty context |
| Confirmation depth | 3 | Depth parameter used by callers |
| Cascade frequency / start depth | 100 / 1024 | Cascade scheduling and retained depth parameters |
| Cascade nodes per level / levels | 256 / 32 | Cascade shape parameters |
| MMR proof history | 256 | Permitted proof-history parameter |
| Median block calculation | 64 | Window used by block-time calculations |

This file supplies values; it does not calculate difficulty, confirm a transaction or validate an MMR proof. To understand a limit's exact inclusive/exclusive comparison, follow the code that consumes it, such as [TxPoWChecker](../brains/TxPoWChecker.md).

## Version strings are evaluated at particular times

Initialization constructs `MINIMA_VERSION` as base + `.` + build, giving `1.1.2`. `getFullMicroVersion` reads the current stored version and micro component each time it is called, giving `1.1.2.8` initially. Changing the base later does not automatically reconstruct the already stored version. Changing MINIMA_VERSION does affect the next full-version call.

The native implementation preserves the composition and uses MiniNumber for decimal/integer-valued protocol quantities. It does not turn block speed into a binary floating-point literal. Native process initialization and Java class initialization are different mechanisms; the tested first-use behavior of TestParams is adapted explicitly rather than assumed.

## Test evidence shared by these settings

[ParamConfigurerOracle.java](../../../../../../../support/ParamConfigurerOracle.java) snapshots the actual Java static fields after each operation. [param_configurer.cpp](../../../../../../../support/param_configurer.cpp) uses [ParamSnapshots.hpp](../../../../../../../support/ParamSnapshots.hpp) to observe the corresponding native fields. Before each scenario, both harnesses restore a captured baseline; a `reset` operation inside a scenario calls the production `GeneralParams.resetDefaults` instead. Those are different operations.

The [comparison driver](../../../../../../../tools/param_configurer_differential.py) tests argument, environment and file inputs, repeated configuration, reset, test-setting overrides, malformed UTF-8 and numbers, map traversal, filesystem effects and loopback HTTP. Existing normal and sanitizer [reports](../../../../../../../build/param-configurer-results.json) each record **719 scenarios, 67 handlers, four HTTP requests per runtime and three subprocess checks**, with zero failures. Those are suite totals shared by this family, not separate totals for each file. This documentation pass read the reports; it did not rerun that suite.

The driver supplies Java's observed enum identity hashes to the native map so both traverse the same arbitrary identity ordering. It normalizes logger prefixes and selected exception diagnostics while retaining their categories. The fixtures do not prove all JVM initialization, sharing, exception or concurrency semantics, and do not start a complete native node.

## Dependencies and reading map

Direct Minima imports: [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [getFullMicroVersion](../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java#L20).

## Check your understanding

If MINIMA_VERSION becomes `demo` and the micro component remains `8`, what does the getter return? **Answer:** `demo.8`. Would changing only MINIMA_BASE_VERSION reconstruct MINIMA_VERSION? **Answer:** no; its initializer is an assignment, not a live dependency.

## Traceability

Pinned Java SHA-256: `e2091a79550cfa59cdeaaef5ad0da382c975eee8feea7a84e93869c3ba6dd4ff`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
