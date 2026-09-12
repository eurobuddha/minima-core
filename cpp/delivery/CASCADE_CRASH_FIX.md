# Replacement development build: cascade cleanup fix

This optimized Apple Silicon build replaces the preview that crashed on
September 11 while destroying an initial-block-download cascade. It corrects
recursive shared_ptr destruction in CascadeNode. Long chains now release parent
references iteratively, while independently owned nodes retain all their links.
The earlier optimized delivery used the same faulty cleanup code as `6c28109`.

## Identify the replacement

The replacement is installed locally. `dev-2026-09-12-cascade-fix` is the planned
separate development release; it is not yet published. Its runtime manifest records the exact source commit, original binary
hash and packaged file hashes. The old release assets remain available for
traceability; upgrading to that earlier optimized release alone does not fix this
crash. Use the installed corrected app; verify its identity against
[current delivery status](CURRENT_STATUS.md).

[Validation evidence](../validation/cascade-release/README.md) and the
[student explanation](../student-guide/new-tests/cascade-release.md) cover the
unfixed SIGBUS reproduction, ownership reasoning, worker regressions and scoped
normal/optimized/sanitizer results. This release does not certify full parity or
overnight stability. There are still 456/457 mapped C++ counterparts and the
external JNI implementation remains missing.

## Fresh preview and recovery

The owner's later instruction authorizes overwriting the local preview and
starting fresh. A separate local recovery copy preserves the old executable,
libraries and wallet; every original file was byte-verified before replacement.
The new launcher uses its fresh data directory, default port 21001 and initial
peer eurobuddha.com:9001. It does not migrate or open the saved old wallet.

The current configurable app supersedes the folder launcher for interactive use.
Its data stays outside the application and ordinary mainnet starts omit `-connect`.
Use Stop node cleanly and wait for node exit before replacing it. The app is
Developer ID signed, not yet notarized. The earlier folder package uses ad-hoc
signing. Both contain the native H2 dependency compiled from Java; no JVM is launched.
