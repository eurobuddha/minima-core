# The `test` command: observing database refresh

The pinned `commands/base/test.java` command logs a message, refreshes the archive
and transaction-proof SQL databases, logs completion, and returns the ordinary
command reply. Its `show` and `action` parameters are accepted but unused. An
exception during refresh prevents the completion log. Adding automatic recovery
or interpreting those parameters would change the source's behavior.

We reuse the actual native MinimaDB refresh implementation. The existing database
fixture records SQL connection and statement calls, identities, locks, files and
logs. The new corpus compares direct calls and registered command execution,
loaded and unloaded databases, repeated refreshes, ignored parameter values and
failures at individual SQL operations. These are isolated fixture databases.

The source also contains a standalone demonstration: it generates 100,000 random
32-byte addresses and prints any whose encoded length differs from 63. We retain
all 100,000 iterations. Because Java and C++ generate different random samples,
the test requires the expected start/finish output with no reported deviations.
The envelope has39bytes and begins with byte0x01, so its unsigned integer has
305significant bits. Base32 requires61digits; the Mx prefix gives63characters.
Java and C++ need not draw the same random bytes to satisfy this invariant.

Before execution, the expected normal case is a successful reply with no response
member and both log messages. Injected failures must match the Java trace and
retained state. Both ordinary and sanitizer builds will run the same checks.

The unused private class-loader resource helper has no native resource service
and is not represented yet. The mapped file remains partial; public-command and
demonstration tests do not erase that remaining fidelity requirement.

Driver: `cpp/tools/base_test_command_differential.py`. Both complete builds pass. Both command suites pass274programs/616operations
and114independent assertions (normal30.943s, sanitized62.695s), with identical
transcript hashes. Both full address demonstrations complete100000iterations.
Actual Java/native H2 nodes also pass before/after-restart checks: four processes,
12refresh commands, eight preserved wallet keys and graceful shutdown in each
build (normal29.629s, sanitized149.797s). Native lifecycle logs use stderr, so
the integration driver preserves those logs and rejects sanitizer diagnostics
and nonzero exits; the initial over-strict stderr fixture failure is retained.
