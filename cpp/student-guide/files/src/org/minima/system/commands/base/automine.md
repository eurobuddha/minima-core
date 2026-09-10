# automine — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/automine.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/automine.cpp)

## Purpose

The active automine command is a no-op after parameter parsing. Its help advertises simulated traffic, but every mining/control branch is commented out.

## Follow the command

1. Create the common reply containing command/status/pending and the supplied parameter object when nonempty.
2. Read enable with a default empty string. The shared getter trims a supplied string and rejects an explicitly blank value; the absent parameter simply returns the default.
3. Return the reply. The single/true/false branches, mining message, state changes and response fields in the file are comments and do not execute.
4. getFunction returns a fresh automine object. getValidParams is inherited as an empty list, so command-runner validation is distinct from directly populating parameters and invoking runCommand.

## Translation and dependencies

The native counterpart reuses `Command.getJSONReply`, `getParam`, inherited metadata and a fresh-command factory. It is registered in CommandRunner, as in Java. The sibling port's source catalogue had no matching automine/Command implementation; the current native command framework is the compatible tested building block. No miner dependency or settings mutation is introduced because the pinned active body has none.

## Understanding the tests

The [Java runner harness](../../../../../../../../support/CommandRunnerOracle.java), [native runner harness](../../../../../../../../support/command_runner.cpp) and [comparison driver](../../../../../../../../tools/command_runner_differential.py) add **32 programs**:

- Thirteen direct calls cover missing enable, true/false/single, trimmed ordinary text, empty/blank text, null, numeric/boolean/object/array values, and an unused parameter.
- One metadata call compares help, full help, empty valid-parameter list and a fresh factory reply.
- Eighteen runner calls combine nine command strings with two caller IDs. They include uppercase parsing, rejected enable/bad parameters, and command sequences that continue after success or stop after parameter rejection.

The baseline passes all 1,233 older cases and fails all 32 additions. For a direct call with no parameters, Java returns `{"command":"automine","status":true,"pending":false}` while the unported native registry returns null. The baseline is retained at `cpp/build/automine-baseline-results.json`.

A direct valid enable value produces a reply containing the original parameter, with no response field or mining action. Through the runner, `automine enable:true` instead produces `Invalid parameter : enable`; the runner validates the inherited empty list before invoking the body. This is why porting only the apparent business logic would miss observable behavior.

Both target builds and all **1,265 runner cases** pass, including all **32 new programs**, with zero differences. Normal CTest took **2.88 seconds**, sanitizer CTest **3.65 seconds**. Both final full builds also pass, along with shared command tests (12,289 cases), command error tests (1,120 cases) and mapping checks. Final reports use `command-runner-differential-results.json` in both build directories. Direct malformed-parameter exceptions normalize to `ERROR` in this older harness, so those comparisons do not certify exact exception messages. Shared command error suites provide separate evidence. Zero miner calls follow from the inspected active source and native body; this harness does not instrument a complete running miner.

```sh
ctest --test-dir cpp/build -R '^command_runner_differential$' --output-on-failure
ctest --test-dir cpp/build-sanitized -R '^command_runner_differential$' --output-on-failure
```


## Limits

Do not infer mining activity from the command name or commented code. A compiling empty method would still need correct envelope and parameter failure behavior.

## Check your understanding

Why can supplying enable:" " fail even though the active command never mines?

Pinned Java SHA-256: `b5784b90a1bac871b55f62e50c1932696a708390c6ea00c74cea432eb89500b3`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
