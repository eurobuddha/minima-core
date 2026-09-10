# Lab: what happens when a command throws?

**Status:** 1,120 Java/C++ comparisons pass in normal and ASAN/UBSAN builds. All nine selected regression suites pass, including the local HTTP test rerun with socket access after its sandbox bind was rejected. Full registry and node integration remain incomplete.

## Start with the source

Read [CommandRunner.java](../../../upstream-study/src/org/minima/system/commands/CommandRunner.java) and its [per-file lesson](../files/src/org/minima/system/commands/CommandRunner.md), then compare [CommandRunner.cpp](../../src/org/minima/system/commands/CommandRunner.cpp). The runner checks parameter names and permissions before calling a command. It handles CommandException separately from other Exception objects. Java Error objects lie outside both catches.

A command can fail in more than one observable way. A normal failed reply contains status false and an error string. An escaping exception prevents the method from returning that reply at all. The command may also have produced logs or earlier side effects. A correct translation must preserve these distinctions.

## Build a controlled experiment

The [Java oracle](../../support/CommandRunnerErrorsOracle.java) executes the unchanged pinned runner with a small command whose behavior the test controls. The [native harness](../../support/command_runner_errors.cpp) uses the actual native runner and a matching command receiver. A fixture named send exercises its admin permission gate without transferring funds. Neither harness substitutes a new runner algorithm.

The command records entry, optionally emits a log, then either succeeds or throws one of nine failure cases: CommandException, IllegalStateException, IOException, NumberFormatException, ArithmeticException, LinkageError, OutOfMemoryError, SQLException, or a runtime exception carried by the SQL adapter. Each request also controls when the logger throws and what it throws.

The [comparison driver](../../tools/command_runner_errors_differential.py) now forms 1,120 requests:

| Dimension | Variations |
| --- | --- |
| Command outcome | 10: success plus nine failure cases |
| Preliminary log | 2: disabled or enabled |
| Command text | 4: alone, middle of batch, write-command batch, invalid-parameter batch |
| Caller | 2: admin 0x00 or non-admin 0x1234 |
| Logger failure | 7: none; first or second observed print throwing a runtime exception, Error, or out-of-memory error |

For each request it compares the complete JSON result or escaped exception text, the ordered command-entry events, and the ordered log bodies. Time/memory prefixes and generated stack-frame lines are excluded because they describe the particular runtime. No generic ERROR normalization is used here.

## Trace three paths by hand

**Ordinary command failure:** CommandException becomes a failed reply with its message and no exception log. Later commands do not run. An earlier successful command remains executed; there is no rollback.

**Other Exception:** IOException is logged first, then returned as a failed reply with java.io.IOException and its message. The initial native catch only wrote a message to std::clog and misclassified this I/O error in JSON. Reusing [NativeThrowable](../../src/support/NativeThrowable.hpp) supplies the existing logger adapter and the source-style exception text.

**Failure inside logging:** suppose a successful command emits a log but that print throws IllegalStateException. This failure occurs inside runCommand, so the runner catches it and tries to log that exception. If printing the exception now throws, that second failure occurs inside a catch body and escapes. A Java Error thrown by either command execution or logging also escapes. Catching every C++ std::exception would incorrectly swallow the native Error representations.

## What changed, and why

The baseline produced **450 mismatches out of 896 requests**. The correction reuses the project's existing MinimaLogger/NativeThrowable machinery and explicit JavaError/std::bad_alloc rethrow boundary. It changes the generic catch's error text to nativeExceptionText so I/O categories are retained. Parsing, permission checks, batch stopping, and the separate CommandException response remain in their original order.

The expanded programs produce **zero mismatches across 1,120 requests** in both builds. The two added outcome cases exercise SQL exception carriers; the [wallet-command lab](wallet-commands.md) explains the mismatch that prompted them. The previous runner comparison still passes 1,233 cases; command accessors pass 12,289; transaction commands pass 1,129 programs with 6,904 operations; the real HTTP handler passes 2,106 requests. Logger and notification comparisons also pass, along with mapping and native stack checks. The initial HTTP regression was blocked by the sandbox, then passed when rerun with local socket access; that environmental failure was not counted as a code pass.

## Run and interpret the evidence

After a successful build, from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^command_runner_errors_differential$'
```

Read cpp/build/command-runner-errors-results.json. The retained command-runner-errors-baseline-results.json records the pre-fix differences. Running the driver with --baseline again would overwrite that historical baseline with the currently compiled program, so use the ordinary CTest command when studying the fixed code.

These are controlled failure observations. They do not prove a complete command registry, wallet execution, arbitrary allocator exhaustion, identical native/JVM frames, or Main lifecycle integration. In particular, injected OutOfMemoryError is a typed test failure, not an attempt to exhaust the machine's memory.

## Questions and answers

1. Why record command-entry events as well as replies? **Answer:** a reply alone cannot show whether a later command incorrectly ran or an earlier effect was undone.
2. Why must invalid parameters be checked before command entry? **Answer:** validation belongs to the runner's gate; the command's body should never execute on that path.
3. Why is IOException text part of the comparison? **Answer:** Java exposes the exception's class in its failed reply; a generic RuntimeException changes observable output.
4. Why does a failure in a catch body escape? **Answer:** that catch does not catch exceptions thrown by its own body. An enclosing handler must handle them, if one exists.
