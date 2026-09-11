# Reuse gate

Read the seven pinned Java sleep/catch sites in TimedChecker.java and backup
archive.java, mysql.java, restoresync.java, vault.java, mmrsync/megammr.java and
mmrsync/megammrsync.java under upstream-study/src/org/minima/system. Read the
matching native helpers/callers, TimedChecker header/body, and the shared
cpp/src/support/NativeThread.hpp/.cpp and MySQLConfiguration.hpp.

Reuse NativeThread's tested pending state, exact exception category and sleep.
Add the two missing source catches and retain TimedChecker's current worker
with shared ownership plus a mutex. Existing validation inputs/ancestor ownership,
source 120000 ms deadline and retained finished/valid flags remain unchanged.

Existing tests read/reused: cpp/support/timed_checker_errors.cpp,
TimedCheckerErrorsOracle.java, cpp/tools/timed_checker_errors_differential.py;
cpp/support/runtime_sleep.cpp, RuntimeSleepOracle.java, MainFixture.hpp,
SystemCheckFixture.hpp, SqlRecordingFixture.hpp and SqlDBOracle.java;
WalletVaultFixture's existing friend access to STOP_NEWKEYS. New runtime modes
reuse the actual Main, in-memory DB objects, archive request encoder, vault body
and MySQL converter. New database calls use the existing recording driver.
No new runtime dependency or replacement algorithm was introduced.

Sibling searches inspected src/system/MessageProcessor.hpp and
src/network/P2PSync.hpp in /Users/eurobuddha/Projects/minima/tools/minima-core-cpp.
The former has ordinary stop/condition-variable behavior and the latter a raw
backoff sleep, not a compatible Java pending-interruption implementation.
The maintained current NativeThread implementation is the compatible candidate.

The initial sibling lookup used two stale paths; rg --files located the actual
nested paths before inspection. No source or implementation was invented from
those failed lookups. The Java runtime fixture now calls getWebHooks to match
native NotifyManager construction; its full preferences comparison is retained.
