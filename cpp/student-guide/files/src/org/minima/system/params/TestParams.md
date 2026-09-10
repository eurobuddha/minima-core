# TestParams — A lazily captured test profile

**Teaching status:** Authored source algorithm, native adaptation and test explanation; complete parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/params/TestParams.java](../../../../../../../../upstream-study/src/org/minima/system/params/TestParams.java) | [src/org/minima/system/params/TestParams.cpp](../../../../../../../src/org/minima/system/params/TestParams.cpp) |

## What this file does

TestParams stores a second set of chain parameters and provides `setTestParams` to copy them into GlobalParams. It does not start a test network by itself. Its default speed is `0.05` blocks per second, its speed window is 16, cascade frequency 3, start depth 32, nodes per level 4, levels 32, confirmation depth 3, proof history 8 and median window 8.

## Follow the copy operation

`setTestParams` assigns ten global fields: speed, speed window, cascade frequency, nodes per level, levels, start depth, confirmation depth, proof history, version and median window. The copied values come from the **current TestParams fields**, so a caller can change the profile before applying it. It does not restore every GlobalParams field, touch GeneralParams flags or enable network services.

The `test` and `solo` consumers in [ParamConfigurer](ParamConfigurer.md) both select this profile when their argument is exactly `true`. The `testchainlength` consumer changes the stored test depth and, if TEST_PARAMS is already true, applies the profile again. This makes the profile and the currently active globals two related but distinct sets of values.

## Why the native class has `values()`

Java initializes TestParams on first active use. Its version initializer captures `GlobalParams.MINIMA_VERSION + "-TEST"` at that time. Rebuilding this string on every call would be wrong; eagerly capturing it before startup mutations could also be wrong.

The native `values()` uses a function-local static settings object, initialized on first use. The subprocess fixture sets the global version to `before`, reads the test version, changes the global version to `after`, and applies the profile. Both Java and C++ print `before-TEST` twice. The second line demonstrates that applying the profile reuses its stored version instead of recalculating it from `after`.

Java copies MiniNumber references into global fields; C++ copies MiniNumber values. The described numeric observations do not prove complete mutable-reference alias parity or Java class-loader behavior.

## Test evidence shared by these settings

[ParamConfigurerOracle.java](../../../../../../../support/ParamConfigurerOracle.java) snapshots the actual Java static fields after each operation. [param_configurer.cpp](../../../../../../../support/param_configurer.cpp) uses [ParamSnapshots.hpp](../../../../../../../support/ParamSnapshots.hpp) to observe the corresponding native fields. Before each scenario, both harnesses restore a captured baseline; a `reset` operation inside a scenario calls the production `GeneralParams.resetDefaults` instead. Those are different operations.

The [comparison driver](../../../../../../../tools/param_configurer_differential.py) tests argument, environment and file inputs, repeated configuration, reset, test-setting overrides, malformed UTF-8 and numbers, map traversal, filesystem effects and loopback HTTP. Existing normal and sanitizer [reports](../../../../../../../build/param-configurer-results.json) each record **719 scenarios, 67 handlers, four HTTP requests per runtime and three subprocess checks**, with zero failures. Those are suite totals shared by this family, not separate totals for each file. This documentation pass read the reports; it did not rerun that suite.

The driver supplies Java's observed enum identity hashes to the native map so both traverse the same arbitrary identity ordering. It normalizes logger prefixes and selected exception diagnostics while retaining their categories. The fixtures do not prove all JVM initialization, sharing, exception or concurrency semantics, and do not start a complete native node.

## Dependencies and reading map

Direct Minima imports: [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [setTestParams](../../../../../../../../upstream-study/src/org/minima/system/params/TestParams.java#L69).

## Check your understanding

After the profile initializes, you change GlobalParams.MINIMA_VERSION. Does TestParams.MINIMA_VERSION change by itself? **Answer:** no. What does `setTestParams` do next? **Answer:** it copies the stored test version back into the global version, along with the other nine profile fields.

## Traceability

Pinned Java SHA-256: `0e7df0a4a2257497a76f633158f90dd68db4028fd4c8f6ab8ebf101b54ad83c8`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
