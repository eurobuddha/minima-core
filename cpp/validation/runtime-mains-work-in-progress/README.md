# Runtime demonstration validation

Both normal and ASAN/UBSAN runs pass 18 Java/native comparisons and 121 assertions.
Added TxPoWMiner, TreeKey and Cascade demonstration entry points using existing
helpers. Miner tests retain all 2,010,000 hashes per invocation; no reduced loop.
TreeKey compares both successive fixed-seed signature JSON outputs and logger
notifications. Cascade uses the exact source Windows filename inside a temporary
working directory; missing, empty, valid and invalid-parent inputs are tested,
including unchanged file bytes and notification counts.

Corrected Cascade default validation logging and MiniFile missing-file load logging
to use MinimaLogger. Other malformed-file logging is not certified by this batch.
See [student lab](../../student-guide/new-tests/runtime-main-methods.md).

No new mapped paths: 456 counterparts; JNI absent and full parity not_verified.
Three database main candidates remain. Broader full suites run on earlier frozen
binaries and are reported separately. Sanitized H2 dependency is not instrumented.
