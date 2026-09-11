# Sanitized HORST completion and teardown correction

The full sanitized run failed the original five-second runner shutdown limit.
A targeted rerun preserved the primary error separately and established that all
1,438 comparisons passed, with no pending case and no primary exception. Its only
failure was native process teardown exceeding five seconds. That failed record is
retained as horst-five-second-cleanup-failure.json and its full log.

Runner.close now accepts an optional timeout, retaining five seconds by default.
Only HORST requests 60 seconds for cleanup after its full 65,536-leaf trees. It
still requires a clean process exit; timeout is a failure. The harness preserves
primary test failures and attempts cleanup of all runners instead of masking an
original failure with a cleanup exception.

The final targeted rerun passed all1,438 comparisons and clean process exit in
360.912 seconds total. No tree size, source loop, case or assertion was removed.
Full sanitizer CTest's original failure remains reported separately from this
corrected targeted pass. See horst-cleanup-corrected-results.json and log.
